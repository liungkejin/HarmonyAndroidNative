//
// Created by LiangKeJin on 2024/12/7.
//

#include "CamManager.h"

NAMESPACE_DEFAULT

static CamManager *g_instance = nullptr;

CamManager& CamManager::instance() {
    if (g_instance == nullptr) {
        g_instance = new CamManager();
    }
    return *g_instance;
}

bool CamManager::registerAvailabilityListener(CamAvailabilityListener *listener) {
    _WARN_IF(listener == nullptr, "listener is nullptr")
    if (m_av_callbacks.find(listener) != m_av_callbacks.end()) {
        _WARN("CamAvailabilityListener(%p) already registered", listener);
        return true;
    }

    auto * cb = new ACameraManager_AvailabilityCallbacks {
        .context = listener,
        .onCameraAvailable = [](void *context, const char *id) {
            auto listener = static_cast<CamAvailabilityListener *>(context);
            if (listener) {
                std::string idstr = id;
                listener->onCamAvailable(idstr);
            }
        },
        .onCameraUnavailable = [](void *context, const char *id) {
            auto listener = static_cast<CamAvailabilityListener *>(context);
            if (listener) {
                std::string idstr = id;
                listener->onCamUnavailable(idstr);
            }
        }
    };
    auto status = ACameraManager_registerAvailabilityCallback(m_mgr, cb);
    if (status != ACAMERA_OK) {
        delete cb;
        _ERROR("ACameraManager_registerAvailabilityCallback failed: %s", CamUtils::errString(status));
        return false;
    }
    this->m_av_callbacks.insert(std::make_pair(listener, cb));
    _INFO("CamAvailabilityListener(%p) registered", listener);
    return true;
}

void CamManager::unregisterAvailabilityListener(CamAvailabilityListener *listener) {
    auto it = m_av_callbacks.find(listener);
    if (it != m_av_callbacks.end()) {
        ACameraManager_unregisterAvailabilityCallback(m_mgr, it->second);
        delete it->second;
        m_av_callbacks.erase(it);
        _INFO("CamAvailabilityListener(%p) unregistered", listener);
    }
}

CamDevice * CamManager::openCamera(const std::string& id, CamDeviceStateCallbacks *callback) {
    ACameraDevice *device = nullptr;
    auto *cb = new ACameraDevice_StateCallbacks {
        .context = callback,
        .onDisconnected = [](void *context, ACameraDevice *device) {
            auto listener = static_cast<CamDeviceStateCallbacks *>(context);
            if (listener) {
                listener->onDisconnected(device);
            }
        },
        .onError = [](void *context, ACameraDevice *device, int error) {
            auto listener = static_cast<CamDeviceStateCallbacks *>(context);
            if (listener) {
                listener->onError(device, error);
            }
        }
    };

    auto status = ACameraManager_openCamera(m_mgr, id.c_str(), cb, &device);
    if (status) {
        delete cb;
        callback->onError(nullptr, status);
        _ERROR("ACameraManager_openCamera failed: %s", CamUtils::errString(status));
        return nullptr;
    }

    _INFO("CamDevice(%s) opened", id.c_str());
    return new CamDevice(device, cb);
}

NAMESPACE_END