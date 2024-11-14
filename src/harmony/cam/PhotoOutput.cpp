//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "PhotoOutput.h"
#include "common/utils/CallbackMgr.h"
#include <cstdint>

NAMESPACE_DEFAULT

static CallbackMgr<PhotoOutput, PhotoCallback> g_callback_mgr;

static void onFrameStart(Camera_PhotoOutput* output) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "PhotoCallback::onFrameStart output(%p) not found", output);
    
    for (auto &cb : *it) {
        cb.second->onFrameStart(cb.first);
    }
}

static void onFrameShutter(Camera_PhotoOutput* output, Camera_FrameShutterInfo *info) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "PhotoCallback::onFrameShutter output(%p) not found", output);
    
    for (auto &cb : *it) {
        cb.second->onFrameShutter(cb.first, info);
    }
}

static void onFrameEnd(Camera_PhotoOutput* output, int32_t frameCount) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "PhotoCallback::onFrameEnd output(%p) not found", output);
    
    for (auto &cb : *it) {
        cb.second->onFrameEnd(cb.first, frameCount);
    }
}

static void onError(Camera_PhotoOutput* output, Camera_ErrorCode err) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "PhotoCallback::onError output(%p) not found", output);
    
    for (auto &cb : *it) {
        cb.second->onError(cb.first, err);
    }
}

static PhotoOutput_Callbacks g_callbacks = {
    .onFrameStart = onFrameStart,
    .onFrameShutter = onFrameShutter,
    .onFrameEnd = onFrameEnd,
    .onError = onError,
};

static void onCaptureReady(Camera_PhotoOutput *output) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "callback onCaptureReady output(%p) not found", output);
    for (auto &cb : *it) {
        cb.second->onCaptureReady(cb.first);
    }
}

static void onEstimatedCaptureDuration(Camera_PhotoOutput *output, int64_t duration) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "callback onEstimatedCaptureDuration output(%p) not found", output);
    for (auto &cb : *it) {
        cb.second->onEstimatedCaptureDuration(cb.first, duration);
    }
}

static void onPhotoAvailable(Camera_PhotoOutput *output, OH_PhotoNative *photo) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "callback onPhotoAvailable output(%p) not found", output);
    
    NativePhoto nativePhoto(photo, false);
    for (auto &cb : *it) {
        cb.second->onPhotoAvailable(cb.first, nativePhoto);
    }
}

static void onPhotoAssetAvailable(Camera_PhotoOutput *output, OH_MediaAsset *asset) {
    auto *it = g_callback_mgr.findCallback((void *)output);
    _WARN_RETURN_IF(it == nullptr, void(), "callback onEstimatedCaptureDuration output(%p) not found", output);
    
    MediaAsset *mediaAsset = new MediaAsset(asset);
    for (auto &cb : *it) {
        cb.second->onPhotoAssetAvailable(cb.first, mediaAsset);
    }
}

CamErrorCode PhotoOutput::setCallback(PhotoCallback *callback) {
    _WARN_RETURN_IF(m_output == nullptr, CAMERA_STATUS_UNAVAILABLE, "Camera_PhotoOutput is nullptr while setCallback");
    
    g_callback_mgr.clearCallback(m_output);
    if (callback != nullptr) {
        g_callback_mgr.addCallback(m_output, *this, callback);
    }
    return CAMERA_OK;
}

void PhotoOutput::setShutterCallbackEnable(bool enable) {
    if (enable) {
        auto err = OH_PhotoOutput_RegisterCallback(m_output, &g_callbacks);
        _ERROR_IF(err, "Camera_PhotoOutput_registerCallbacks failed: %s", CamUtils::errString(err));
    } else {
        auto err = OH_PhotoOutput_UnregisterCallback(m_output, &g_callbacks);
        _ERROR_IF(err, "Camera_PhotoOutput_unregisterCallbacks failed: %s", CamUtils::errString(err));
    }
}

