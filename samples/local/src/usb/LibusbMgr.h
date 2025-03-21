//
// Created by LiangKeJin on 2025/3/16.
//

#pragma once

#include <common/utils/EventThread.h>

#include "LibusbDevice.h"
#include "LibusbUtils.h"

NAMESPACE_DEFAULT

class LibusbMgr {
public:
    bool initialize();

    void release();

public:
    int onDevicePlug(libusb_device* dev);

    int onDeviceUnplug(libusb_device* dev);

private:
    libusb_context* m_usb_context = nullptr;
    libusb_hotplug_callback_handle m_hotplug_cb_handle = 0;
    EventThread m_event_thread;
    bool m_is_listening = false;

    std::mutex m_proc_lock;
    std::list<LibusbDevice> m_devices;
};

NAMESPACE_END