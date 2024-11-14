//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CaptureSession.h"
#include "CamManager.h"
#include "common/utils/CallbackMgr.h"

NAMESPACE_DEFAULT

static CallbackMgr<CaptureSession, SessionListener> g_callback_mgr;

static void onSessionFocusStateChange(Camera_CaptureSession *session, Camera_FocusState focusState) {
    _INFO("onSessionFocusStateChange: state: %s", CamUtils::focusStateStr(focusState));

    auto callbacks = g_callback_mgr.findCallback(session);
    if (callbacks == nullptr) {
        _WARN("onSessionFocusStateChange session(%p): no callback found", session);
        return;
    }
    for (auto &it : *callbacks) {
        it.second->onSessionFocusStateChange(it.first, focusState);
    }
}

static void onSessionError(Camera_CaptureSession *session, Camera_ErrorCode error) {
    _WARN("onSessionError: %s", CamUtils::errString(error));

    auto callbacks = g_callback_mgr.findCallback(session);
    if (callbacks == nullptr) {
        _WARN("onSessionError session(%p): no callback found", session);
        return;
    }
    for (auto &it : *callbacks) {
        it.second->onSessionError(it.first, error);
    }
}

static CaptureSession_Callbacks g_captureSessionCallbacks = {.onFocusStateChange = onSessionFocusStateChange,
                                                             .onError = onSessionError};

CaptureSession::CaptureSession(Camera_CaptureSession *s) : m_session(s) {}

CaptureSession::~CaptureSession() { release(); }

CamErrorCode CaptureSession::registerListener(SessionListener *listener) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::registerCallback failed: m_session == nullptr")

    if (!g_callback_mgr.hasAnyCallback(m_session)) {
        CamErrorCode error = OH_CaptureSession_RegisterCallback(m_session, &g_captureSessionCallbacks);
        _ERROR_RETURN_IF(error, error, "CaptureSession::registerCallback failed: %s", CamUtils::errString(error))
    }

    g_callback_mgr.addCallback(m_session, *this, listener);

    _INFO("capture session register callback(%p) success", listener);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::unregisterListener(SessionListener *listener) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::unregisterCallback failed: m_session == nullptr")

    if (!g_callback_mgr.hasAnyCallback(m_session)) {
        _WARN("capture session has no callback, ignore unregister(%p)", listener);
        return CAMERA_OK;
    }
    g_callback_mgr.removeCallback(m_session, listener);
    if (!g_callback_mgr.hasAnyCallback(m_session)) {
        CamErrorCode error = OH_CaptureSession_UnregisterCallback(m_session, &g_captureSessionCallbacks);
        _ERROR_RETURN_IF(error, error, "CaptureSession::unregisterCallback failed: %s",
                         CamUtils::errString(error))
    }
    _INFO("capture session unregister callback(%p) success", listener);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::beginConfig() {
    _FATAL_IF(m_session == nullptr, "CaptureSession::beginConfig() failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_BeginConfig(m_session);
    _ERROR_RETURN_IF(error, error, "CaptureSession::beginConfig() failed: %s", CamUtils::errString(error))

    _INFO("CaptureSession::beginConfig()");
    return CAMERA_OK;
}

CamErrorCode CaptureSession::commitConfig() {
    _FATAL_IF(m_session == nullptr, "CaptureSession::commitConfig() failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_CommitConfig(m_session);
    _ERROR_RETURN_IF(error, error, "CaptureSession::commitConfig() failed: %s", CamUtils::errString(error))

    _INFO("CaptureSession::commitConfig()");
    return CAMERA_OK;
}

CamErrorCode CaptureSession::config(const ConfigRunnable &run) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::config() failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_BeginConfig(m_session);
    _ERROR_RETURN_IF(error, error, "CaptureSession::config() begin config failed: %s", CamUtils::errString(error))

    error = run(*this);
    _ERROR_RETURN_IF(error, error, "CaptureSession::config() failed: %s", CamUtils::errString(error))

    error = OH_CaptureSession_CommitConfig(m_session);
    _ERROR_RETURN_IF(error, error, "CaptureSession::config() commit config failed: %s",
                     CamUtils::errString(error))

    return error;
}

CamInput *CaptureSession::addInput(const CamDevice &device) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addInput failed: m_session == nullptr")

    for (auto &i : m_inputs) {
        if (strcmp(i->device().id(), device.id()) == 0) {
            _WARN("capture session(%p) already has input(%p) camera id: %s", m_session, i, i->device().id());
            return i;
        }
    }

    CamInput *input = CamManager::createCameraInput(device);
    _ERROR_RETURN_IF(input == nullptr, nullptr, "CaptureSession::addInput failed: input == nullptr")
    CamErrorCode code = input->open();
    if (code != CAMERA_OK) {
        delete input;
        _ERROR("capture session(%p) add input(%p) open failed: %d", m_session, input, code);
        return nullptr;
    }

    code = OH_CaptureSession_AddInput(m_session, input->m_input);
    if (code != CAMERA_OK) {
        delete input;
        _ERROR("capture session(%p) add input(%p) failed: %d", m_session, input, code);
        return nullptr;
    } else {
        _INFO("capture session(%p) add input(%p) success", m_session, input);
        m_inputs.push_back(input);
        return input;
    }
}

