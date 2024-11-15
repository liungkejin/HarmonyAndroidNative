//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CamManager.h"
#include <set>

NAMESPACE_DEFAULT

static std::mutex g_list_mutex;
static std::vector<CamStatusChangeListener *> g_listeners;
// 目前正在使用的相机
static std::set<CamDevice> g_in_using_cameras;

static void onCameraStatusChange(Camera_Manager *mgr, Camera_StatusInfo *info) {
    CamStatusInfo status(info);

    const CamDevice &dev = status.m_device;
    if (status.m_status == CAMERA_STATUS_UNAVAILABLE) {
        g_in_using_cameras.insert(dev);
    } else {
        g_in_using_cameras.erase(dev);
    }
    
    const std::string devInfo = dev.toString();
    std::string statusStr = CamUtils::statusStr(status.m_status);
    _INFO("CamMgr: on camera(%s) status changed: %s, in using cameras: %d", devInfo.c_str(), statusStr, g_in_using_cameras.size());
    
    std::lock_guard<std::mutex> lock(g_list_mutex);
    for (auto &cb : g_listeners) {
        cb->onCamStatusChange(status);
    }
}

static CameraManager_Callbacks g_callbacks = {
    .onCameraStatus = onCameraStatusChange,
};

static Camera_Manager *__camera_mgr = nullptr;
static Camera_Manager *getCamMgr() {
    if (__camera_mgr == nullptr) {
        CamErrorCode error = OH_Camera_GetCameraManager(&__camera_mgr);
        _FATAL_IF(error, "Failed to get camera manager, error code: %d", error);
        error = OH_CameraManager_RegisterCallback(__camera_mgr, &g_callbacks);
        _ERROR_RETURN_IF(error, nullptr,
                         "CamManager::registerStatusChangeListener error: %s", CamUtils::errString(error));
    }
    return __camera_mgr;
}

void CamManager::registerStatusChangeListener(CamStatusChangeListener *listener) {
    std::lock_guard<std::mutex> lock(g_list_mutex);
    for (auto & l : g_listeners) {
        if (l == listener) {
            return;
        }
    }
    g_listeners.push_back(listener);
    _INFO("registerStatusChangeListener, size: %d", g_listeners.size());
}

void CamManager::unregisterStatusChangeListener(CamStatusChangeListener *listener) {
    _INFO("unregisterStatusChangeListener");
    std::lock_guard<std::mutex> lock(g_list_mutex);
    g_listeners.erase(std::remove(g_listeners.begin(), g_listeners.end(), listener), g_listeners.end());
}

bool CamManager::isCameraMuted() { 
    bool muted = false;
    CamErrorCode error = OH_CameraManager_IsCameraMuted(getCamMgr(), &muted);
    _ERROR_IF(error, "OH_CameraManager_IsCameraMuted failed: %s", CamUtils::errString(error))
    return muted;
}

bool CamManager::isCameraInUsing(const CamDevice& device) {
    return g_in_using_cameras.find(device) != g_in_using_cameras.end();
}

CamDeviceList CamManager::getCamDeviceList() {
    CamDeviceList devices;

    Camera_Device *camDevices = nullptr;
    uint32_t sumCamDevSize = 0;
    CamErrorCode error = OH_CameraManager_GetSupportedCameras(getCamMgr(), &camDevices, &sumCamDevSize);
    _ERROR_RETURN_IF(error, devices, "Failed to get supported cameras, error: %s", CamUtils::errString(error))

    for (int i = 0; i < sumCamDevSize; i++) {
        devices.add(camDevices + i);
    }
    error = OH_CameraManager_DeleteSupportedCameras(getCamMgr(), camDevices, sumCamDevSize);
    _WARN_IF(error, "Failed to delete supported cameras, error: %s", CamUtils::errString(error))

    return devices;
}

CamOutputCapability* CamManager::getCameraOutputCapability(const CamDevice &d) {
    Camera_Device dev = {.cameraId = d.id(),
                         .cameraPosition = d.position(),
                         .cameraType = d.type(),
                         .connectionType = d.connectionType()};

    Camera_OutputCapability *cap = nullptr;
    CamErrorCode error = OH_CameraManager_GetSupportedCameraOutputCapability(getCamMgr(), &dev, &cap);
    _ERROR_RETURN_IF(error || !cap, nullptr, "Failed to get camera output capability, error: %ds", CamUtils::errString(error))

    auto *camCap = new CamOutputCapability(cap);
    // release
    error = OH_CameraManager_DeleteSupportedCameraOutputCapability(getCamMgr(), cap);
    _WARN_IF(error, "Failed to delete camera output capability, error: %s", CamUtils::errString(error));

    return camCap;
}

