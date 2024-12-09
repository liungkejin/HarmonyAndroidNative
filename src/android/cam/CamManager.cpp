//
// Created by LiangKeJin on 2024/12/7.
//

#include "CamManager.h"

NAMESPACE_DEFAULT

bool CamManager::registerAvailabilityListener(znative::CamAvailabilityListener *listener) {
    _WARN_IF(listener == nullptr, "listener is nullptr")

    auto * cb = new ACameraManager_AvailabilityCallbacks {
        .context = listener,
        .onCameraAvailable = [](void *context, const char *id) {
            auto listener = static_cast<znative::CamAvailabilityListener *>(context);
            if (listener) {
                std::string idstr = id;
                listener->onCamAvailable(idstr);
            }
        },
        .onCameraUnavailable = [](void *context, const char *id) {
            auto listener = static_cast<znative::CamAvailabilityListener *>(context);
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
    this->m_callbacks.insert(std::make_pair(listener, cb));
    return true;
}

void CamManager::unregisterAvailabilityListener(znative::CamAvailabilityListener *listener) {
    auto it = m_callbacks.find(listener);
    if (it != m_callbacks.end()) {
        ACameraManager_unregisterAvailabilityCallback(m_mgr, it->second);
        delete it->second;
        m_callbacks.erase(it);
    }
}

NAMESPACE_END