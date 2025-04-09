//
// Created by LiangKeJin on 2025/3/30.
//

#pragma once

#include <common/utils/EventThread.h>

#include "ZNamespace.h"
#include "hv/hv.h"
#include "hv/hloop.h"

NAMESPACE_DEFAULT

class TCPServerListener {
public:
    virtual ~TCPServerListener() {}
    virtual void onConnect() = 0;
    virtual void onRecv(const uint8_t* data, const int len) = 0;
    virtual void onDisconnect(int error = 0) = 0;
};

class TCPServer {
public:
    TCPServer() = default;

public:
    void setListener(TCPServerListener* listener) {
        m_listener = listener;
    }

    bool startLocal(int port) {
        std::string host = "0.0.0.0";
        return start(host, port);
    }

    bool start(const std::string& host, int port);

    int send(const uint8_t *data, int len);

    void stop();

public:
    void onAccept();

    void onRecv(const uint8_t *data, int len);

    void onClose(int error);

private:
    hloop_t* m_loop = nullptr;
    hio_t *m_listen_io = nullptr;
    EventThread m_event_thread;

    TCPServerListener* m_listener = nullptr;
};

NAMESPACE_END
