//
// Created by LiangKeJin on 2025/3/16.
//

#pragma once

#include <common/utils/EventThread.h>

#include "AOAProtocol.h"
#include "LibusbDevice.h"
#include "LibusbDeviceTransfer.h"
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
private:
    struct DeviceBox {
        LibusbDevice dev;
        std::shared_ptr<LibusbDeviceTransfer> transfer = nullptr;

        void closeDevice() {
            dev.close();
            if (transfer) {
                transfer->stop();
                transfer = nullptr;
            }
        }

        void release() {
            dev.close();
            if (transfer) {
                transfer->release();
                transfer = nullptr;
            }
        }
    };
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
     * 设置设备到 accessory 模式
     * @param devInfo 设备信息
     * @return 是否设置成功
     */
    bool setupDeviceToAccessory(const LibusbDeviceInfo& devInfo, const AOAInfo& aoaInfo);

    /**
     * 打开设备, 如果是 AOA 设备, 则会直接 claim 接口, 不需要调用 claimInterface
     * @param devInfo 设备信息
     * @return 是否打开成功
     */
    std::shared_ptr<LibusbDeviceTransfer> openDevice(const LibusbDeviceInfo& devInfo);

    /**
     * 获取设备配置信息
     * @param devInfo 设备信息
     * @return 配置信息列表
     */
    std::list<LibusbConfig> getDeviceConfig(const LibusbDeviceInfo& devInfo);

    /**
     * 声明读写接口
     * @param devInfo 设备信息
     * @param settings 接口设置
     * @param type 传输类型
     * @return 是否声明成功
     */
    bool claimInterface(const LibusbDeviceInfo& devInfo,
        const LibusbInterfaceSetting settings, const libusb_endpoint_transfer_type type);

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

    DeviceBox* findDevice(const LibusbDeviceInfo& dev_info);

    void onDeviceListUpdate();

private:
    libusb_context* m_usb_context = nullptr;
    libusb_hotplug_callback_handle m_hotplug_cb_handle = 0;
    EventThread m_event_thread;
    EventThread m_work_thread;
    bool m_is_listening = false;

    std::mutex m_proc_lock;
    std::list<DeviceBox> m_devices;
    LibusbDeviceListener* m_listener = nullptr;
};

NAMESPACE_END
