//
// Created by LiangKeJin on 2025/4/13.
//

#include "TCPClient.h"

#include <common/utils/Base.h>

NAMESPACE_DEFAULT
class TCPClientContext {
public:
    TCPClientContext(TCPClient* c) : client(c) {
    }

    ~TCPClientContext() {
        m_event_thread.quit();
        this->release();
    }

    TCPClient* client;

    std::string host;
    int port = 0;

    bool connected = false;

    TCPClientListener* listener = nullptr;
    hloop_t* loop = nullptr;
    hio_t* listen_io = nullptr;

    std::mutex lock;

    bool connect(const std::string& host, int port, int timeoutMs = 0) {
        LOCK_MUTEX(lock);
        this->releaseInternal();
        this->loop = hloop_new(HLOOP_FLAG_AUTO_FREE);
        if (this->loop == nullptr) {
            return false;
        }
        this->listen_io = hio_create_socket(loop, host.c_str(), port, HIO_TYPE_TCP, HIO_CLIENT_SIDE);
        if (listen_io == nullptr) {
            _ERROR("hio_create_socket failed(%s:%d)", host.c_str(), port);
            this->releaseInternal();
            return false;
        }

        this->host = host;
        this->port = port;
        _INFO("tcpclient connect(%s:%d) timeoutms = %d", host.c_str(), port, timeoutMs);
        if (timeoutMs > 0) {
            hio_set_connect_timeout(listen_io, timeoutMs);
        }

        hevent_set_userdata(listen_io, this);
        hio_setcb_connect(listen_io, [](hio_t* io) {
            TCPClientContext* ctx = (TCPClientContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onConnect();
            }
        });

        hio_setcb_close(listen_io, [](hio_t* io) {
            TCPClientContext* ctx = (TCPClientContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onClose();
            }
        });

        hio_setcb_read(listen_io, [](hio_t* io, void* buf, int readbytes) {
            TCPClientContext* ctx = (TCPClientContext*)hevent_userdata(io);
            if (ctx) {
                ctx->onRecv((const uint8_t*)buf, readbytes);
            }
        });

        int ret = hio_connect(listen_io);
        if (ret != 0) {
            _ERROR("hio_connect(%s:%d) failed: %d", host.c_str(), port, ret);
            this->releaseInternal();
            return false;
        }

        hio_read_start(listen_io);
        m_event_thread.post([this]() {
            if (loop) {
                _INFO("TCPClient run loop");
                hloop_run(loop);
                _INFO("TCPClient loop stopped");
            }
            else {
                _ERROR("TCPClient loop is null");
            }
        });
        _INFO("hio_connect(%s:%d) success", host.c_str(), port);
        return true;
    }

    void onConnect() {
        LOCK_MUTEX(lock);
        this->connected = true;
        if (listener) {
            listener->onConnect(*client);
        }
    }

    int send(const uint8_t* data, const int len) {
        LOCK_MUTEX(lock);
        if (!connected) {
            return -1;
        }
        int ret = hio_write(listen_io, data, len);
        if (ret != len) {
            _ERROR("hio_write failed: %d", ret);
        }
        else {
            _INFO("hio_write success: %d", ret);
        }
        return ret;
    }

    void onRecv(const uint8_t* data, const int len) {
        LOCK_MUTEX(lock);
        if (listener) {
            listener->onRecv(*client, data, len);
        }
    }

    void onClose() {
        LOCK_MUTEX(lock);
        this->connected = false;
        if (listener) {
            listener->onDisconnect(*client);
        }
    }

    void release() {
        _INFO("TCPClient disconnect");
        LOCK_MUTEX(lock);
        this->releaseInternal();
        _INFO("TCPClient disconnect done");
    }

private:
    void releaseInternal() {
        this->connected = false;
        if (listen_io) {
            hio_close(listen_io);
            listen_io = nullptr;
        }
        if (loop) {
            hloop_stop(loop);
            hloop_free(&loop);
            loop = nullptr;
        }
    }

private:
    EventThread m_event_thread;
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
