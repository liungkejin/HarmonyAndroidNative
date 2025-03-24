//
// Created by LiangKeJin on 2025/3/16.
//

#pragma once

#include <common/utils/EventThread.h>

#include "AOAProtocol.h"
#include "LibusbDevice.h"
#include "LibusbUtils.h"

NAMESPACE_DEFAULT

class LibusbDeviceListener {
public:
    virtual ~LibusbDeviceListener() = default;

    virtual void onDevicePlug(const LibusbDeviceInfo& dev) = 0;

    virtual void onDeviceUnplug(const LibusbDeviceInfo& dev) = 0;

    virtual void onDeviceListUpdate(std::list<LibusbDeviceInfo> devList) = 0;
};

class LibusbMgr {
public:
    bool initialize();

    void release();

public:
    void setListener(LibusbDeviceListener* listener) {
        m_listener = listener;
    }

    int onDevicePlug(libusb_device* dev);

    int onDeviceUnplug(libusb_device* dev);

public:
    /**
     * 获取当前设备信息列表
     * @return 当前设备列表
     */
    std::list<LibusbDeviceInfo> getDeviceList();

    /**
     * 打开设备
     * @param devInfo 设备信息
     * @return 是否打开成功
     */
    bool openDevice(const LibusbDeviceInfo& devInfo);

    /**
     * 设置设备到 accessory 模式
     * @param devInfo 设备信息
     * @return 是否设置成功
     */
    bool setupDeviceToAccessory(const LibusbDeviceInfo& devInfo, const AOAInfo& aoaInfo);

    /**
     * 关闭设备
     * @param dev_info 设备信息
     * @return 是否关闭成功
     */
    bool closeDevice(const LibusbDeviceInfo& dev_info);

private:

    /**
     * @return 实时列出所有设备列表
     */
    std::list<LibusbDevice> listDevices();

    LibusbDevice* findDevice(const LibusbDeviceInfo& dev_info);

    void onDeviceListUpdate();

private:
    libusb_context* m_usb_context = nullptr;
    libusb_hotplug_callback_handle m_hotplug_cb_handle = 0;
    EventThread m_event_thread;
    EventThread m_work_thread;
    bool m_is_listening = false;

    std::mutex m_proc_lock;
    std::list<LibusbDevice> m_devices;
    LibusbDeviceListener* m_listener = nullptr;
};

NAMESPACE_END
