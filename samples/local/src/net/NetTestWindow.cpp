//
// Created by LiangKeJin on 2025/4/13.
//

#include "NetTestWindow.h"

#include <common/net/TCPClient.h>
#include <common/net/TCPServer.h>
#include <common/net/WebSocket.h>

using namespace znative;

class MyTCPServerListener : public TCPServerListener {
public:
    void onAccept(TCPServerConnection& connection) override {
        _INFO("tcp server onAccept");
    }

    void onRecv(TCPServerConnection& connection, const uint8_t* data, const int len) override {
        _INFO("tcp server onRecv len=%d", len);
        int send = connection.send(data, len);
        _INFO("tcp server send: %d, expect: %d", send, len);
    }

    void onClose(TCPServerConnection& connection) override {
        _INFO("tcp server onClose");
    }
};

class MyTCPClientListener : public TCPClientListener {
public:
    void onConnect(TCPClient& client) override {
        _INFO("tcp client onConnect");
    }

    void onRecv(TCPClient& client, const uint8_t* data, const int len) override {
        _INFO("tcp client onRecv len=%d", len);
    }

    void onDisconnect(TCPClient& client) override {
        _INFO("tcp client onDisconnect");
    }
};

int server_port = 19999;
TCPServer m_tcp_server;
MyTCPServerListener m_tcp_server_listener;

TCPClient m_tcp_client;
MyTCPClientListener m_tcp_client_listener;

char m_client_input_buffer[1024] = {0};


WSocketClient m_ws_client;
std::shared_ptr<WSocketServer> m_ws_server;

void NetTestWindow::onVisible(int width, int height) {
    m_tcp_server.setListener(&m_tcp_server_listener);
    m_tcp_client.setListener(&m_tcp_client_listener);

    m_ws_server = std::make_shared<WSocketServer>();
    WSService ws;
    ws.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
        _INFO("websocket server onopen");
        channel->send("hello world");
    };
    ws.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg) {
        WSOpCode code = channel->opcode;
        _INFO("websocket server onmessage: %s, opcode: %d", msg.c_str(), code);
        channel->send("response_msg");
    };
    ws.onclose = [](const WebSocketChannelPtr& channel) {
        _INFO("websocket server onclose");
    };
    ws.ping_interval = 10000;
    m_ws_server->start(19998, ws);

    WSClientHandler handler;
    handler.onopen = [](WSocketClient& client) {
        _INFO("websocket client onopen");
    };
    handler.onmessage = [](WSocketClient& client, const std::string& msg, WSOpCode code) {
        _INFO("websocket client onmessage: %s, opcode: %d", msg.c_str(), code);
    };
    handler.onclose = [](WSocketClient& client) {
        _INFO("websocket client onclose");
    };
    m_ws_client.setHandler(handler);
}

void NetTestWindow::onInvisible(int width, int height) {
    m_tcp_server.stop();
    m_tcp_client.disconnect();

    m_ws_server->stop();
    m_ws_client.disconnect();
}


void NetTestWindow::onPreRender(int width, int height) {
}

void NetTestWindow::onRenderImgui(int width, int height, ImGuiIO& io) {
    ImGui::Begin("网络测试", 0, 0);

    ImGui::InputInt("端口", &server_port);
    if (m_tcp_server.isRunning()) {
        ImGui::Text("TCPServer 运行中");
        if (ImGui::Button("关闭TCPServer")) {
            m_tcp_server.stop();
        }
    }
    else {
        if (ImGui::Button("开启TCPServer")) {
            if (!m_tcp_server.startLocal(server_port)) {
                _ERROR("startLocal port 9999 failed");
            }
        }
    }

    if (m_tcp_client.isConnected()) {
        ImGui::Text("TCPClient 已连接");
        if (ImGui::Button("断开TCPClient")) {
            m_tcp_client.disconnect();
        }
        ImGui::InputText("client输入", m_client_input_buffer, sizeof(m_client_input_buffer));
        if (ImGui::Button("发送数据")) {
            auto len = strlen(m_client_input_buffer);
            m_tcp_client.send((const uint8_t*)m_client_input_buffer, len);
        }
    }
    else {
        ImGui::Text("TCPClient 未连接");
        if (ImGui::Button("连接TCPClient")) {
            m_tcp_client.connect("127.0.0.1", server_port);
        }
    }

    if (m_ws_client.isConnected()) {
        ImGui::Text("WebSocketClient 已连接");
        if (ImGui::Button("发送数据")) {
            m_ws_client.send("hello world from client");
        }
        if (ImGui::Button("断开 WebSocketClient")) {
            m_ws_client.disconnect();
        }
    } else if (m_ws_client.isConnecting()) {
        ImGui::Text("WebSocketClient 正在连接");
    } else {
        if (ImGui::Button("连接 WebSocketClient(ws://127.0.0.1:19998)")) {
            m_ws_client.connect("ws://127.0.0.1:19998");
        }
    }

    ImGui::End();
}
