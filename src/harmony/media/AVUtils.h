//
// Created on 2024/8/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include <multimedia/player_framework/native_averrors.h>

NAMESPACE_DEFAULT

class AVUtils {
public:
    static std::string errString(int code) {
        switch (code) {
        case AV_ERR_OK :
            return "AV_ERR_OK";
        case AV_ERR_NO_MEMORY :
            return "AV_ERR_NO_MEMORY";
        case AV_ERR_OPERATE_NOT_PERMIT :
            return "AV_ERR_OPERATE_NOT_PERMIT";
        case AV_ERR_INVALID_VAL :
            return "AV_ERR_INVALID_VAL";
        case AV_ERR_IO :
            return "AV_ERR_IO";
        case AV_ERR_TIMEOUT :
            return "AV_ERR_TIMEOUT";
        case AV_ERR_UNKNOWN :
            return "AV_ERR_UNKNOWN";
        case AV_ERR_SERVICE_DIED :
            return "AV_ERR_SERVICE_DIED";
        case AV_ERR_INVALID_STATE :
            return "AV_ERR_INVALID_STATE";
        case AV_ERR_UNSUPPORT :
            return "AV_ERR_UNSUPPORT";
        case AV_ERR_EXTEND_START :
            return "AV_ERR_EXTEND_START";
        case AV_ERR_DRM_BASE :
            return "AV_ERR_DRM_BASE";
        case AV_ERR_DRM_DECRYPT_FAILED :
            return "AV_ERR_DRM_DECRYPT_FAILED";
        default :
            return std::string("UNKNOWN(") + std::to_string(code) + ")";
        }
    }
};

NAMESPACE_END
