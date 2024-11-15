//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "Namespace.h"
#include "common/Log.h"
#include <ohaudio/native_audiostream_base.h>

NAMESPACE_DEFAULT

class ASUtils {
public:
    static std::string resultString(int result) {
        switch (result) {
            case AUDIOSTREAM_SUCCESS :
                return "SUCCESS";

            case AUDIOSTREAM_ERROR_INVALID_PARAM :
                return "INVALID_PARAM";

            case AUDIOSTREAM_ERROR_ILLEGAL_STATE :
                return "ILLEGAL_STATE";

            case AUDIOSTREAM_ERROR_SYSTEM :
                return "ERROR_SYSTEM";

            default:
                return std::string("UNKNOWN(") + std::to_string(result) + ")";
        }
    }
};

NAMESPACE_END