CamErrorCode CaptureSession::removeInput(const CamDevice &device) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::removeInput failed: m_session == nullptr")

    CamErrorCode errCode = WT_CAM_ERROR_UNKNOWN;
    for (auto it = m_inputs.begin(); it != m_inputs.end(); ++it) {
        if (strcmp((*it)->device().id(), device.id()) == 0) {
            errCode = OH_CaptureSession_RemoveInput(m_session, (*it)->m_input);

            m_inputs.erase(it);
            break;
        }
    }

    if (errCode == WT_CAM_ERROR_UNKNOWN) {
        // not found
        _WARN("capture session(%p) remove input camera id(%s) failed: not found!", m_session, device.id());
        return WT_CAM_ERROR_UNKNOWN;
    } else if (errCode != CAMERA_OK) {
        _ERROR("capture session(%p) remove input camera id(%s) failed: %d", m_session, device.id(), errCode);
    } else {
        _INFO("capture session(%p) remove input camera id(%s) success", m_session, device.id());
    }

    return errCode;
}

CamErrorCode CaptureSession::removeInput(const CamInput *input) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::removeInput failed: m_session == nullptr")

    CamErrorCode errCode = WT_CAM_ERROR_UNKNOWN;
    for (auto it = m_inputs.begin(); it != m_inputs.end(); ++it) {
        if (*it == input) {
            errCode = OH_CaptureSession_RemoveInput(m_session, (*it)->m_input);

            m_inputs.erase(it);
            break;
        }
    }

    const char *id = input->m_device.id();
    if (errCode == WT_CAM_ERROR_UNKNOWN) {
        // not found
        _WARN("capture session(%p) remove input camera id(%s) failed: not found!", m_session, id);
        return WT_CAM_ERROR_UNKNOWN;
    } else if (errCode != CAMERA_OK) {
        _ERROR("capture session(%p) remove input camera id(%s) failed: %d", m_session, id, errCode);
    } else {
        _INFO("capture session(%p) remove input camera id(%s) success", m_session, id);
    }

    return errCode;
}

PreviewOutput *CaptureSession::addPreviewOutput(const CamProfile *profile, const char *surfaceId) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addPreviewOutput failed: m_session == nullptr");

    PreviewOutput *previewOutput = CamManager::createPreviewOutput(profile, surfaceId);
    _ERROR_RETURN_IF(previewOutput == nullptr, nullptr,
                     "CaptureSession::addPreviewOutput failed: previewOutput == nullptr");

    addPreviewOutput(previewOutput);
    return previewOutput;
}

CamErrorCode CaptureSession::addPreviewOutput(PreviewOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addPreviewOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_AddPreviewOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) add preview output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error));
    m_preview_outputs.push_back(output);
    _INFO("capture session(%p) add preview output(%p)", m_session, output);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::removePreviewOutput(const PreviewOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::removePreviewOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_RemovePreviewOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session (%p) remove preview output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))

    auto it = std::find(m_preview_outputs.begin(), m_preview_outputs.end(), output);
    if (it != m_preview_outputs.end()) {
        m_preview_outputs.erase(it);
    }
    _INFO("capture session remove preview output(%p)", output);
    return CAMERA_OK;
}

PhotoOutput *CaptureSession::addPhotoOutput(const CamProfile *profile, const char *surfaceId) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addPhotoOutput failed: m_session == nullptr")

    PhotoOutput *photoOutput = CamManager::createPhotoOutput(profile, surfaceId);
    _ERROR_RETURN_IF(photoOutput == nullptr, nullptr, "CaptureSession::addPhotoOutput failed: photoOutput == nullptr")

    addPhotoOutput(photoOutput);
    return photoOutput;
}

PhotoOutput *CaptureSession::addPhotoOutputWithoutSurface(const CamProfile *profile) {
    _FATAL_IF(m_session == nullptr, "addPhotoOutputWithoutSurface failed: m_session == nullptr")

    PhotoOutput *photoOutput = CamManager::createPhotoOutputWithoutSurface(profile);
    _ERROR_RETURN_IF(photoOutput == nullptr, nullptr, "addPhotoOutputWithoutSurface failed: photoOutput == nullptr")

    addPhotoOutput(photoOutput);
    return photoOutput;
}


CamErrorCode CaptureSession::addPhotoOutput(PhotoOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addPhotoOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_AddPhotoOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) add photo output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))
    m_photo_outputs.push_back(output);
    _INFO("capture session(%p) add photo output(%p)", m_session, output);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::removePhotoOutput(const PhotoOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::removePhotoOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_RemovePhotoOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) remove photo output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))

    auto it = std::find(m_photo_outputs.begin(), m_photo_outputs.end(), output);
    if (it != m_photo_outputs.end()) {
        m_photo_outputs.erase(it);
    }
    _INFO("capture session remove photo output(%p)", output);
    return CAMERA_OK;
}

