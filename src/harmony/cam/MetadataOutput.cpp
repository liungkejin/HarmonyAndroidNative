//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "MetadataOutput.h"
#include "common/utils/CallbackMgr.h"

NAMESPACE_DEFAULT

static CallbackMgr<MetadataOutput, MetadataCallback> g_callback_mgr;

static void onAvailable(Camera_MetadataOutput* metadataOutput, Camera_MetadataObject* metadataObject, uint32_t size) {
    auto *it = g_callback_mgr.findCallback((void *)metadataOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onFrameStart: MetadataOutput(%x) is not found.", metadataOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onAvailable(cb.first, metadataObject, size);
    }
}

static void onError(Camera_MetadataOutput *metadataOutput, Camera_ErrorCode errorCode) {
    auto *it = g_callback_mgr.findCallback((void *)metadataOutput);
    _WARN_RETURN_IF(it == nullptr, void(), "onError: MetadataOutput(%x) is not found.", metadataOutput)

    // callback
    for (auto &cb : *it) {
        cb.second->onError(cb.first, errorCode);
    }
}

static MetadataOutput_Callbacks g_callbacks = {
    .onMetadataObjectAvailable = onAvailable,
    .onError = onError
};

CamErrorCode MetadataOutput::setCallback(MetadataCallback *callback) {
    _FATAL_IF(m_output == nullptr, "Camera_MetadataOutput is null while registerCallback()")

    if (!g_callback_mgr.hasAnyCallback(m_output)) {
        if (callback == nullptr) {
            return CAMERA_OK;
        }
        // register callback
        CamErrorCode code = OH_MetadataOutput_RegisterCallback(m_output, &g_callbacks);
        _ERROR_RETURN_IF(code != CAMERA_OK, code, "Camera_MetadataOutput_RegisterCallback failed: %d", code);
    } else {
        g_callback_mgr.clearCallback(m_output);
        if (callback != nullptr) {
            g_callback_mgr.addCallback(m_output, *this, callback);
        } else {
            auto err = OH_MetadataOutput_UnregisterCallback(m_output, &g_callbacks);
            _ERROR_RETURN_IF(err, err, "OH_MetadataOutput_UnregisterCallback failed: %d", err);
        }
    }
    if (callback) {
        _INFO("metadata output register callback(%p) success.", callback);
    } else {
        _INFO("metadata output unregister callback success.");
    }
    return CAMERA_OK;
}

CamErrorCode MetadataOutput::start() {
    _FATAL_IF(m_output == nullptr, "Camera_MetadataOutput is null while start()")

    CamErrorCode error = OH_MetadataOutput_Start(m_output);
    _ERROR_RETURN_IF(error, error, "OH_MetadataOutput_Start failed: %d", error)

    m_started = true;
    _INFO("metadata output start success.");
    return CAMERA_OK;
}

CamErrorCode MetadataOutput::stop() {
    _FATAL_IF(m_output == nullptr, "Camera_MetadataOutput is null while stop()")

    CamErrorCode error = OH_MetadataOutput_Stop(m_output);
    _ERROR_RETURN_IF(error, error, "OH_MetadataOutput_Stop failed: %d", error)

    m_started = false;
    _INFO("metadata output stop success.");
    return CAMERA_OK;
}

CamErrorCode MetadataOutput::release() {
    _WARN_RETURN_IF(m_output == nullptr, CAMERA_OK, "Camera_MetadataOutput already released.")

    if (m_started) {
        CamErrorCode error = OH_MetadataOutput_Stop(m_output);
        _INFO_IF(error, "OH_MetadataOutput_Stop failed: %d", error);
        m_started = false;
    }

    setCallback(nullptr);

    CamErrorCode error = OH_MetadataOutput_Release(m_output);
    m_output = nullptr;
    _WARN_RETURN_IF(error, error, "OH_MetadataOutput_Release failed: %d", error)

    _INFO("metadata output release successfully.");
    return CAMERA_OK;
}


NAMESPACE_END