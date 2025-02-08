//
// Created on 2024/8/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "common/Log.h"
#include <string>
#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/pixelmap_native.h>

NAMESPACE_DEFAULT

class ImageUtils {
public:
    static std::string errorString(int code) {
        switch (code) {
        case IMAGE_SUCCESS :
            return "SUCCESS";
        case IMAGE_BAD_PARAMETER :
            return "BAD_PARAMETER";
        case IMAGE_UNSUPPORTED_MIME_TYPE :
            return "UNSUPPORTED_MIME_TYPE";
        case IMAGE_UNKNOWN_MIME_TYPE :
            return "UNKNOWN_MIME_TYPE";
        case IMAGE_TOO_LARGE :
            return "TOO_LARGE";
        case IMAGE_UNSUPPORTED_OPERATION :
            return "UNSUPPORTED_OPERATION";
        case IMAGE_UNSUPPORTED_METADATA :
            return "UNSUPPORTED_METADATA";
        case IMAGE_UNSUPPORTED_CONVERSION :
            return "UNSUPPORTED_CONVERSION";
        case IMAGE_INVALID_REGION :
            return "INVALID_REGION";
        case IMAGE_ALLOC_FAILED :
            return "ALLOC_FAILED";
        case IMAGE_COPY_FAILED :
            return "COPY_FAILED";
        case IMAGE_UNKNOWN_ERROR :
            return "UNKNOWN_ERROR";
        case IMAGE_BAD_SOURCE :
            return "BAD_SOURCE";
        case IMAGE_DECODE_FAILED :
            return "DECODE_FAILED";
        case IMAGE_ENCODE_FAILED :
            return "ENCODE_FAILED";
        default :
            return std::string("UNKNOWN_ERROR_CODE_") + std::to_string(code);
        }
    }

    static std::string formatString(int format) {
        switch (format) {
        case PIXEL_FORMAT_RGB_565 :
            return "PIXEL_FORMAT_RGB_565";
        case PIXEL_FORMAT_RGBA_8888 :
            return "PIXEL_FORMAT_RGBA_8888";
        case PIXEL_FORMAT_BGRA_8888 :
            return "PIXEL_FORMAT_BGRA_8888";
        case PIXEL_FORMAT_RGB_888 :
            return "PIXEL_FORMAT_RGB_888";
        case PIXEL_FORMAT_ALPHA_8 :
            return "PIXEL_FORMAT_ALPHA_8";
        case PIXEL_FORMAT_RGBA_F16 :
            return "PIXEL_FORMAT_RGBA_F16";
        case PIXEL_FORMAT_NV21 :
            return "PIXEL_FORMAT_NV21";
        case PIXEL_FORMAT_NV12 :
            return "PIXEL_FORMAT_NV12";
        case PIXEL_FORMAT_RGBA_1010102 :
            return "PIXEL_FORMAT_RGBA_1010102";
        case PIXEL_FORMAT_YCBCR_P010 :
            return "PIXEL_FORMAT_YCBCR_P010";
        case PIXEL_FORMAT_YCRCB_P010 :
            return "PIXEL_FORMAT_YCRCB_P010";
        default :
            return std::string("UNKNOWN_FORMAT_") + std::to_string(format);
        }
    }

    static int formatBytes(int width, int height, int format) {
        switch (format) {
        case PIXEL_FORMAT_RGB_565 :
            return 2 * width * height;
        case PIXEL_FORMAT_RGBA_8888 :
        case PIXEL_FORMAT_BGRA_8888 :
            return 4 * width * height;
        case PIXEL_FORMAT_RGB_888 :
            return 3 * width * height;
        case PIXEL_FORMAT_ALPHA_8 :
            return 1 * width * height;
        case PIXEL_FORMAT_RGBA_F16 :
            return 8 * width * height;
        case PIXEL_FORMAT_NV21 :
        case PIXEL_FORMAT_NV12 :
            return width * height * 3 / 2;
        default :
            _FATAL("unknown pixel format: %d", format);
        }
    }
};

NAMESPACE_END
