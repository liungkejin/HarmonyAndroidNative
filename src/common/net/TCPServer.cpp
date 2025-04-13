//
// Created by LiangKeJin on 2025/3/30.
//

#include "TCPServer.h"

#include <common/Log.h>
#include <common/utils/Base.h>

NAMESPACE_DEFAULT
class TCPServerContext {
public:
    explicit TCPServerContext(TCPServer* server) : m_server(server) {
    }

    ~TCPServerContext() {
        this->stop();
        m_event_thread.quit();
    }

    void setListener(TCPServerListener* listener) {
        m_listener = listener;
    }

    bool isRunning() {
        return m_listen_io != nullptr;
    }

    bool start(const std::string& host, int port) {
        LOCK_MUTEX(m_lock);
        this->stopInternal();

        hloop_t* loop = hloop_new(HLOOP_FLAG_AUTO_FREE);
        _ERROR_RETURN_IF(loop == nullptr, false, "tcp server hloop_new failed");

        hio_t* io = hio_create_socket(loop, host.c_str(), port, HIO_TYPE_TCP, HIO_SERVER_SIDE);
        if (io == nullptr) {
            hloop_free(&loop);
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
            hloop_free(&loop);
            hio_close(io);
            _ERROR("hio_accept failed: %d", ret);
            return false;
        }

        m_loop = loop;
        m_listen_io = io;

        _INFO("tcp server start(%s:%d), loop: %p, listen io: %p: %d", host.c_str(), port, m_loop, m_listen_io,
              hio_id(m_listen_io));
        m_event_thread.post([this]() {
            if (m_loop) {
                _INFO("tcp server loop");
                hloop_run(m_loop);
                _INFO("tcp server loop end");
            } else {
                _ERROR("tcp server == null");
            }
        });
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
            _ERROR("onClose[%p:%d] not found", io);
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

        if (m_loop) {
            hloop_stop(m_loop);
            hloop_free(&m_loop);
            m_loop = nullptr;
            _INFO("TCPServerContext free loop");
        }
    }

public:
    TCPServer* m_server;
    std::string m_host;
    int m_port = 0;

    hloop_t* m_loop = nullptr;
    hio_t* m_listen_io = nullptr;

    std::unordered_map<hio_t*, TCPServerConnection*> m_connections;

    TCPServerListener* m_listener = nullptr;
    EventThread m_event_thread;

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
