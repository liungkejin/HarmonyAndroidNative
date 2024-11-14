//
// Created on 2024/9/10.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "common/Object.h"
#include "MAssetUtils.h"
#include <cstdint>
#include <multimedia/media_library/media_access_helper_capi.h>
#include <multimedia/media_library/media_asset_base_capi.h>
#include <multimedia/media_library/media_asset_capi.h>
#include <multimedia/media_library/media_asset_change_request_capi.h>

NAMESPACE_DEFAULT

class MediaAssetChangeRequest : Object {
public:
    MediaAssetChangeRequest(OH_MediaAssetChangeRequest *request, bool own = true) : m_request(request), m_own(own) {}
    MediaAssetChangeRequest(const MediaAssetChangeRequest &o) : m_request(o.m_request), m_own(o.m_own), Object(o) {}

    ~MediaAssetChangeRequest() {
        if (m_own && no_reference() && m_request) {
            OH_MediaAssetChangeRequest_Release(m_request);
            m_request = nullptr;
        }
    }

public:
    inline ML_ErrorCode addImageBuffer(uint8_t *buffer, uint32_t length) {
        return addResourceWithBuffer(MEDIA_LIBRARY_IMAGE_RESOURCE, buffer, length);
    }

    inline ML_ErrorCode addVideoBuffer(uint8_t *buffer, uint32_t length) {
        return addResourceWithBuffer(MEDIA_LIBRARY_VIDEO_RESOURCE, buffer, length);
    }

    ML_ErrorCode addResourceWithBuffer(MediaLibrary_ResourceType resourceType, uint8_t *buffer, uint32_t length) {
        _ASSERT(m_request == nullptr)

        ML_ErrorCode error = OH_MediaAssetChangeRequest_AddResourceWithBuffer(m_request, resourceType, buffer, length);
        _ERROR_RETURN_IF(error, error, "AddResourceWithBuffer failed: %s", MAssetUtils::errString(error));

        return error;
    }

    ML_ErrorCode saveCameraPhoto(MediaLibrary_ImageFileType fileType = MEDIA_LIBRARY_IMAGE_JPEG) {
        _ASSERT(m_request == nullptr)
        ML_ErrorCode error = OH_MediaAssetChangeRequest_SaveCameraPhoto(m_request, fileType);
        _ERROR_RETURN_IF(error, error, "SaveCameraPhoto failed: %s", MAssetUtils::errString(error));
        return error;
    }
    
    ML_ErrorCode discardCameraPhoto() {
        _ASSERT(m_request == nullptr)
        ML_ErrorCode error = OH_MediaAssetChangeRequest_DiscardCameraPhoto(m_request);
        _ERROR_RETURN_IF(error, error, "DiscardCameraPhoto failed: %s", MAssetUtils::errString(error));
        return error;
    }
    
    ML_ErrorCode applyChanges() {
        _ASSERT(m_request == nullptr)
        
        ML_ErrorCode error = OH_MediaAccessHelper_ApplyChanges(m_request);
        _ERROR_RETURN_IF(error, error, "ApplyChanges failed: %s", MAssetUtils::errString(error));
        return error;
    }

private:
    bool m_own;
    OH_MediaAssetChangeRequest *m_request;
};

class MediaAsset : Object {
public:
    MediaAsset(OH_MediaAsset *asset, bool own = true) : m_asset(asset), m_own(own) {}

    MediaAsset(const MediaAsset &o) : m_asset(o.m_asset), m_own(o.m_own), Object(o) {}

    ~MediaAsset() {
        if (m_own && no_reference() && m_asset) {
            OH_MediaAsset_Release(m_asset);
            m_asset = nullptr;
        }
    }
    
public:
    MediaAssetChangeRequest changeRequest() {
        _ASSERT(m_asset == nullptr)
        OH_MediaAssetChangeRequest *request = OH_MediaAssetChangeRequest_Create(m_asset);
        _FATAL_IF(request == nullptr, "create request failed!")
        return MediaAssetChangeRequest(request);
    }
    
    void discard() {
        MediaAssetChangeRequest request = changeRequest();
        request.discardCameraPhoto();
    }

public:
    std::string uri() {
        _ASSERT(m_asset == nullptr)

        const char *uri = nullptr;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetUri(m_asset, &uri);
        _ERROR_RETURN_IF(error || uri == nullptr, "", "GetUri failed: %s", MAssetUtils::errString(error));
        return uri;
    }

    std::string displayName() {
        _ASSERT(m_asset == nullptr)

        const char *name = nullptr;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetDisplayName(m_asset, &name);
        _ERROR_RETURN_IF(error || name == nullptr, "", "GetDisplayName failed: %s", MAssetUtils::errString(error));
        return name;
    }

    uint32_t size() {
        _ASSERT(m_asset == nullptr)

        uint32_t msize = 0;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetSize(m_asset, &msize);
        _ERROR_RETURN_IF(error, 0, "GetSize failed: %s", MAssetUtils::errString(error));
        return msize;
    }

    uint32_t modifiedMs() {
        _ASSERT(m_asset == nullptr)

        uint32_t dateModifiedMs = 0;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetDateModifiedMs(m_asset, &dateModifiedMs);
        _ERROR_RETURN_IF(error, 0, "GetDateModifiedMs failed: %s", MAssetUtils::errString(error));
        return dateModifiedMs;
    }

    uint32_t width() {
        _ASSERT(m_asset == nullptr)

        uint32_t w = 0;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetWidth(m_asset, &w);
        _ERROR_RETURN_IF(error, 0, "GetWidth failed: %s", MAssetUtils::errString(error))
        return w;
    }

    uint32_t height() {
        _ASSERT(m_asset == nullptr)

        uint32_t h = 0;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetHeight(m_asset, &h);
        _ERROR_RETURN_IF(error, 0, "GetHeight failed: %s", MAssetUtils::errString(error))
        return h;
    }

    uint32_t orientation() {
        _ASSERT(m_asset == nullptr)

        uint32_t o = 0;
        MediaLibrary_ErrorCode error = OH_MediaAsset_GetOrientation(m_asset, &o);
        _ERROR_RETURN_IF(error, 0, "GetOrientation failed: %s", MAssetUtils::errString(error))
        return o;
    }

public:
    inline OH_MediaAsset *value() { return m_asset; }

    std::string toString() {
        std::stringstream ss;
        ss << "uri: " << uri() << "\n"
           << "displayName: " << displayName() << "\n"
           << "size: " << width() << "x" << height() << "\n"
           << "length: " << size() << "\n"
           << "modified ms: " << modifiedMs() << "\n"
           << "orientation: " << orientation();
        return ss.str();
    }
private:
    bool m_own;
    OH_MediaAsset *m_asset;
};

NAMESPACE_END
