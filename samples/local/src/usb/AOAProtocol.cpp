//
// Created by LiangKeJin on 25-3-23.
//

#include "AOAProtocol.h"

NAMESPACE_DEFAULT

#define     VID_GOOGLE              0x18D1
#define     PID_AOA_ACC             0x2D00
#define     PID_AOA_ACC_ADB         0x2D01
#define     PID_AOA_AU              0x2D02
#define     PID_AOA_AU_ADB          0x2D03
#define     PID_AOA_ACC_AU          0x2D04
#define     PID_AOA_ACC_AU_ADB      0x2D05

bool AOAProtocol::isAccessory(uint16_t vid, uint16_t pid) {
    if (vid == VID_GOOGLE) {
        switch (pid) {
        case PID_AOA_ACC:
        case PID_AOA_ACC_ADB:
        case PID_AOA_ACC_AU:
        case PID_AOA_ACC_AU_ADB:
            return true;

            //音频
        case PID_AOA_AU:
        case PID_AOA_AU_ADB:
            break;

        default:
            break;
        }
    }

    return false;
}

bool AOAProtocol::setupDeviceToAccessory(LibusbDevice& device, const AOAInfo& info) {
    _INFO("start setup accessory mode, info: {\n"
       "    manufacturer: %s\n"
       "    model: %s\n"
       "    description: %s\n"
       "    serial: %s\n"
       "    version: %d\n"
       "    uri: %s\n}",
       info.manufacturer, info.model, info.description, info.serial, info.version, info.uri);

    if (!connectAccessoryImpl(device, info)) {
        _ERROR("Failed to setup accessory, close device");
        device.close();
        return false;
    }
    _INFO("accessory setup success");
    return true;
}

bool AOAProtocol::connectAccessoryImpl(LibusbDevice& device, const AOAInfo& info) {
    int ret = device.open();
    if (ret != LIBUSB_SUCCESS) {
        return false;
    }

    ret = device.active(0);
    if (ret != LIBUSB_SUCCESS) {
        // 不管是否激活成功，都继续执行
    }

    unsigned char ioBuffer[2] = {0};
    ret = device.transCtrl(0xc0, 51, 0, 0, ioBuffer, 2, 1000);
    if (ret < 0) {
        return false;
    }

    int aoaVersion = ioBuffer[1] << 8 | ioBuffer[0];
    if (aoaVersion != 1 && aoaVersion != 2) {
        _ERROR("wrong aoa version: %d", aoaVersion);
        return false;
    }
    _INFO("aoa version: %d", aoaVersion);

    uint8_t* data[6] = {
        (uint8_t*)info.manufacturer.c_str(),
        (uint8_t*)info.model.c_str(),
        (uint8_t*)info.description.c_str(),
        (uint8_t*)info.serial.c_str(),
        (uint8_t*)info.version.c_str(),
        (uint8_t*)info.uri.c_str(),
    };

    for (int i = 0; i < 6; ++i) {
        ret = device.transCtrl(0x40, 52, 0, i, data[i], strlen((char*)data[i]), 500);
        if (ret < 0) {
            _ERROR("Failed to send aoa info: %s", data[i]);
            return false;
        }
    }

    // 发送序号为53的USB报文，切换USB模式
    ret = device.transCtrl(0x40, 53, 0, 0, nullptr, 0, 1000);
    if (ret < 0) {
        return false;
    }

    return true;
}

bool AOAProtocol::openAccessory(LibusbDevice& device) {
    _INFO("start open accessory: %s", device.toString());
    auto configs = device.getConfigs();

    libusb_endpoint_transfer_type transferType = LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK;
    LibusbInterfaceSetting interfaceSetting;
    for (auto& config : configs) {
        interfaceSetting = config.findInterface(LIBUSB_CLASS_VENDOR_SPEC,
            LIBUSB_CLASS_VENDOR_SPEC, transferType, true, true);
        if (interfaceSetting.valid()) {
            break;
        }
    }
    if (!interfaceSetting.valid()) {
        _ERROR("Failed to find transfer interface, open accessory failed");
        return false;
    }
    _INFO("find transfer interface: %s", interfaceSetting.toString());
    int ret = device.open();
    if (ret!= LIBUSB_SUCCESS) {
        _ERROR("Failed to open device, open accessory failed");
        return false;
    }
    ret = device.claimInterface(interfaceSetting, transferType);
    if (ret!= LIBUSB_SUCCESS) {
        _ERROR("Failed to claim interface, open accessory failed");
        return false;
    }
    _INFO("open accessory success");
    return true;
}


NAMESPACE_END