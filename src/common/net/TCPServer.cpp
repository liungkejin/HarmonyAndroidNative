//
// Created by LiangKeJin on 2025/3/30.
//

#include "TCPServer.h"

#include <common/Log.h>
#include <common/utils/Base.h>
#include <hv/EventLoopThread.h>

NAMESPACE_DEFAULT
class TCPServerContext {
public:
    explicit TCPServerContext(TCPServer* server) : m_server(server) {
    }

    ~TCPServerContext() {
        this->stop();
    }

    void setListener(TCPServerListener* listener) {
        m_listener = listener;
    }

    bool isRunning() {
        return m_listen_io != nullptr;
    }

    std::string getHost() const {
        return m_host;
    }

    int getPort() const {
        return m_port;
    }

    bool start(const std::string& host, int port) {
        LOCK_MUTEX(m_lock);
        this->stopInternal();

        m_event_loop_thread = std::make_unique<hv::EventLoopThread>();
        m_event_loop_thread->start();
        hloop_t *loop = m_event_loop_thread->hloop();
        _ERROR_RETURN_IF(loop == nullptr, false, "tcp server hloop_new failed");

        hio_t* io = hio_create_socket(loop, host.c_str(), port, HIO_TYPE_TCP, HIO_SERVER_SIDE);
        if (io == nullptr) {
            _ERROR_RETURN_IF(io == nullptr, false, "tcp server hio_create_socket failed");
        }
        hevent_set_userdata(io, this);
        hio_setcb_accept(io, [](hio_t* io) {
            _INFO("tcp server onAccept: %p: %d", io, hio_id(io));
            TCPServerContext* ctx = (TCPServerContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onAccept(io);
            }
            else {
                _ERROR("NO context! tcp server onAccept: %p: %d", io, hio_id(io));
            }
        });
        int ret = hio_accept(io);
        if (ret != 0) {
            hio_close(io);
            _ERROR("hio_accept failed: %d", ret);
            return false;
        }

        m_listen_io = io;
        m_host = host;
        m_port = port;

        _INFO("tcp server run loop");
        hloop_run(loop);
        _INFO("tcp server start(%s:%d) success, listen io: %p: %d", host.c_str(), port, m_listen_io, hio_id(m_listen_io));
        return true;
    }

    void onAccept(hio_t* io) {
        LOCK_MUTEX(m_lock);
        TCPServerConnection* conn = new TCPServerConnection(*m_server, io);
        _INFO("onAccept[%p:%d] [%s:%d] <= [%s:%d]", io, conn->id(),
              conn->localAddr(), conn->localPort(), conn->peerAddr(), conn->peerPort());
        // 判断是否存在
        auto it = m_connections.find(io);
        if (it != m_connections.end()) {
            _ERROR("onAccept[%p:%d] [%s:%d] <= [%s:%d] already exists", io, conn->id(),
                   conn->localAddr(), conn->localPort(), conn->peerAddr(), conn->peerPort());
            // 删除之前的
            delete it->second;
            m_connections.erase(it);
        }
        m_connections[io] = conn;

        hio_setcb_close(io, [](hio_t* io) {
            TCPServerContext* ctx = (TCPServerContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onClose(io);
            }
            else {
                _ERROR("NO context! tcp server onClose: %p: %d", io, hio_id(io));
            }
        });
        hio_setcb_read(io, [](hio_t* io, void* buf, int readbytes) {
            TCPServerContext* ctx = (TCPServerContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onRecv(io, (const uint8_t*)buf, readbytes);
            }
            else {
                _ERROR("NO context! tcp server onRecv: %p: %d", io, hio_id(io));
            }
        });

        if (m_listener) {
            m_listener->onAccept(*conn);
        }

        hio_read_start(io);
    }

    void onRecv(hio_t* io, const uint8_t* data, const int len) {
        LOCK_MUTEX(m_lock);
        auto it = m_connections.find(io);
        if (it == m_connections.end()) {
            _ERROR("onRecv[%p:%d] not found", io);
            return;
        }
        TCPServerConnection* conn = it->second;
        _INFO("onRecv[%p:%d] [%s:%d] <= [%s:%d] len=%d", io, conn->id(),
              conn->localAddr(), conn->localPort(), conn->peerAddr(), conn->peerPort(), len);
        if (m_listener) {
            m_listener->onRecv(*conn, data, len);
        }
    }

    void onClose(hio_t* io) {
        LOCK_MUTEX(m_lock);
        auto it = m_connections.find(io);
        if (it == m_connections.end()) {
            _WARN("onClose[%p] not found, all connections(%d)", io, m_connections.size());
            return;
        }
        TCPServerConnection* conn = it->second;
        _INFO("onClose[%p:%d] [%s:%d] <= [%s:%d]", io, conn->id(),
              conn->localAddr(), conn->localPort(), conn->peerAddr(), conn->peerPort());
        if (m_listener) {
            m_listener->onClose(*conn);
        }
        delete conn;
        m_connections.erase(it);
    }

    void stop() {
        _INFO("TCPServerContext stop");
        LOCK_MUTEX(m_lock);
        stopInternal();
        _INFO("TCPServerContext stop done");
    }

private:
    void stopInternal() {
        // 关闭所有连接
        for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
            auto conn = it->second;
            conn->disconnect();
            _INFO("TCPServerContext disconnect conn[%p:%d] [%s:%d] <= [%s:%d]", it->first,
                  conn->id(), conn->localAddr(), conn->localPort(), conn->peerAddr(), conn->peerPort());
        }
        m_connections.clear();

        if (m_listen_io) {
            hio_close(m_listen_io);
            m_listen_io = nullptr;
            _INFO("TCPServerContext close listen_io");
        }
        if (m_event_loop_thread) {
            m_event_loop_thread->stop();
        }
    }

public:
    TCPServer* m_server;
    std::string m_host;
    int m_port = 0;

    hio_t* m_listen_io = nullptr;

    std::unordered_map<hio_t*, TCPServerConnection*> m_connections;

    TCPServerListener* m_listener = nullptr;
    std::unique_ptr<hv::EventLoopThread> m_event_loop_thread;

    std::mutex m_lock;
};

TCPServer::TCPServer() {
    m_context = new TCPServerContext(this);
}

TCPServer::~TCPServer() {
    if (m_context) {
        delete m_context;
        m_context = nullptr;
    }
}

void TCPServer::setListener(TCPServerListener* listener) {
    m_context->setListener(listener);
}

bool TCPServer::isRunning() const {
    return m_context->isRunning();
}

bool TCPServer::start(const std::string& host, int port) {
    return m_context->start(host, port);
}

void TCPServer::stop() {
    m_context->stop();
}


NAMESPACE_END
