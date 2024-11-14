//
// Created on 2024/9/10.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "MediaAssetMgr.h"
#include "harmony/masset/MAssetUtils.h"

NAMESPACE_DEFAULT

MediaAssetMgr MediaAssetMgr::g_instance;

static std::mutex g_data_prepared_mutex;
static std::unordered_map<std::string, ML_DataPreparedCallback> g_data_prepared_callbacks;
static std::unordered_map<std::string, int> g_data_result_map;

static void onDataPrepared(int result, MediaLibrary_RequestId requestId) {
    _INFO("onDataPrepared: result: %d, request id: %s", result, requestId.requestId);

    std::string key(requestId.requestId);
    std::lock_guard<std::mutex> locker(g_data_prepared_mutex);

    auto cb = g_data_prepared_callbacks.find(key);
    if (cb == g_data_prepared_callbacks.end()) {
        g_data_result_map.insert(std::make_pair(key, result));
        return;
    }
    g_data_prepared_callbacks.erase(cb);
    cb->second(result, key);
}

std::string MediaAssetMgr::requestPath(ML_MediaType mediaType, const char *uri, ML_DeliveryMode deliverMode,
                                       const char *dstPath, ML_DataPreparedCallback callback) {
    _INFO("request %s path, uri: %s, delivery mode: %s, dst path: %s", MAssetUtils::mediaTypeStr(mediaType), uri,
          MAssetUtils::deliveryModeStr(deliverMode), dstPath);

    MediaLibrary_RequestOptions options = {.deliveryMode = deliverMode};
    MediaLibrary_RequestId requestId =
        mediaType == MediaLibrary_MediaType::MEDIA_LIBRARY_IMAGE
            ? OH_MediaAssetManager_RequestImageForPath(getMgr(), uri, options, dstPath, onDataPrepared)
            : OH_MediaAssetManager_RequestVideoForPath(getMgr(), uri, options, dstPath, onDataPrepared);
    
    std::lock_guard<std::mutex> locker(g_data_prepared_mutex);
    std::string id(requestId.requestId);
    auto result = g_data_result_map.find(id);
    if (result != g_data_result_map.end()) {
        callback(result->second, id);
    } else {
        g_data_prepared_callbacks.insert(std::make_pair(requestId.requestId, callback));
    }

    return std::string(requestId.requestId);
}

struct ImageRequest {
    int callback_count;
    ML_ImageDataPreparedCallback callback;
};
static std::mutex g_image_data_prepared_mutex;
static std::unordered_map<std::string, ImageRequest> g_image_data_prepared_callbacks;

struct ImageResult {
    MediaLibrary_ErrorCode result;
    MediaLibrary_MediaQuality media_quality;
    MediaLibrary_MediaContentType type;
    OH_ImageSourceNative *image_source_native;
};
static std::unordered_map<std::string, ImageResult> g_image_data_map;

static void onImageDataPrepared(MediaLibrary_ErrorCode result, MediaLibrary_RequestId requestId,
                                MediaLibrary_MediaQuality mediaQuality, MediaLibrary_MediaContentType type,
                                OH_ImageSourceNative *imageSourceNative) {
    _INFO("onImageDataPrepared, result: %s, request id: %s, quality: %s, content type: %s",
          MAssetUtils::errString(result), requestId.requestId, MAssetUtils::qualityStr(mediaQuality),
          MAssetUtils::mediaContentTypeStr(type));
    std::lock_guard<std::mutex> locker(g_image_data_prepared_mutex);

    std::string key(requestId.requestId);
    auto cb = g_image_data_prepared_callbacks.find(key);
    if (cb == g_image_data_prepared_callbacks.end()) {
        ImageResult imageResult = {
            .result = result,
            .media_quality = mediaQuality,
            .type = type,
            .image_source_native = imageSourceNative
        };
        g_image_data_map.insert(std::make_pair(key, imageResult));
        return;
    }
    
    cb->second.callback_count -= 1;
    if (cb->second.callback_count < 1) {
        g_image_data_prepared_callbacks.erase(cb);
    }
    NImageSource source(imageSourceNative);
    cb->second.callback(result, key, mediaQuality, type, source);
}

std::string MediaAssetMgr::requestImage(OH_MediaAsset *asset, ML_DeliveryMode deliveryMode,
                                        ML_ImageDataPreparedCallback callback) {

    MediaLibrary_RequestOptions options = {.deliveryMode = deliveryMode};

    MediaLibrary_RequestId requestId;
    ML_ErrorCode error = OH_MediaAssetManager_RequestImage(getMgr(), asset, options, &requestId, onImageDataPrepared);
    _ERROR_RETURN_IF(error, "", "requestImage failed: %s", MAssetUtils::errString(error));

    
    ImageRequest request = {.callback_count = 1, .callback = callback};
    if (deliveryMode == ML_DeliveryMode::MEDIA_LIBRARY_BALANCED_MODE) {
        request.callback_count = 2;
    }

    std::lock_guard<std::mutex> locker(g_image_data_prepared_mutex);
    std::string id(requestId.requestId);
    if (g_image_data_prepared_callbacks.find(id) == g_image_data_prepared_callbacks.end()) {
        g_image_data_prepared_callbacks.insert(std::make_pair(id, request));
    }
    auto imageData = g_image_data_map.find(id);
    if (imageData != g_image_data_map.end()) {
        // 数据先返回了
        auto cb = g_image_data_prepared_callbacks.find(id);
        cb->second.callback_count -= 1;
        if (cb->second.callback_count < 1) {
            g_image_data_prepared_callbacks.erase(cb);
        }
        auto &imgResult = imageData->second;
        NImageSource source(imgResult.image_source_native);
        cb->second.callback(imgResult.result, id, imgResult.media_quality, imgResult.type, source);
        
        g_image_data_map.erase(imageData);
    }

    return id;
}

bool MediaAssetMgr::cancelRequest(const std::string &id) {
    {
        std::lock_guard<std::mutex> locker(g_data_prepared_mutex);
        g_data_prepared_callbacks.erase(id);
    }
    {
        std::lock_guard<std::mutex> locker(g_image_data_prepared_mutex);
        g_image_data_prepared_callbacks.erase(id);
    }

    MediaLibrary_RequestId requestId;
    strcpy(requestId.requestId, id.c_str());
    return OH_MediaAssetManager_CancelRequest(getMgr(), requestId);
}

NAMESPACE_END