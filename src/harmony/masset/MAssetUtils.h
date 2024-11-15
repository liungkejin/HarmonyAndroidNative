//
// Created on 2024/9/10.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/Log.h"
#include <cstdint>
#include <multimedia/media_library/media_asset_base_capi.h>

NAMESPACE_DEFAULT

typedef MediaLibrary_ErrorCode ML_ErrorCode;
typedef MediaLibrary_DeliveryMode ML_DeliveryMode;
typedef MediaLibrary_MediaType ML_MediaType;
typedef MediaLibrary_RequestId ML_RequestId;
typedef MediaLibrary_MediaQuality ML_MediaQuality;
typedef MediaLibrary_MediaContentType ML_MediaContentType;

class MAssetUtils {
public:
    static std::string errString(MediaLibrary_ErrorCode code) {
        switch (code) {
        case MEDIA_LIBRARY_OK :
            return "OK";
        case MEDIA_LIBRARY_PERMISSION_DENIED :
            return "PERMISSION_DENIED";
        case MEDIA_LIBRARY_PARAMETER_ERROR :
            return "PARAMETER_ERROR";
        case MEDIA_LIBRARY_NO_SUCH_FILE :
            return "NO_SUCH_FILE";
        case MEDIA_LIBRARY_INVALID_DISPLAY_NAME :
            return "INVALID_DISPLAY_NAME";
        case MEDIA_LIBRARY_INVALID_ASSET_URI :
            return "INVALID_ASSET_URI";
        case MEDIA_LIBRARY_INVALID_PHOTO_KEY :
            return "INVALID_PHOTO_KEY";
        case MEDIA_LIBRARY_OPERATION_NOT_SUPPORTED :
            return "OPERATION_NOT_SUPPORTED";
        case MEDIA_LIBRARY_INTERNAL_SYSTEM_ERROR :
            return "INTERNAL_SYSTEM_ERROR";
        default :
            std::string err = "UNKNOWN_ERROR: " + std::to_string((uint32_t)code);
            return err;
        }
    }

    static std::string deliveryModeStr(MediaLibrary_DeliveryMode mode) {
        switch (mode) {
        case MEDIA_LIBRARY_FAST_MODE :
            return "DeliveryMode::FAST_MODE";
        case MEDIA_LIBRARY_HIGH_QUALITY_MODE :
            return "DeliveryMode::HIGH_QUALITY_MODE";
        case MEDIA_LIBRARY_BALANCED_MODE :
            return "DeliveryMode::BALANCED_MODE";
        default :
            std::string err = "UNKNOWN_DELIVERYMODE: " + std::to_string((uint32_t)mode);
            return err;
        }
    }

    static std::string mediaTypeStr(MediaLibrary_MediaType type) {
        switch (type) {
        case MEDIA_LIBRARY_IMAGE :
            return "MediaType::IMAGE";
        case MEDIA_LIBRARY_VIDEO :
            return "MediaType::VIDEO";
        default :
            std::string err = "UNKNOWN_MEDIATYPE: " + std::to_string((uint32_t)type);
            return err;
        }
    }

    static std::string mediaContentTypeStr(MediaLibrary_MediaContentType type) {
        switch (type) {
        case MEDIA_LIBRARY_COMPRESSED :
            return "MediaContentType::COMPRESSED";
        case MEDIA_LIBRARY_PICTURE_OBJECT :
            return "MediaContentType::PICTURE_OBJECT";
        default :
            std::string err = "UNKNOWN_MediaContentType: " + std::to_string((uint32_t)type);
            return err;
        }
    }

    static std::string qualityStr(MediaLibrary_MediaQuality quality) {
        switch (quality) {
        case MEDIA_LIBRARY_QUALITY_FAST :
            return "QUALITY_FAST";
        case MEDIA_LIBRARY_QUALITY_FULL :
            return "QUALITY_FULL";

        default :
            std::string err = "UNKNOWN_QUANLITY: " + std::to_string((uint32_t)quality);
            return err;
        }
    }
};

NAMESPACE_END
