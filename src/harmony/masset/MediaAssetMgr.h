//
// Created on 2024/9/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "MAssetUtils.h"
#include "harmony/image/NImageSource.h"
#include <multimedia/media_library/media_asset_manager_capi.h>

NAMESPACE_DEFAULT

#define MEDIA_ASSET_MGR (MediaAssetMgr::getInstance())

typedef std::function<void(int result, std::string requestId)> ML_DataPreparedCallback;
typedef std::function<void(ML_ErrorCode result, std::string requestId, ML_MediaQuality quality,
                           ML_MediaContentType type, NImageSource &imgSource)>
    ML_ImageDataPreparedCallback;

class MediaAssetMgr {
public:
    static MediaAssetMgr& getInstance();

private:
    MediaAssetMgr() = default;

    OH_MediaAssetManager *getMgr() {
        if (m_manager == nullptr) {
            m_manager = OH_MediaAssetManager_Create();
            _ERROR_IF(m_manager == nullptr, "create media asset manager failed!");
        }
        return m_manager;
    }

public:
    std::string requestPath(ML_MediaType mediaType, const char *uri, ML_DeliveryMode deliverMode, const char *dstPath,
                            ML_DataPreparedCallback callback);

    bool cancelRequest(const std::string& id);

    std::string requestImage(OH_MediaAsset *asset, ML_DeliveryMode deliveryMode, ML_ImageDataPreparedCallback);

    /**
     * 清除所有无效的请求
     */
    void cleanIdleRequests();
private:
    OH_MediaAssetManager *m_manager = nullptr;
};

NAMESPACE_END
