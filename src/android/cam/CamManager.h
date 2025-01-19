//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <camera/NdkCameraManager.h>
#include "ZNamespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include "CamUtils.h"
#include "CamCharacteristics.h"
#include "CamDevice.h"

NAMESPACE_DEFAULT

#define CAM_MANAGER CamManager::instance()

class CamAvailabilityListener {
public:
    virtual void onCamAvailable(const std::string &id) {}

    virtual void onCamUnavailable(const std::string &id) {}
};

class CamManager {
public:
    static CamManager& instance();

public:
    CamManager() {
        m_mgr = ACameraManager_create();
        _FATAL_IF(m_mgr == nullptr, "ACameraManager_create failed")
    }

    ~CamManager() {
        if (m_mgr) {
            // release callback
            for (auto &pair: m_av_callbacks) {
                ACameraManager_unregisterAvailabilityCallback(m_mgr, pair.second);
                delete pair.second;
            }
            m_av_callbacks.clear();

            ACameraManager_delete(m_mgr);
            m_mgr = nullptr;
        }
    }

public:
    /**
     * 获取所有相机 id
     * @return 相机 id 列表
     */
    std::vector<std::string> getIdList() {
        std::vector<std::string> idVector;
        ACameraIdList *idList = nullptr;
        auto error = ACameraManager_getCameraIdList(m_mgr, &idList);
        _ERROR_RETURN_IF(error, idVector, "ACameraManager_getCameraIdList failed: %s",CamUtils::errString(error));

        if (idList) {
            for (int i = 0; i < idList->numCameras; ++i) {
                idVector.emplace_back(idList->cameraIds[i]);
            }
            ACameraManager_deleteCameraIdList(idList);
        }
        return idVector;
    }

    /**
     * 获取相机特性
     * @param id 相机 id
     * @return 相机特性
     */
    CamCharacteristics getCharacteristics(const std::string &id) {
        ACameraMetadata *chars = nullptr;
        auto status = ACameraManager_getCameraCharacteristics(m_mgr, id.c_str(), &chars);
        _ERROR_IF(status, "ACameraManager_getCameraCharacteristics failed: %s", CamUtils::errString(status));
        return CamCharacteristics(chars);
    }

    /**
     * 注册相机可用性监听器
     * @param listener 监听器
     * @return 注册成功返回 true
     */
    bool registerAvailabilityListener(CamAvailabilityListener *listener);

    /**
     * 注销相机可用性监听器
     * @param listener 监听器
     */
    void unregisterAvailabilityListener(CamAvailabilityListener *listener);

    /**
     * 打开相机
     * @param id 相机 id
     * @param callback onError 和 onDisconnected 的回调
     * @return 打开失败返回 nullptr, 失败状态会通过 callback的 onError 通知
     */
    CamDevice * openCamera(const std::string& id, CamDeviceStateCallbacks *callback);

private:
    ACameraManager *m_mgr;

    std::unordered_map<CamAvailabilityListener *, ACameraManager_AvailabilityCallbacks *> m_av_callbacks;
};

NAMESPACE_END
