//
// Created by LiangKeJin on 25-3-23.
//

#include "LibusbDeviceTransfer.h"

#include <common/utils/Base.h>

#include "LibusbMgr.h"

NAMESPACE_DEFAULT

bool LibusbDeviceTransfer::start() {
    // 判断当前设备是否是可用的
    if (m_is_released) {
        _ERROR("device is released");
        return false;
    }
    if (m_is_running) {
        return true;
    }
    m_is_running = true;
    auto interface = m_device.activeInterface();
    m_recv_transfer_type = interface.getActiveInEndpoint().transferType();
    m_send_transfer_type = interface.getActiveOutEndpoint().transferType();
    if (m_recv_transfer_type == LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK ||
        m_recv_transfer_type == LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT) {
        m_recv_thread.post([this]() {
            this->receiveData(m_recv_transfer_type);
        });
    }
    return true;
}

void LibusbDeviceTransfer::stop() {
    if (!m_is_running) {
        _ERROR("device is stopped");
        return;
    }
    _INFO("device transfer stopping...");
    m_is_running = false;

    m_recv_thread.quit();
    m_send_thread.quit();
    // 清空队列
    LOCK_MUTEX(m_queue_mutex);
    m_send_pending_queue.clear();
    m_send_free_queue.clear();
    _INFO("device transfer stopped");
}

void LibusbDeviceTransfer::release() {
    if (m_is_released) {
        return;
    }
    m_is_released = true;
    stop();
    _INFO("device transfer released");
}

void LibusbDeviceTransfer::receiveData(EndpointTransferType transType) {
    constexpr int bufferSize = 1024;
    uint8_t buffer[bufferSize] = {0};
    int ret = 0;
    while (!m_is_released && m_is_running && ret == 0) {
        int recvLength = 0;
        if (transType == LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK) {
            ret = m_device.recvBulk(buffer, bufferSize, &recvLength, 0);
        }
        else if (transType == LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT) {
            ret = m_device.recvInterrupt(buffer, bufferSize, &recvLength, 0);
        }
        else {
            ret = -1;
            _ERROR("unsupported transfer type: %d", transType);
        }
        if (ret == 0 && recvLength > 0 && m_listener) {
            m_listener->onDataRecv(buffer, recvLength);
        }
        if (ret == LIBUSB_ERROR_TIMEOUT) {
            ret = 0;
        }
    }
    if (ret != 0) {
        _ERROR("Failed to receive data: %s", LibusbUtils::errString(ret));
    }
}

void LibusbDeviceTransfer::send(const uint8_t* data, const int len) {
    {
        LOCK_MUTEX(m_queue_mutex);
        // 发送数据
        if (m_is_released) {
            _ERROR("send failed: device is released");
            return;
        }
        if (!m_is_running) {
            _ERROR("send failed: device is not running");
            return;
        }
        if (m_send_transfer_type != LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK &&
            m_send_transfer_type != LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT) {
            _ERROR("send failed: device is not bulk transfer type or interrupt transfer type");
            return;
        }
        FlexArray<uint8_t> arr;
        if (!m_send_free_queue.empty()) {
            arr = m_send_free_queue.front();
            m_send_free_queue.pop_front();
        }
        auto ptr = arr.obtain(len);
        memcpy(ptr, data, len);
        m_send_pending_queue.push_back(arr);
    }
    // _INFO("ready to send data: %d bytes", len);
    m_send_thread.post([this]() {
        while (!m_is_released && m_is_running) {
            if (!this->sendData()) {
                break;
            }
        }
    });
}

bool LibusbDeviceTransfer::sendData() {
    FlexArray<uint8_t> pendingData;
    {
        LOCK_MUTEX(m_queue_mutex);
        if (m_send_pending_queue.empty()) {
            return false;
        }
        pendingData = m_send_pending_queue.front();
        m_send_pending_queue.pop_front();
    }
    int dataLength = pendingData.size();
    uint8_t* ptr = pendingData.data();
    if (ptr == nullptr || dataLength < 1) {
        _ERROR("send failed: data is empty");
        return false;
    }
    int ret = 0;
    int sumSendLength = 0;
    do {
        int sendLength = 0;
        uint8_t *startData = ptr + sumSendLength;
        int leftLength = dataLength - sumSendLength;
        if (m_send_transfer_type == LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK) {
            ret = m_device.sendBulk(startData, leftLength, &sendLength, 100);
        }
        else {
            ret = m_device.sendInterrupt(startData, leftLength, &sendLength, 100);
        }
        if (ret == 0) {
            sumSendLength += sendLength;
        }
    }
    while ((ret == 0 || ret == LIBUSB_ERROR_TIMEOUT) && sumSendLength < dataLength && m_is_running && !m_is_released);

    {
        LOCK_MUTEX(m_queue_mutex);
        m_send_free_queue.push_back(pendingData);
    }

    _ERROR_IF(ret != 0, "Failed to send data: %s", LibusbUtils::errString(ret));
    if (ret == 0 && m_listener && sumSendLength > 0) {
        m_listener->onDataSend(sumSendLength);
    }
    return ret == 0;
}


NAMESPACE_END
