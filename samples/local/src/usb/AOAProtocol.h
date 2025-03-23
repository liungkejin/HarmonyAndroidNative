//
// Created by LiangKeJin on 25-3-23.
//

#pragma once

#include "LibusbDevice.h"
#include "ZNamespace.h"

NAMESPACE_DEFAULT

struct AOAInfo {
    std::string manufacturer;
    std::string model;
    std::string description;
    std::string serial;
    std::string version;
    std::string uri;
};

class AOAProtocol {
public:
    static bool isAccessory(const LibusbDevice& device) {
        return isAccessory(device.vendorId(), device.productId());
    }

    /**
     * 判断当前设备是否是 Android Accessory
     * @param vid vendor id
     * @param pid product id
     * @return 判断当前设备是否是 Android Accessory
     */
    static bool isAccessory(uint16_t vid, uint16_t pid);

    /**
     * 连接 Android Accessory
     * @param device 设备
     * @param info 信息
     * @return 是否连接成功
     */
    static bool connectAccessory(LibusbDevice& device, const AOAInfo& info);

    /**
     * 打开 Android Accessory
     * 设置读写端点
     * @param device 设备
     * @return 是否打开成功
     */
    static bool openAccessory(LibusbDevice& device);

private:
    static bool connectAccessoryImpl(LibusbDevice& device, const AOAInfo& info);
};

NAMESPACE_END
