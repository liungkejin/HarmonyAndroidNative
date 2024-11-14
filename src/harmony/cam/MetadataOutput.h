//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "CamDevice.h"
#include <cstdint>
#include <ohcamera/camera.h>
#include <ohcamera/metadata_output.h>

NAMESPACE_DEFAULT

class MetadataOutput;
class CaptureSession;

class MetadataCallback {
public:
    virtual void onAvailable(MetadataOutput &video, Camera_MetadataObject *obj, uint32_t size) {}

    virtual void onError(MetadataOutput &video, Camera_ErrorCode err) {}
};

// 封装 Camera_PreviewOutput
class MetadataOutput {
    friend class CaptureSession;

public:
    MetadataOutput(Camera_MetadataOutput *p, const Camera_MetadataObjectType type) : m_output(p), m_type(type){};
    MetadataOutput(const MetadataOutput &other) : m_output(other.m_output), m_type(other.m_type){};

    ~MetadataOutput() { release(); }

public:
    
    inline Camera_MetadataObjectType type() const { return m_type; }
    
    /**
     * 设置回调函数
     * @param callback 如果为null,就表示反注册
     * @return 错误码
     */
    CamErrorCode setCallback(MetadataCallback *callback);

    /**
     * 开始预览
     * @return 错误码
     */
    CamErrorCode start();

    /**
     * 停止预览
     * @return 错误码
     */
    CamErrorCode stop();

private:
    /**
     * 释放预览资源，释放后所有调用都会抛出异常
     * @return 错误码
     */
    CamErrorCode release();

private:
    Camera_MetadataObjectType m_type;
    Camera_MetadataOutput *m_output;

    bool m_started = false;
};
NAMESPACE_END
