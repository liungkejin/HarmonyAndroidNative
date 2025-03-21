//
// Created by LiangKeJin on 2025/3/16.
//

#include "LibusbMgr.h"

#include <common/Log.h>
#include <common/utils/Base.h>

NAMESPACE_DEFAULT
static int LIBUSB_CALL hotplug_callback(libusb_context* ctx, libusb_device* dev, libusb_hotplug_event event, void* user_data) {
    LibusbMgr* mgr = (LibusbMgr*)user_data;
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        _INFO("device plugged");
        return mgr->onDevicePlug(dev);
    } else {
        _INFO("device unplugged");
        return mgr->onDeviceUnplug(dev);
    }
}

bool LibusbMgr::initialize() {
    _INFO("LibusbMgr initialize");
    int err = libusb_init(&m_usb_context);
    _ERROR_RETURN_IF(err != LIBUSB_SUCCESS, false, "libusb_init failed: %s", LibusbUtils::errString(err));

    // 判断当前库是否支持热插拔
    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        this->release();
        _ERROR("libusb has no hotplug capability");
        return false;
    }

    // 注册热插拔回调函数
    int vendorId = LIBUSB_HOTPLUG_MATCH_ANY;
    int productId = LIBUSB_HOTPLUG_MATCH_ANY;
    int classId = LIBUSB_HOTPLUG_MATCH_ANY;
    err = libusb_hotplug_register_callback(m_usb_context,
        LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_ENUMERATE,
        vendorId, productId, classId, hotplug_callback, this, &m_hotplug_cb_handle);
    if (err != LIBUSB_SUCCESS) {
        this->release();
        _ERROR("libusb_hotplug_register_callback failed: %s", LibusbUtils::errString(err));
        return false;
    }

    m_is_listening = true;
    m_event_thread.post([this](){
        while (m_is_listening) {
            int rc = libusb_handle_events(m_usb_context);
            _INFO("libusb_handle_events rc: %d", rc);
        }
    });
    return true;
}

int LibusbMgr::onDevicePlug(libusb_device* dev) {
    LOCK_MUTEX(m_proc_lock);

    libusb_device_descriptor desc;
    int ret = libusb_get_device_descriptor(dev, &desc);
    _ERROR_RETURN_IF(ret != LIBUSB_SUCCESS, ret, "libusb_get_device_descriptor failed: %s", LibusbUtils::errString(ret));

    LibusbDevice device(dev, desc);
    _INFO("device plugged: %s", device.toString());
    m_devices.push_back(device);

    return LIBUSB_SUCCESS;
}

int LibusbMgr::onDeviceUnplug(libusb_device* dev) {
    LOCK_MUTEX(m_proc_lock);
    bool removed = false;
    // 从 m_devices 中删除 dev
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if (it->device() == dev) {
            removed = true;
            m_devices.erase(it);
            _INFO("device unplugged: %s", it->toString());
            break;
        }
    }
    _ERROR_RETURN_IF(!removed, LIBUSB_ERROR_NOT_FOUND, "device not found: %p", dev);
    return LIBUSB_SUCCESS;
}


void LibusbMgr::release() {
    m_is_listening = false;

    if (m_hotplug_cb_handle) {
        libusb_hotplug_deregister_callback(m_usb_context, m_hotplug_cb_handle);
        m_hotplug_cb_handle = 0;
    }

    if (m_usb_context) {
        libusb_exit(m_usb_context);
        m_usb_context = nullptr;
        _INFO("LibusbMgr released");
    }
    m_event_thread.quit();
}


NAMESPACE_END