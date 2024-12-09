//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <camera/NdkCameraManager.h>
#include "Namespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include "CamUtils.h"
#include "CamCharacteristics.h"

NAMESPACE_DEFAULT

class CamAvailabilityListener {
public:
    virtual void onCamAvailable(const std::string &id) {}
    virtual void onCamUnavailable(const std::string &id) {}
};

class CamManager {
public:
    CamManager() {
        m_mgr = ACameraManager_create();
        _FATAL_IF(m_mgr == nullptr, "ACameraManager_create failed")
    }

    ~CamManager() {
        if (m_mgr) {
            // release callback
            for (auto &pair : m_callbacks) {
                ACameraManager_unregisterAvailabilityCallback(m_mgr, pair.second);
                delete pair.second;
            }
            m_callbacks.clear();

            ACameraManager_delete(m_mgr);
            m_mgr = nullptr;
        }
    }

public:
    std::vector<std::string> getIdList() {
        std::vector<std::string> idVector;
        ACameraIdList *idList = nullptr;
        auto status = ACameraManager_getCameraIdList(m_mgr, &idList);
        _ERROR_RETURN_IF(status != ACAMERA_OK, idVector, "ACameraManager_getCameraIdList failed: %s", CamUtils::errString(status));

        if (idList) {
            for (int i = 0; i < idList->numCameras; ++i) {
                idVector.emplace_back(idList->cameraIds[i]);
            }
            ACameraManager_deleteCameraIdList(idList);
        }
        return idVector;
    }

    CamCharacteristics getCharacteristics(const std::string &id) {
        ACameraMetadata *chars = nullptr;
        auto status = ACameraManager_getCameraCharacteristics(m_mgr, id.c_str(), &chars);
        _ERROR_IF(status != ACAMERA_OK, "ACameraManager_getCameraCharacteristics failed: %s", CamUtils::errString(status));
        return CamCharacteristics(chars);
    }

    bool registerAvailabilityListener(CamAvailabilityListener *listener);

    void unregisterAvailabilityListener(CamAvailabilityListener *listener);

private:
    ACameraManager *m_mgr;
    std::unordered_map<CamAvailabilityListener *, ACameraManager_AvailabilityCallbacks*> m_callbacks;
};

NAMESPACE_END
