//
// Created by LiangKeJin on 2025/3/30.
//

#include "TCPServer.h"

#include <common/Log.h>
#include <common/utils/Base.h>

NAMESPACE_DEFAULT

static void on_close(hio_t* io) {
    int error = hio_error(io);
    _INFO("on_close fd=%d error=%d", hio_fd(io), error);
    TCPServer* server = (TCPServer*) hio_context(io);
    if (server) {
        server->onClose(error);
    }
}

static void on_recv(hio_t* io, void* buf, int readbytes) {
    _INFO("on_recv fd=%d readbytes=%d", hio_fd(io), readbytes);
    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
    _INFO("on_recv [%s] <=> [%s]",
            SOCKADDR_STR(hio_localaddr(io), localaddrstr),
            SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));

    TCPServer* server = (TCPServer*) hio_context(io);
    if (server) {
        server->onRecv((const uint8_t*)buf, readbytes);
    }
}

static void on_accept(hio_t* io) {
    _INFO("on_accept connfd=%d", hio_fd(io));
    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
    _INFO("accept connfd=%d [%s] <= [%s]", hio_fd(io),
            SOCKADDR_STR(hio_localaddr(io), localaddrstr),
            SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));

    TCPServer* server = (TCPServer*) hio_context(io);
    if (server) {
        server->onAccept();
    }

    hio_setcb_close(io, on_close);
    hio_setcb_read(io, on_recv);

    hio_read_start(io);
}


bool TCPServer::start(const std::string& host, int port) {
    if (m_loop) {
        _ERROR("TCPProxy already started");
        return true;
    }
    m_loop = hloop_new(HLOOP_FLAG_AUTO_FREE);
    _ERROR_RETURN_IF(m_loop == nullptr, false, "hloop_new failed");
    m_listen_io = hloop_create_tcp_server(m_loop, host.c_str(), port, on_accept);
    _ERROR_RETURN_IF(m_listen_io == nullptr, false, "hloop_create_tcp_server failed");
    hio_set_context(m_listen_io, this);

    m_event_thread.post([this]() {
        hloop_run(m_loop);
    });
    return true;
}

int TCPServer::send(const uint8_t* data, int len) {
    _ERROR_RETURN_IF(m_listen_io == nullptr, 0, "m_listen_io is nullptr");
    int nwrite = hio_write(m_listen_io, data, len);
    _ERROR_RETURN_IF(nwrite < len, nwrite, "hio_write failed, write %d bytes, expect %d bytes", nwrite, len);
    return nwrite;
}

void TCPServer::stop() {
    if (m_loop) {
        hloop_stop(m_loop);
        m_event_thread.quit();
        m_loop = nullptr;
        m_listen_io = nullptr;
    }
}

void TCPServer::onAccept() {
    if (m_listener) {
        m_listener->onConnect();
    }
}

void TCPServer::onRecv(const uint8_t* data, int len) {
    if (m_listener) {
        m_listener->onRecv(data, len);
    }
}

void TCPServer::onClose(int error) {
    if (m_listener) {
        m_listener->onDisconnect(error);
    }
}


NAMESPACE_END