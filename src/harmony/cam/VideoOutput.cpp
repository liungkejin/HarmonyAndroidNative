//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "VideoOutput.h"
#include "common/utils/CallbackMgr.h"

NAMESPACE_DEFAULT

static CallbackMgr<VideoOutput, VideoCallback> g_callback_mgr;

static void onFrameStart(Camera_VideoOutput *VideoOutput) {
    auto *it = g_callback_mgr.findCallback((void *)VideoOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onFrameStart: VideoOutput(%x) is not found.", VideoOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onFrameStart(cb.first);
    }
}

static void onFrameEnd(Camera_VideoOutput *VideoOutput, int frameCount) {
    auto *it = g_callback_mgr.findCallback((void *)VideoOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onFrameEnd: VideoOutput(%x) is not found.", VideoOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onFrameEnd(cb.first, frameCount);
    }
}

static void onError(Camera_VideoOutput *VideoOutput, Camera_ErrorCode errorCode) {
    auto *it = g_callback_mgr.findCallback((void *)VideoOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onError: VideoOutput(%x) is not found.", VideoOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onError(cb.first, errorCode);
    }
}

static VideoOutput_Callbacks g_video_callbacks = {
    .onFrameStart = onFrameStart, 
    .onFrameEnd = onFrameEnd, 
    .onError = onError
};

CamErrorCode VideoOutput::setCallback(VideoCallback *callback) {
    _FATAL_IF(m_output == nullptr, "Camera_VideoOutput is null while registerCallback()")

    if (!g_callback_mgr.hasAnyCallback(m_output)) {
        if (callback == nullptr) {
            return CAMERA_OK;
        }
        // register callback
        CamErrorCode code = OH_VideoOutput_RegisterCallback(m_output, &g_video_callbacks);
        _ERROR_RETURN_IF(code != CAMERA_OK, code, "Camera_VideoOutput_RegisterCallback failed: %d", code);
    } else {
        g_callback_mgr.clearCallback(m_output);
        if (callback != nullptr) {
            g_callback_mgr.addCallback(m_output, *this, callback);
        } else {
            auto err = OH_VideoOutput_UnregisterCallback(m_output, &g_video_callbacks);
            _ERROR_RETURN_IF(err, err, "OH_VideoOutput_UnregisterCallback failed: %d", err);
        }
    }
    if (callback) {
        _INFO("video output register callback(%p) success.", callback);
    } else {
        _INFO("video output unregister callback success.");
    }
    return CAMERA_OK;
}

CamErrorCode VideoOutput::start() {
    _FATAL_IF(m_output == nullptr, "Camera_VideoOutput is null while start()")

    CamErrorCode error = OH_VideoOutput_Start(m_output);
    _ERROR_RETURN_IF(error, error, "OH_VideoOutput_Start failed: %d", error)

    m_started = true;
    _INFO("video output start success.");
    return CAMERA_OK;
}

CamErrorCode VideoOutput::stop() {
    _FATAL_IF(m_output == nullptr, "Camera_VideoOutput is null while stop()")

    CamErrorCode error = OH_VideoOutput_Stop(m_output);
    _ERROR_RETURN_IF(error, error, "OH_VideoOutput_Stop failed: %d", error)

    m_started = false;
    _INFO("video output stop success.");
    return CAMERA_OK;
}

CamErrorCode VideoOutput::release() {
    _WARN_RETURN_IF(m_output == nullptr, CAMERA_OK, "Camera_VideoOutput already released.")

    if (m_started) {
        CamErrorCode error = OH_VideoOutput_Stop(m_output);
        _INFO_IF(error, "OH_VideoOutput_Stop failed: %d", error);
        m_started = false;
    }

    setCallback(nullptr);

    CamErrorCode error = OH_VideoOutput_Release(m_output);
    m_output = nullptr;
    _WARN_RETURN_IF(error, error, "OH_VideoOutput_Release failed: %d", error)

    _INFO("video output release successfully.");
    return CAMERA_OK;
}


NAMESPACE_END