VideoOutput *CaptureSession::addVideoOutput(const CamProfile *profile, const char *surfaceId) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addVideoOutput failed: m_session == nullptr")

    VideoOutput *videoOutput = CamManager::createVideoOutput(profile, surfaceId);
    _ERROR_RETURN_IF(videoOutput == nullptr, nullptr, "CaptureSession::addVideoOutput failed: videoOutput == nullptr")

    addVideoOutput(videoOutput);
    return videoOutput;
}

CamErrorCode CaptureSession::addVideoOutput(VideoOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addVideoOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_AddVideoOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) add video output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))
    m_video_outputs.push_back(output);
    _INFO("capture session(%p) add video output(%p)", m_session, output);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::removeVideoOutput(const VideoOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::removeVideoOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_RemoveVideoOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) remove video output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))

    auto it = std::find(m_video_outputs.begin(), m_video_outputs.end(), output);
    if (it != m_video_outputs.end()) {
        m_video_outputs.erase(it);
    }
    _INFO("capture session remove video output(%p)", output);
    return CAMERA_OK;
}

MetadataOutput *CaptureSession::addMetadataOutput(const Camera_MetadataObjectType type) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addMetadataOutput failed: m_session == nullptr")

    MetadataOutput *output = CamManager::createMetadataOutput(type);
    _ERROR_RETURN_IF(output == nullptr, nullptr, "CaptureSession::addMetadataOutput failed: output == nullptr")

    addMetadataOutput(output);
    return output;
}

CamErrorCode CaptureSession::addMetadataOutput(MetadataOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::addMetadataOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_AddMetadataOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) add metadata output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))
    m_metadata_outputs.push_back(output);
    _INFO("capture session(%p) add metadata output(%p)", m_session, output);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::removeMetadataOutput(const MetadataOutput *output) {
    _FATAL_IF(m_session == nullptr, "CaptureSession::removeMetadataOutput failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_RemoveMetadataOutput(m_session, output->m_output);
    _ERROR_RETURN_IF(error, error, "capture session(%p) remove metadata output(%p) failed: %s", m_session, output,
                     CamUtils::errString(error))

    auto it = std::find(m_metadata_outputs.begin(), m_metadata_outputs.end(), output);
    if (it != m_metadata_outputs.end()) {
        m_metadata_outputs.erase(it);
    }
    _INFO("capture session remove metadata output(%p)", output);
    return CAMERA_OK;
}

CamErrorCode CaptureSession::start() {
    _FATAL_IF(m_session == nullptr, "CaptureSession::start failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_Start(m_session);
    _ERROR_RETURN_IF(error, error, "capture session start failed: %s", CamUtils::errString(error))

    m_started = true;
    _INFO("capture session start success");
    return CAMERA_OK;
}

CamErrorCode CaptureSession::stop() {
    _FATAL_IF(m_session == nullptr, "CaptureSession::stop failed: m_session == nullptr")

    CamErrorCode error = OH_CaptureSession_Stop(m_session);
    _ERROR_RETURN_IF(error, error, "capture session stop failed: %s", CamUtils::errString(error))

    m_started = false;
    _INFO("capture session stop success");
    return CAMERA_OK;
}

CamErrorCode CaptureSession::release() {
    if (m_session == nullptr) {
        _WARN("capture session already released");
        return CAMERA_OK;
    }
    if (m_started) {
        CamErrorCode error = OH_CaptureSession_Stop(m_session);
        _WARN_IF(error, "capture session stop failed: %s", CamUtils::errString(error));
        m_started = false;
    }

    if (g_callback_mgr.hasAnyCallback(m_session)) {
        g_callback_mgr.clearCallback(m_session);
//        OH_CaptureSession_UnregisterCallback(m_session, &g_captureSessionCallbacks);
    }

//    for (auto &input : m_inputs) {
//        OH_CaptureSession_RemoveInput(m_session, input->m_input);
//    }
    m_inputs.clear();

//    for (auto &out : m_preview_outputs) {
//        OH_CaptureSession_RemovePreviewOutput(m_session, out->m_output);
//    }
    m_preview_outputs.clear();

//    for (auto &out : m_photo_outputs) {
//        OH_CaptureSession_RemovePhotoOutput(m_session, out->m_output);
//    }
    m_photo_outputs.clear();

//    for (auto &out : m_video_outputs) {
//        OH_CaptureSession_RemoveVideoOutput(m_session, out->m_output);
//    }
    m_video_outputs.clear();

//    for (auto &out : m_metadata_outputs) {
//        OH_CaptureSession_RemoveMetadataOutput(m_session, out->m_output);
//    }
    m_metadata_outputs.clear();

    CamErrorCode error = OH_CaptureSession_Release(m_session);
    m_session = nullptr;

    _WARN_RETURN_IF(error, error, "capture session release failed: %s", CamUtils::errString(error))

    _INFO("capture session release success");
    return CAMERA_OK;
}

NAMESPACE_END