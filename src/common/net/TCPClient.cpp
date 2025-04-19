//
// Created by LiangKeJin on 2025/4/13.
//

#include "TCPClient.h"

#include <common/utils/Base.h>
#include <hv/EventLoopThread.h>

NAMESPACE_DEFAULT
class TCPClientContext {
public:
    std::string host;
    int port = 0;

    bool connected = false;

    TCPClientListener* listener = nullptr;

public:
    explicit TCPClientContext(TCPClient* c) : m_client(c) {
    }

    ~TCPClientContext() {
        this->release();
    }

    bool connect(const std::string& host, int port, int timeoutMs = 0) {
        LOCK_MUTEX(m_lock);
        this->releaseInternal();

        m_event_loop_thread = std::make_unique<hv::EventLoopThread>();
        m_event_loop_thread->start();
        hloop_t* loop = m_event_loop_thread->hloop();
        _ERROR_RETURN_IF(loop == nullptr, false, "tcp client loop thread start failed");

        this->m_listen_io = hio_create_socket(loop, host.c_str(), port, HIO_TYPE_TCP, HIO_CLIENT_SIDE);
        if (m_listen_io == nullptr) {
            _ERROR("hio_create_socket failed(%s:%d)", host.c_str(), port);
            this->releaseInternal();
            return false;
        }

        this->host = host;
        this->port = port;
        _INFO("tcpclient connect(%s:%d) timeoutms = %d", host.c_str(), port, timeoutMs);
        if (timeoutMs > 0) {
            hio_set_connect_timeout(m_listen_io, timeoutMs);
        }

        hevent_set_userdata(m_listen_io, this);
        hio_setcb_connect(m_listen_io, [](hio_t* io) {
            TCPClientContext* ctx = (TCPClientContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onConnect();
            }
        });

        hio_setcb_close(m_listen_io, [](hio_t* io) {
            TCPClientContext* ctx = (TCPClientContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onClose();
            }
        });

        hio_setcb_read(m_listen_io, [](hio_t* io, void* buf, int readbytes) {
            TCPClientContext* ctx = (TCPClientContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onRecv((const uint8_t*)buf, readbytes);
            }
        });

        int ret = hio_connect(m_listen_io);
        if (ret != 0) {
            _ERROR("hio_connect(%s:%d) failed: %d", host.c_str(), port, ret);
            this->releaseInternal();
            return false;
        }

        hio_read_start(m_listen_io);
        _INFO("TCPClient run loop");
        hloop_run(loop);
        _INFO("TCPClient(%s:%d) start success", host.c_str(), port);
        return true;
    }

    void onConnect() {
        LOCK_MUTEX(m_lock);
        this->connected = true;
        if (listener) {
            listener->onConnect(*m_client);
        }
    }

    int send(const uint8_t* data, const int len) {
        LOCK_MUTEX(m_lock);
        if (!connected) {
            return -1;
        }
        int ret = hio_write(m_listen_io, data, len);
        if (ret != len) {
            _ERROR("hio_write failed: %d", ret);
        }
        else {
            _INFO("hio_write success: %d", ret);
        }
        return ret;
    }

    void onRecv(const uint8_t* data, const int len) {
        LOCK_MUTEX(m_lock);
        if (listener) {
            listener->onRecv(*m_client, data, len);
        }
    }

    void onClose() {
        LOCK_MUTEX(m_lock);
        this->connected = false;
        if (listener) {
            listener->onDisconnect(*m_client);
        }
    }

    void release() {
        _INFO("TCPClient disconnect");
        LOCK_MUTEX(m_lock);
        this->releaseInternal();
        _INFO("TCPClient disconnect done");
    }

private:
    void releaseInternal() {
        this->connected = false;
        if (m_listen_io) {
            hio_close(m_listen_io);
            m_listen_io = nullptr;
        }
        if (m_event_loop_thread) {
            m_event_loop_thread->stop();
        }
    }

private:
    std::mutex m_lock;

    TCPClient* m_client;
    hio_t* m_listen_io = nullptr;
    std::unique_ptr<hv::EventLoopThread> m_event_loop_thread = nullptr;
};

TCPClient::TCPClient() {
    m_context = new TCPClientContext(this);
}

TCPClient::~TCPClient() {
    if (m_context) {
        m_context->release();
        delete m_context;
        m_context = nullptr;
    }
}

void TCPClient::setListener(TCPClientListener* listener) {
    m_context->listener = listener;
}

bool TCPClient::connect(const std::string& host, int port, int timeoutMs) {
    return m_context->connect(host, port, timeoutMs);
}

std::string TCPClient::getHost() const {
    return m_context->host;
}

int TCPClient::getPort() const {
    return m_context->port;
}

bool TCPClient::isConnected() const {
    return m_context->connected;
}

int TCPClient::send(const uint8_t* data, const int len) {
    return m_context->send(data, len);
}

void TCPClient::disconnect() {
    m_context->release();
}


NAMESPACE_END
