//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "PreviewOutput.h"

NAMESPACE_DEFAULT

static CallbackMgr<PreviewOutput, PreviewCallback> g_callback_mgr;

static void onFrameStart(Camera_PreviewOutput *previewOutput) {
    auto *it = g_callback_mgr.findCallback((void *)previewOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onFrameStart: previewOutput(%x) is not found.", previewOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onFrameStart(cb.first);
    }
}

static void onFrameEnd(Camera_PreviewOutput *previewOutput, int frameCount) {
    auto *it = g_callback_mgr.findCallback((void *)previewOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onFrameEnd: previewOutput(%x) is not found.", previewOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onFrameEnd(cb.first, frameCount);
    }
}

static void onPreviewError(Camera_PreviewOutput *previewOutput, Camera_ErrorCode errorCode) {
    auto *it = g_callback_mgr.findCallback((void *)previewOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onPreviewError: previewOutput(%x) is not found.", previewOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onError(cb.first, errorCode);
    }
}

static PreviewOutput_Callbacks g_preview_ouput_callback = {
    .onFrameStart = onFrameStart,
    .onFrameEnd = onFrameEnd,
    .onError = onPreviewError
};

CamErrorCode PreviewOutput::setCallback(PreviewCallback *callback) {
    _FATAL_IF(m_output == nullptr, "Camera_PreviewOutput is null while registerCallback()")

    if (!g_callback_mgr.hasAnyCallback(m_output)) {
        if (callback == nullptr) {
            return CAMERA_OK;
        }
        // register callback
        CamErrorCode code = OH_PreviewOutput_RegisterCallback(m_output, &g_preview_ouput_callback);
        _ERROR_RETURN_IF(code != CAMERA_OK, code, "Camera_PreviewOutput_RegisterCallback failed: %s", CamUtils::errString(code));
    } else {
        g_callback_mgr.clearCallback(m_output);
        if (callback != nullptr) {
            g_callback_mgr.addCallback(m_output, *this, callback);
        } else {
            auto err = OH_PreviewOutput_UnregisterCallback(m_output, &g_preview_ouput_callback);
            _ERROR_RETURN_IF(err, err, "OH_PreviewOutput_UnregisterCallback failed: %s", CamUtils::errString(err));
        }
    }
    if (callback) {
        _INFO("preview output register callback(%p) success.", callback);
    } else {
        _INFO("preview output unregister callback success.");
    }
    return CAMERA_OK;
}

//CamErrorCode PreviewOutput::unregisterCallback(PreviewCallback *callback) {
//    _FATAL_IF(m_output == nullptr, "Camera_PreviewOutput is null while unregisterCallback()")
//
//    if (!callback_helper.hasAnyCallback(m_output)) {
//        _WARN("unregisterCallback: previewOutput(%x) is not found.", m_output);
//        return CAMERA_OK;
//    }
//    callback_helper.removeCallback(m_output, callback);
//    if (!callback_helper.hasAnyCallback(m_output)) {
//        // unregister callback
//        CamErrorCode error = OH_PreviewOutput_UnregisterCallback(m_output, &previewOutputCallbacks);
//        _FATAL_IF(error, "Camera_PreviewOutput_UnregisterCallback failed: %d", error)
//    }
//    _INFO("preview output unregister callback(%p) success.", callback);
//    return CAMERA_OK;
//}

CamErrorCode PreviewOutput::start() {
    _FATAL_IF(m_output == nullptr, "Camera_PreviewOutput is null while start()")
    if (m_started) {
        return CAMERA_OK;
    }

    CamErrorCode error = OH_PreviewOutput_Start(m_output);
    _ERROR_RETURN_IF(error, error, "OH_PreviewOutput_Start failed: %s", CamUtils::errString(error))

    m_started = true;
    _INFO("preview output start success.");
    return CAMERA_OK;
}

CamErrorCode PreviewOutput::stop() {
    _FATAL_IF(m_output == nullptr, "Camera_PreviewOutput is null while stop()")
    if (!m_started) {
        return CAMERA_OK;
    }

    CamErrorCode error = OH_PreviewOutput_Stop(m_output);
    _ERROR_RETURN_IF(error, error, "OH_PreviewOutput_Stop failed: %s", CamUtils::errString(error))

    m_started = false;
    _INFO("preview output stop success.");
    return CAMERA_OK;
}

CamErrorCode PreviewOutput::release() {
    _WARN_RETURN_IF(m_output == nullptr, CAMERA_OK, "Camera_PreviewOutput already released.")
    
    m_started = false;
    // 清除 callback
    setCallback(nullptr);

    CamErrorCode error = OH_PreviewOutput_Release(m_output);
    m_output = nullptr;
    _WARN_RETURN_IF(error, error, "OH_PreviewOutput_Release failed: %s", CamUtils::errString(error))
    _INFO("preview output release successfully.");
    return CAMERA_OK;
}

NAMESPACE_END