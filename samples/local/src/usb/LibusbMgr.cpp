//
// Created by LiangKeJin on 2025/3/16.
//

#include "LibusbMgr.h"

#include <common/Log.h>
#include <common/utils/Base.h>

#include "AOAProtocol.h"

NAMESPACE_DEFAULT
static int LIBUSB_CALL hotplug_callback(libusb_context* ctx,
    libusb_device* dev, libusb_hotplug_event event, void* user_data) {
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

    LOCK_MUTEX(m_proc_lock);
    m_devices = listDevices();
    m_is_listening = true;
    m_event_thread.post([this](){
        _INFO("LibusbMgr event thread started");
        while (m_is_listening) {
            libusb_handle_events(m_usb_context);
            // _INFO("libusb_handle_events rc: %d", rc);
        }
        _INFO("LibusbMgr event thread stopped");
    });
    return true;
}

int LibusbMgr::onDevicePlug(libusb_device* dev) {
    LibusbDeviceInfo deviceInfo;
    {
        LOCK_MUTEX(m_proc_lock);

        libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(dev, &desc);
        _ERROR_RETURN_IF(ret != LIBUSB_SUCCESS, ret, "libusb_get_device_descriptor failed: %s", LibusbUtils::errString(ret));

        LibusbDevice device(dev, desc);
        // 检查设备是否已经在 m_devices 中
        for (auto& d : m_devices) {
            if (d.device() == dev) {
                _INFO("device already in m_devices");
                return LIBUSB_SUCCESS;
            }
        }
        m_devices.push_back(device);
        deviceInfo = device.info();
    }

    m_work_thread.post([deviceInfo, this]() {
        LOCK_MUTEX(m_proc_lock);
        if (auto* dd = findDevice(deviceInfo)) {
            dd->prepare();
            _INFO("device plugged: %s", dd->toString());
            if (m_listener) {
                m_listener->onDevicePlug(dd->info());
            }

            this->onDeviceListUpdate();
        }
    });
    return LIBUSB_SUCCESS;
}

int LibusbMgr::onDeviceUnplug(libusb_device* dev) {
    LOCK_MUTEX(m_proc_lock);
    LibusbDevice device;
    // 从 m_devices 中删除 dev
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if (it->device() == dev) {
            device = *it;
            m_devices.erase(it);
            break;
        }
    }
    if (device.valid()) {
        _INFO("device unplugged: %s", device.toString());
    } else {
        _INFO("device not found: %p", dev);
    }
    m_work_thread.post([device, this]() {
        LOCK_MUTEX(m_proc_lock);
        if (device.valid()) {
            if (m_listener) {
                m_listener->onDeviceUnplug(device.info());
            }
        }
        this->onDeviceListUpdate();
    });
    return LIBUSB_SUCCESS;
}

std::list<LibusbDeviceInfo> LibusbMgr::getDeviceList() {
    LOCK_MUTEX(m_proc_lock);
    std::list<LibusbDeviceInfo> devices;
    for (auto& device : m_devices) {
        devices.push_back(device.info());
    }
    return devices;
}

bool LibusbMgr::openDevice(const LibusbDeviceInfo& devInfo) {
    LOCK_MUTEX(m_proc_lock);
    bool result = false;
    if (LibusbDevice* device = findDevice(devInfo)) {
        if (AOAProtocol::isAccessory(devInfo)) {
            result = AOAProtocol::openAccessory(*device);
        } else {
            result = device->open() == LIBUSB_SUCCESS;
        }

        if (!result) {
            _ERROR("open device(%s) failed: %s", device->name());
        }
        _INFO("device opened: %s", device->toString());
    } else {
        _ERROR("open device failed! device not found: %s", devInfo.toString());
    }
    this->onDeviceListUpdate();
    return result;
}

bool LibusbMgr::setupDeviceToAccessory(const LibusbDeviceInfo& devInfo, const AOAInfo& aoaInfo) {
    LOCK_MUTEX(m_proc_lock);
    bool result = false;
    if (LibusbDevice* device = findDevice(devInfo)) {
        result = AOAProtocol::setupDeviceToAccessory(*device, aoaInfo);
        if (!result) {
            _ERROR("setup device(%s) to accessory failed!", device->name());
        }
    } else {
        _ERROR("setup device to accessory failed! device not found: %s", devInfo.toString());
    }
    this->onDeviceListUpdate();
    return result;
}

bool LibusbMgr::closeDevice(const LibusbDeviceInfo& devInfo) {
    LOCK_MUTEX(m_proc_lock);
    bool result = false;
    if (LibusbDevice* device = findDevice(devInfo)) {
        device->close();
        result = true;
        _INFO("device closed: %s", device->toString());
    } else {
        _ERROR("close device failed! device not found: %s", devInfo.toString());
    }
    this->onDeviceListUpdate();
    return result;
}

// 注意加锁
LibusbDevice* LibusbMgr::findDevice(const LibusbDeviceInfo& dev_info) {
    for (auto& device : m_devices) {
        if (device == dev_info) {
            return &device;
        }
    }
    return nullptr;
}

std::list<LibusbDevice> LibusbMgr::listDevices() {
    std::list<LibusbDevice> devices;
    if (m_usb_context == nullptr) {
        _ERROR("libusb context is null");
        return devices;
    }
    libusb_device** list;
    ssize_t size = libusb_get_device_list(m_usb_context, &list);
    for (ssize_t i= 0; i< size; ++i) {
        libusb_device* dev = list[i];
        libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(dev, &desc);
        if (ret != LIBUSB_SUCCESS) {
            _ERROR("libusb_get_device_descriptor(%p) failed: %s", dev, LibusbUtils::errString(ret));
            continue;
        }
        LibusbDevice usbDevice(dev, desc);
        usbDevice.prepare();
        devices.push_back(usbDevice);
    }
    libusb_free_device_list(list, 1);
    return devices;
}

// 注意加锁
void LibusbMgr::onDeviceListUpdate() {
    if (m_listener) {
        std::list<LibusbDeviceInfo> devices;
        for (auto& device : m_devices) {
            devices.push_back(device.info());
        }
        m_listener->onDeviceListUpdate(devices);
    }
}


void LibusbMgr::release() {
    LOCK_MUTEX(m_proc_lock);

    m_is_listening = false;
    libusb_interrupt_event_handler(m_usb_context);
    m_event_thread.quit();
    m_work_thread.quit();

    for (auto& device : m_devices) {
        device.close();
    }
    m_devices.clear();

    if (m_hotplug_cb_handle) {
        libusb_hotplug_deregister_callback(m_usb_context, m_hotplug_cb_handle);
        m_hotplug_cb_handle = 0;
    }

    if (m_usb_context) {
        libusb_exit(m_usb_context);
        m_usb_context = nullptr;
        _INFO("LibusbMgr released");
    }
}


NAMESPACE_END