void PhotoOutput::setCaptureReadyCallbackEnable(bool enable) {
    if (enable) {
        auto err = OH_PhotoOutput_RegisterCaptureReadyCallback(m_output, onCaptureReady);
        _ERROR_IF(err, "RegisterCaptureReadyCallback failed: %s", CamUtils::errString(err));
    } else {
        auto err = OH_PhotoOutput_UnregisterCaptureReadyCallback(m_output, onCaptureReady);
        _ERROR_IF(err, "UnregisterCaptureReadyCallback failed: %s", CamUtils::errString(err));
    }
}

void PhotoOutput::setEstimatedDurationCallbackEnable(bool enable) {
    if (enable) {
        auto err = OH_PhotoOutput_RegisterEstimatedCaptureDurationCallback(m_output, onEstimatedCaptureDuration);
        _ERROR_IF(err, "RegisterEstimatedCaptureDurationCallback failed: %s", CamUtils::errString(err));
    } else {
        auto err = OH_PhotoOutput_UnregisterEstimatedCaptureDurationCallback(m_output, onEstimatedCaptureDuration);
        _ERROR_IF(err, "UnregisterEstimatedCaptureDurationCallback failed: %s", CamUtils::errString(err));
    }
}

void PhotoOutput::setNativePhotoCallbackEnable(bool enable) {
    if (enable) {
        auto err = OH_PhotoOutput_RegisterPhotoAvailableCallback(m_output, onPhotoAvailable);
        _ERROR_IF(err, "RegisterPhotoAvailableCallback failed: %s", CamUtils::errString(err));
    } else {
        auto err = OH_PhotoOutput_UnregisterPhotoAvailableCallback(m_output, onPhotoAvailable);
        _ERROR_IF(err, "UnregisterPhotoAvailableCallback failed: %s", CamUtils::errString(err));
    }
}

void PhotoOutput::setPhotoAssetCallbackEnable(bool enable) {
    if (enable) {
        auto err = OH_PhotoOutput_RegisterPhotoAssetAvailableCallback(m_output, onPhotoAssetAvailable);
        _ERROR_IF(err, "RegisterPhotoAvailableCallback failed: %s", CamUtils::errString(err));
    } else {
        auto err = OH_PhotoOutput_UnregisterPhotoAssetAvailableCallback(m_output, onPhotoAssetAvailable);
        _ERROR_IF(err, "UnregisterPhotoAvailableCallback failed: %s", CamUtils::errString(err));
    }
}

CamErrorCode PhotoOutput::capture(const PhotoCaptureSetting *setting) {
    _FATAL_IF(m_output == nullptr, "Camera_PhotoOutput is nullptr while capture");
    
    if (setting == nullptr) {
        auto err = OH_PhotoOutput_Capture(m_output);
        _ERROR_RETURN_IF(err, err, "Camera_PhotoOutput_Capture failed: %d", err);
    } else {
        auto err = OH_PhotoOutput_Capture_WithCaptureSetting(m_output, *setting);
        _ERROR_RETURN_IF(err, err, "Camera_PhotoOutput_CaptureWithSetting failed: %d", err);
    }
    
    return CAMERA_OK;
}

bool PhotoOutput::isMirrorSupported() {
    _FATAL_IF(m_output == nullptr, "Camera_PhotoOutput is nullptr while isMirrorSupported");
    
    bool mirrorSupported = false;
    auto err = OH_PhotoOutput_IsMirrorSupported(m_output, &mirrorSupported);
    _ERROR_RETURN_IF(err, err, "Camera_PhotoOutput_IsMirrorSupported failed: %d", err);
    
    return mirrorSupported;
}

CamErrorCode PhotoOutput::release() {
    _WARN_RETURN_IF(m_output == nullptr, CAMERA_OK, "Camera_PhotoOutput is already released");

    setCallback(nullptr);
    
    auto err = OH_PhotoOutput_Release(m_output);
    m_output = nullptr;
    _WARN_RETURN_IF(err, err, "Camera_PhotoOutput_Release failed: %d", err);
    _INFO("photo output released successfully");
    
    return CAMERA_OK;
}

NAMESPACE_END