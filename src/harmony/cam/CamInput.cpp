//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CamInput.h"
#include "common/utils/CallbackMgr.h"
#include "common/Log.h"

NAMESPACE_DEFAULT

static CallbackMgr<CamInput, CamInputCallback> g_callback_mgr;

static void onCamInputError(const Camera_Input *input, Camera_ErrorCode errCode) {
    auto callbacks = g_callback_mgr.findCallback(input);
    _WARN_RETURN_IF(callbacks == nullptr, void(), "onCamInputError: no callback for input %p", input)

    _INFO("onCamInputError: input %p, errCode %d", input, errCode);
    for (auto &cb : *callbacks) {
        cb.second->onError(cb.first, errCode);
    }
}

static CameraInput_Callbacks g_input_callbacks = {
    .onError = onCamInputError,
};

CamInput::CamInput(const CamDevice &device, Camera_Input *input) : m_device(device), m_input(input) {}

CamInput::~CamInput() { release(); }

CamErrorCode CamInput::registerCallback(CamInputCallback *callback) {
    _FATAL_IF(m_input == nullptr, "CamInput::registerCallback: camera_input is nullptr")

    if (!g_callback_mgr.hasAnyCallback(m_input)) {
        CamErrorCode error = OH_CameraInput_RegisterCallback(m_input, &g_input_callbacks);
        _ERROR_RETURN_IF(error, error, "CameraInput_RegisterCallback failed, error: %s", CamUtils::errString(error))
    }

    g_callback_mgr.addCallback(m_input, *this, callback);

    _INFO("CameraInput_RegisterCallback callback(%p) success", callback);
    return CAMERA_OK;
}

CamErrorCode CamInput::unregisterCallback(CamInputCallback *callback) {
    _FATAL_IF(m_input == nullptr, "CamInput::unregisterCallback: camera_input is nullptr");

    if (!g_callback_mgr.hasAnyCallback(m_input)) {
        _WARN("camera input no any callback, ignore unregister(%p)", callback);
        return CAMERA_OK;
    }

    g_callback_mgr.removeCallback(m_input, callback);
    if (!g_callback_mgr.hasAnyCallback(m_input)) {
        CamErrorCode error = OH_CameraInput_UnregisterCallback(m_input, &g_input_callbacks);
        _ERROR_RETURN_IF(error, error, "CamInput::unregisterCallback failed: %s, callback(%p)",
                         CamUtils::errString(error), callback)
    }

    _INFO("CameraInput_RegisterCallback callback(%p) success", callback);
    return CAMERA_OK;
}

CamErrorCode CamInput::open() {
    _FATAL_IF(m_input == nullptr, "CamInput::open: camera_input is nullptr");

    CamErrorCode code = OH_CameraInput_Open(m_input);
    if (code != CAMERA_OK) {
        _ERROR("CameraInput_Open failed, code %d", code);
    } else {
        m_opened = true;
        _INFO("CameraInput_Open success, input(%p)", m_input);
    }
    return code;
}

CamErrorCode CamInput::close() {
    _FATAL_IF(m_input == nullptr, "CamInput::close: camera_input is nullptr");

    CamErrorCode code = OH_CameraInput_Close(m_input);
    m_opened = false;
    if (code != CAMERA_OK) {
        _ERROR("CameraInput_Close failed, code %d", code);
    } else {
        _INFO("CameraInput_Close success, input(%p)", m_input);
    }
    return code;
}

CamErrorCode CamInput::release() {
    _WARN_RETURN_IF(m_input == nullptr, CAMERA_OK, "CamInput::release: camera_input already released");

    if (g_callback_mgr.hasAnyCallback(m_input)) {
        g_callback_mgr.clearCallback(m_input);

        OH_CameraInput_UnregisterCallback(m_input, &g_input_callbacks);
    }

    CamErrorCode code;
    if (m_opened) {
        code = OH_CameraInput_Close(m_input);
        if (code != CAMERA_OK) {
            _WARN("CameraInput_Close failed, code %s", CamUtils::errString(code));
        } else {
            _INFO("CameraInput_Close success, input(%p)", m_input);
        }
        m_opened = false;
    } else {
        // 注意 close 之后 release 会返回 CAMERA_SERVICE_FATAL_ERROR
        code = OH_CameraInput_Release(m_input);
        if (code != CAMERA_OK) {
            _WARN("CameraInput_Release failed, code %s", CamUtils::errString(code));
        } else {
            std::string devStr = m_device.toString();
            _INFO("CameraInput %s: release success,", devStr.c_str());
        }
    }
    m_input = nullptr;
    return code;
}

NAMESPACE_END