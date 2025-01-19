//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "ZNamespace.h"
#include "CamDevice.h"
#include "CamUtils.h"
#include "CamInput.h"
#include "CaptureSession.h"
#include "PhotoOutput.h"

NAMESPACE_DEFAULT

typedef enum Camera_Status CamStatus;

class CamStatusInfo {
public:
    explicit CamStatusInfo(Camera_StatusInfo *info) : m_device(info->camera), m_status(info->status){};

public:
    CamDevice m_device;
    CamStatus m_status;
};

class CamStatusChangeListener {
public:
    virtual void onCamStatusChange(CamStatusInfo &info) {}
};

// 封装 Camera_Manager
class CamManager {
public:
    /**
     * 注册相机状态变化的监听
     */
    static void registerStatusChangeListener(CamStatusChangeListener *listener);
    
    /**
     * 反注册
     */
    static void unregisterStatusChangeListener(CamStatusChangeListener *listener);
    
    /**
     * 判断当前系统相机是否被禁用了
     * @return 输出相机是否被禁用
     */
    static bool isCameraMuted();
    
    /**
     * 判断相机是否正在被使用
     */
    static bool isCameraInUsing(const CamDevice &device);

    /**
     * 拿到所有支持的相机设备
     */
    static CamDeviceList getCamDeviceList();

    /**
     * @return 相机支持的输出特性, 可能为nullptr
     */
    static CamOutputCapability* getCameraOutputCapability(const CamDevice &d);

    /**
     * 创建一个相机输入
     * @param d 相机设备
     * @return 可能为 nullptr
     */
    static CamInput *createCameraInput(const CamDevice &d);

    /**
     * 创建一个capture任务
     */
    static CaptureSession *createCaptureSession();

    /**
     * 创建一个预览输出
     */
    static PreviewOutput *createPreviewOutput(const CamProfile *profile, const char *surfaceId);
    
    /**
     * 创建一个拍照输出
     */
    static PhotoOutput *createPhotoOutput(const CamProfile *profile, const char *surfaceId);
    
    /**
     * 创建一个拍照输出, 不需要 surface, 回调 PhotoAsset
     */
    static PhotoOutput *createPhotoOutputWithoutSurface(const CamProfile *profile);
    
    /**
     * 创建一个视频输出
     */
    static VideoOutput *createVideoOutput(const CamProfile *profile, const char *surfaceId);
    
    /**
     * 创建一个 Metadata 输出
     */
    static MetadataOutput *createMetadataOutput(Camera_MetadataObjectType type);

    /**
     * 释放 Camera_Manager
     */
    static void release();
};

NAMESPACE_END