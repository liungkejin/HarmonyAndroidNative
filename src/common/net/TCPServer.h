//
// Created by LiangKeJin on 2025/3/30.
//

#pragma once

#include <common/utils/Base.h>
#include <common/utils/EventThread.h>

#include "ZNamespace.h"
#include "hv/hv.h"
#include "hv/hloop.h"

NAMESPACE_DEFAULT
class TCPServer;
class TCPServerContext;

class TCPServerConnection {
public:
    TCPServerConnection(TCPServer& server, hio_t* io) : m_server(server), m_io(io) {
        m_connection_id = hio_id(io);

        char localaddrstr[SOCKADDR_STRLEN] = {0};
        // 127.0.0.1:19999
        std::string local_addr = SOCKADDR_STR(hio_localaddr(io), localaddrstr);
        int pos = local_addr.find(':');
        if (pos != std::string::npos) {
            m_local_addr = local_addr.substr(0, pos);
            m_local_port = std::stoi(local_addr.substr(pos + 1));
        }

        char peeraddrstr[SOCKADDR_STRLEN] = {0};
        std::string peer_addr = SOCKADDR_STR(hio_peeraddr(io), peeraddrstr);
        pos = peer_addr.find(':');
        if (pos != std::string::npos) {
            m_remote_addr = peer_addr.substr(0, pos);
            m_remote_port = std::stoi(peer_addr.substr(pos + 1));
        }
    }

    ~TCPServerConnection() {
        this->disconnect();
    }

    TCPServer& server() {
        return m_server;
    }

    int id() const {
        return m_connection_id;
    }

    std::string localAddr() const {
        return m_local_addr;
    }

    int localPort() const {
        return m_local_port;
    }

    std::string peerAddr() const {
        return m_remote_addr;
    }

    int peerPort() const {
        return m_remote_port;
    }

    int error() const {
        return hio_error(m_io);
    }

    int send(const uint8_t* data, int len) {
        LOCK_MUTEX(m_lock);
        if (!m_io) {
            return -1;
        }
        return hio_write(m_io, data, len);
    }

    void disconnect() {
        LOCK_MUTEX(m_lock);
        if (m_io) {
            hio_close(m_io);
            m_io = nullptr;
        }
    }

private:
    TCPServer& m_server;
    hio_t* m_io;
    int m_connection_id = -1;
    std::string m_local_addr;
    int m_local_port;
    std::string m_remote_addr;
    int m_remote_port;

    std::mutex m_lock;
};

class TCPServerListener {
public:
    virtual ~TCPServerListener() {
    }

    /**
     * @brief 有新的连接
     * @param connection 连接对象
     */
    virtual void onAccept(TCPServerConnection& connection) = 0;
    /**
     * @brief 有数据到达
     * @param connection 连接对象
     * @param data 数据指针
     * @param len 数据长度
     */
    virtual void onRecv(TCPServerConnection& connection, const uint8_t* data, const int len) = 0;
    /**
     * @brief 连接关闭
     * @param connection 连接对象, 注意: 执行完之后，该对象会被释放, 不能再使用
     */
    virtual void onClose(TCPServerConnection& connection) = 0;
};

class TCPServer {
public:
    TCPServer();

    ~TCPServer();

public:
    void setListener(TCPServerListener* listener);

    bool startLocal(int port) {
        std::string host = "0.0.0.0";
        return start(host, port);
    }

    bool start(const std::string& host, int port);

    bool isRunning() const;

    void stop();

private:
    TCPServerContext* m_context = nullptr;
};

NAMESPACE_END
