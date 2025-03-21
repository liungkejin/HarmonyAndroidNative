//
// Created by LiangKeJin on 2025/3/21.
//

#pragma once

#include <libusb.h>
#include <string>

#include "ZNamespace.h"

NAMESPACE_DEFAULT
class LibusbUtils {
public:
    static std::string toHexString(const uint64_t val) {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "0x%llX", val);
        return std::string(buf);
    }

    static std::string errString(const int err) {
        switch (err) {
        case LIBUSB_ERROR_IO:
            return "LIBUSB_ERROR_IO";
        case LIBUSB_ERROR_INVALID_PARAM:
            return "LIBUSB_ERROR_INVALID_PARAM";
        case LIBUSB_ERROR_ACCESS:
            return "LIBUSB_ERROR_ACCESS";
        case LIBUSB_ERROR_NO_DEVICE:
            return "LIBUSB_ERROR_NO_DEVICE";
        case LIBUSB_ERROR_NOT_FOUND:
            return "LIBUSB_ERROR_NOT_FOUND";
        case LIBUSB_ERROR_BUSY:
            return "LIBUSB_ERROR_BUSY";
        case LIBUSB_ERROR_TIMEOUT:
            return "LIBUSB_ERROR_TIMEOUT";
        case LIBUSB_ERROR_OVERFLOW:
            return "LIBUSB_ERROR_OVERFLOW";
        case LIBUSB_ERROR_PIPE:
            return "LIBUSB_ERROR_PIPE";
        case LIBUSB_ERROR_INTERRUPTED:
            return "LIBUSB_ERROR_INTERRUPTED";
        case LIBUSB_ERROR_NO_MEM:
            return "LIBUSB_ERROR_NO_MEM";
        case LIBUSB_ERROR_NOT_SUPPORTED:
            return "LIBUSB_ERROR_NOT_SUPPORTED";
        case LIBUSB_ERROR_OTHER:
            return "LIBUSB_ERROR_OTHER";
        default:
            return std::string("Unknown error: ") + std::to_string(err);
        }
    }

    static std::string descTypeToString(const uint8_t type) {
        switch (type) {
        case LIBUSB_DT_DEVICE:
            return "LIBUSB_DT_DEVICE";
        case LIBUSB_DT_CONFIG:
            return "LIBUSB_DT_CONFIG";
        case LIBUSB_DT_STRING:
            return "LIBUSB_DT_STRING";
        case LIBUSB_DT_INTERFACE:
            return "LIBUSB_DT_INTERFACE";
        case LIBUSB_DT_ENDPOINT:
            return "LIBUSB_DT_ENDPOINT";
        case LIBUSB_DT_INTERFACE_ASSOCIATION:
            return "LIBUSB_DT_INTERFACE_ASSOCIATION";
        case LIBUSB_DT_BOS:
            return "LIBUSB_DT_BOS";
        case LIBUSB_DT_DEVICE_CAPABILITY:
            return "LIBUSB_DT_DEVICE_CAPABILITY";
        case LIBUSB_DT_HID:
            return "LIBUSB_DT_HID";
        case LIBUSB_DT_REPORT:
            return "LIBUSB_DT_REPORT";
        case LIBUSB_DT_PHYSICAL:
            return "LIBUSB_DT_PHYSICAL";
        case LIBUSB_DT_HUB:
            return "LIBUSB_DT_HUB";
        case LIBUSB_DT_SUPERSPEED_HUB:
            return "LIBUSB_DT_SUPERSPEED_HUB";
        case LIBUSB_DT_SS_ENDPOINT_COMPANION:
            return "LIBUSB_DT_SS_ENDPOINT_COMPANION";
        default:
            return std::string("Unknown type: ") + std::to_string(type);
        }
    }
};

NAMESPACE_END