CamInput *CamManager::createCameraInput(const CamDevice &d) {
    Camera_Input *input = nullptr;
    Camera_Device dev = {.cameraId = d.id(),
                         .cameraPosition = d.position(),
                         .cameraType = d.type(),
                         .connectionType = d.connectionType()};
    CamErrorCode error = OH_CameraManager_CreateCameraInput(getCamMgr(), &dev, &input);
    _ERROR_RETURN_IF(error || !input, nullptr, "Failed to create camera input, error: %s", CamUtils::errString(error))

    return new CamInput(d, input);
}

CaptureSession *CamManager::createCaptureSession() {
    Camera_CaptureSession *session = nullptr;
    CamErrorCode error = OH_CameraManager_CreateCaptureSession(getCamMgr(), &session);
    _ERROR_RETURN_IF(error || !session, nullptr, "Failed to create capture session, error: %s", CamUtils::errString(error))

    return new CaptureSession(session);
}

PreviewOutput *CamManager::createPreviewOutput(const CamProfile *profile, const char *surfaceId) {
    Camera_PreviewOutput *output = nullptr;
    Camera_Profile prof = {
        .format = profile->format,
        .size = Camera_Size {
            .width = profile->width,
            .height = profile->height,
        }
    };
    CamErrorCode error = OH_CameraManager_CreatePreviewOutput(getCamMgr(), &prof, surfaceId, &output);
    _ERROR_RETURN_IF(error || !output, nullptr, "Failed to create preview output, error: %s", CamUtils::errString(error))

    return new PreviewOutput(output, profile);
}

PhotoOutput *CamManager::createPhotoOutput(const CamProfile *profile, const char *surfaceId) {
    Camera_PhotoOutput *output = nullptr;
    Camera_Profile prof = {
        .format = profile->format,
        .size = Camera_Size {
            .width = profile->width,
            .height = profile->height,
        }
    };
    CamErrorCode error = OH_CameraManager_CreatePhotoOutput(getCamMgr(), &prof, surfaceId, &output);
    _ERROR_RETURN_IF(error || !output, nullptr, "CreatePhotoOutput, error: %s", CamUtils::errString(error))

    return new PhotoOutput(output, profile);
}

PhotoOutput *CamManager::createPhotoOutputWithoutSurface(const CamProfile *profile) {
    Camera_PhotoOutput *output = nullptr;
    Camera_Profile prof = {
        .format = profile->format,
        .size = Camera_Size {
            .width = profile->width,
            .height = profile->height,
        }
    };
    CamErrorCode error = OH_CameraManager_CreatePhotoOutputWithoutSurface(getCamMgr(), &prof, &output);
    _ERROR_RETURN_IF(error || !output, nullptr, "CreatePhotoOutputWithoutSurface, error: %s", CamUtils::errString(error))

    return new PhotoOutput(output, profile);
}

VideoOutput *CamManager::createVideoOutput(const CamProfile *profile, const char *surfaceId) {
    Camera_VideoOutput *output = nullptr;
    Camera_VideoProfile prof = {
        .format = profile->format,
        .size = Camera_Size {
            .width = profile->width,
            .height = profile->height,
        },
        .range = Camera_FrameRateRange {
            .min = profile->minFps,
            .max = profile->maxFps,
        }
    };
    CamErrorCode error = OH_CameraManager_CreateVideoOutput(getCamMgr(), &prof, surfaceId, &output);
    _ERROR_RETURN_IF(error || !output, nullptr, "Failed to create video output, error: %s", CamUtils::errString(error))

    return new VideoOutput(output, profile);
}

MetadataOutput *CamManager::createMetadataOutput(Camera_MetadataObjectType type) {
    Camera_MetadataOutput *output = nullptr;
    CamErrorCode error = OH_CameraManager_CreateMetadataOutput(getCamMgr(), &type, &output);
    _ERROR_RETURN_IF(error || !output, nullptr, "Failed to create metadata output, error: %s", CamUtils::errString(error))

    return new MetadataOutput(output, type);
}

void CamManager::release() {
    if (__camera_mgr != nullptr) {
        CamErrorCode error = OH_CameraManager_UnregisterCallback(__camera_mgr, &g_callbacks);
        _WARN_IF(error, "Failed to unregister callback, error: %s", CamUtils::errString(error))
        error = OH_Camera_DeleteCameraManager(__camera_mgr);
        _WARN_IF(error, "Failed to release camera manager, error: %s", CamUtils::errString(error))

        __camera_mgr = nullptr;
    }
}

NAMESPACE_END