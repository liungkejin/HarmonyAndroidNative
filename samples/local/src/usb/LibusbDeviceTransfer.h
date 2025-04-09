//
// Created by LiangKeJin on 25-3-23.
//

#pragma once

#include <common/utils/Array.h>
#include <common/utils/EventThread.h>

#include "LibusbDevice.h"
#include "ZNamespace.h"

NAMESPACE_DEFAULT

class LibusbDeviceTransferListener {
public:
    virtual ~LibusbDeviceTransferListener() {}
    virtual void onDataRecv(const uint8_t* data, const int len) = 0;
    virtual void onDataSend(const int len) = 0;
};

class LibusbMgr;
class LibusbDeviceTransfer {
    friend class LibusbMgr;
public:
    LibusbDeviceTransfer(LibusbMgr& mgr, LibusbDevice& device) :
        m_mgr(mgr), m_device(device), m_recv_thread("recv"), m_send_thread("send") {}
    ~LibusbDeviceTransfer() {}

public:
    bool start();

    bool isRunning() const {
        return m_is_running && !m_is_released;
    }

    void stop();

    void release();

public:
	void setListener(LibusbDeviceTransferListener* listener) {
		m_listener = listener;
	}

    void send(const uint8_t* data, const int len);

    void transCtrl(const uint8_t* data, const int len);

private:

    bool sendData();

    void receiveData(EndpointTransferType type);

private:
    LibusbMgr& m_mgr;
    LibusbDevice& m_device;
    LibusbDeviceTransferListener* m_listener = nullptr;

    EndpointTransferType m_recv_transfer_type = LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK;
    EndpointTransferType m_send_transfer_type = LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK;

    bool m_is_running = false;
    EventThread m_recv_thread;
    EventThread m_send_thread;

    std::mutex m_queue_mutex;
    std::list<FlexArray<uint8_t>> m_send_pending_queue;
    std::list<FlexArray<uint8_t>> m_send_free_queue;

    bool m_is_released = false;
};

NAMESPACE_END
