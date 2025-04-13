//
// Created by LiangKeJin on 2025/4/13.
//

#pragma once

#include <common/utils/EventThread.h>

#include "ZNamespace.h"
#include "hv/hv.h"
#include "hv/hloop.h"

NAMESPACE_DEFAULT

class TCPClient;
class TCPClientContext;

class TCPClientListener {
public:
    virtual ~TCPClientListener() {
    }

    virtual void onConnect(TCPClient& client) = 0;
    virtual void onRecv(TCPClient& client, const uint8_t* data, const int len) = 0;
    virtual void onDisconnect(TCPClient& client) = 0;
};

class TCPClient {
public:
    TCPClient();

    ~TCPClient();

public:
    void setListener(TCPClientListener* listener);
    /**
     * @brief 连接服务器
     * @param host 服务器地址
     * @param port 服务器端口
     * @param timeoutMs 连接超时时间，单位毫秒，默认为3000毫秒
     * @return 是否执行成功
     */
    bool connect(const std::string& host, int port, int timeoutMs = 3000);

    std::string getHost() const;

    int getPort() const;

    bool isConnected() const;

    /**
     * @brief 发送数据
     * @param data 数据指针
     * @param len 数据长度
     * @return 发送的字节数，如果返回-1表示发送失败
     */
    int send(const uint8_t* data, int len);

    void disconnect();

private:
    TCPClientContext* m_context;
};

NAMESPACE_END
