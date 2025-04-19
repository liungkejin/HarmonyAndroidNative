//
// Created by LiangKeJin on 2025/4/20.
//

#pragma once

#include <common/Log.h>

#include "ZNamespace.h"
#include <hv/WebSocketClient.h>
#include <hv/EventLoopThread.h>
#include <hv/HttpMessage.h>
#include <hv/WebSocketServer.h>

NAMESPACE_DEFAULT

// WebSocketClient重连设置
// #define DEFAULT_RECONNECT_MIN_DELAY     1000    // ms
// #define DEFAULT_RECONNECT_MAX_DELAY     60000   // ms
// #define DEFAULT_RECONNECT_DELAY_POLICY  2       // exponential
// #define DEFAULT_RECONNECT_MAX_RETRY_CNT INFINITE
//
// typedef struct reconn_setting_s {
//     uint32_t min_delay; // ms
//     uint32_t max_delay; // ms
//     uint32_t cur_delay; // ms
//     /*
//      * @delay_policy
//      * 0: fixed
//      * min_delay=3s => 3,3,3...
//      * 1: linear
//      * min_delay=3s max_delay=10s => 3,6,9,10,10...
//      * other: exponential
//      * min_delay=3s max_delay=60s delay_policy=2 => 3,6,12,24,48,60,60...
//      */
//     uint32_t delay_policy;
//     uint32_t max_retry_cnt;
//     uint32_t cur_retry_cnt;
//
// #ifdef __cplusplus
//     reconn_setting_s() {
//         min_delay = DEFAULT_RECONNECT_MIN_DELAY;
//         max_delay = DEFAULT_RECONNECT_MAX_DELAY;
//         cur_delay = 0;
//         // 1,2,4,8,16,32,60,60...
//         delay_policy = DEFAULT_RECONNECT_DELAY_POLICY;
//         max_retry_cnt = DEFAULT_RECONNECT_MAX_RETRY_CNT;
//         cur_retry_cnt = 0;
//     }
// #endif
// } reconn_setting_t;
typedef reconn_setting_s WSReconnectSetting;
typedef ws_opcode WSOpCode;
class WSocketClient;

struct WSClientConfig {
    int ping_interval_ms = 10000;
    WSReconnectSetting reconn_setting = reconn_setting_s();
};

struct WSClientHandler {
    std::function<void(WSocketClient& client)> onopen = nullptr;
    std::function<void(WSocketClient& client, const std::string& data, WSOpCode opcode)> onmessage = nullptr;
    std::function<void(WSocketClient& client)> onclose = nullptr;
};

class WSocketClient {
public:
    WSocketClient() {
        m_loop_thread = std::make_unique<hv::EventLoopThread>();
        m_loop_thread->start();
        m_client = std::make_unique<hv::WebSocketClient>(m_loop_thread->loop());
    }

    void setHandler(const WSClientHandler& handler) {
        m_handler = handler;
    }

    bool connect(const std::string& url, WSClientConfig config = WSClientConfig()) {
        if (m_client) {
            this->disconnect();
        }
        _INFO("WebSocketClient connecting to %s", url);
        m_loop_thread = std::make_unique<hv::EventLoopThread>();
        m_loop_thread->start();
        m_client = std::make_unique<hv::WebSocketClient>(m_loop_thread->loop());
        m_client->onopen = [this]() {
            m_connect_status = 2;
            if (m_handler.onopen) {
                m_handler.onopen(*this);
            }
        };
        m_client->onmessage = [this](const std::string& data) {
            if (m_handler.onmessage) {
                m_handler.onmessage(*this, data, m_client->opcode());
            }
        };
        m_client->onclose = [this]() {
            m_connect_status = 0;
            if (m_handler.onclose) {
                m_handler.onclose(*this);
            }
        };

        m_client->setPingInterval(config.ping_interval_ms);
        m_client->setReconnect(&config.reconn_setting);

        // http_headers headers;
        // headers["Origin"] = "http://www.xxxx.com";
        m_connect_status = 1;
        int ret = m_client->open(url.c_str());
        if (ret != 0) {
            m_connect_status = 0;
            _ERROR("WebSocketClient open(%s) failed, ret=%d", url, ret);
            return false;
        }
        _INFO("WebSocketClient open(%s) success, ret=%d", url, ret);
        return true;
    }

    int send(const std::string& data) {
        return send(data.c_str(), data.size());
    }

    int send(const char* buf, int len, WSOpCode opcode = WS_OPCODE_TEXT) {
        if (m_client) {
            int sendLen = m_client->send(buf, len, opcode);
            if (sendLen < len) {
                _ERROR("WebSocketClient send failed, sendLen=%d, expectLen=%d", sendLen, len);
            }
            return sendLen;
        }
        _ERROR("WebSocketClient send failed, client is null");
        return -1;
    }

    bool isConnecting() {
        return m_connect_status == 1;
    }

    bool isConnected() {
        return m_connect_status == 2;
    }

    void disconnect() {
        if (m_client) {
            m_client->stop();
        }
        if (m_loop_thread) {
            m_loop_thread->stop();
            m_loop_thread = nullptr;
        }
        m_connect_status = 0;
    }

private:
    std::unique_ptr<hv::EventLoopThread> m_loop_thread = nullptr;
    std::unique_ptr<hv::WebSocketClient> m_client = nullptr;

    WSClientHandler m_handler = WSClientHandler();
    // 0: disconnected
    // 1: connecting
    // 2: connected
    int m_connect_status = 0;
};

// WebSocketServer
typedef hv::WebSocketService WSService;
class WSocketServer : public hv::HttpServer {
public:
    WSocketServer() {}

    ~WSocketServer() { stop(); }

public:
    void start(int port, const WSService& service) {
        this->m_ws_service = service;
        this->ws = &this->m_ws_service;
        this->port = port;
        HttpServer::start();
    }

private:
    WebSocketService m_ws_service;
};
NAMESPACE_END
