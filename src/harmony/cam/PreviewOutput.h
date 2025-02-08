//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "ZNamespace.h"
#include "CamDevice.h"
#include "CamUtils.h"
#include "common/utils/CallbackMgr.h"
#include <ohcamera/preview_output.h>

NAMESPACE_DEFAULT

class PreviewOutput;
class CaptureSession;

class PreviewCallback {
public:
    virtual void onFrameStart(PreviewOutput &preview) {}

    virtual void onFrameEnd(PreviewOutput &preview, int frameCount) {}

    virtual void onError(PreviewOutput &preview, CamErrorCode errorCode) {}
};

// 封装 Camera_PreviewOutput
class PreviewOutput {
    friend class CaptureSession;

public:
    //static PreviewOutput *create(const CamProfile *profile);

public:
    PreviewOutput(Camera_PreviewOutput *p, const CamProfile *profile) : m_output(p), m_profile(*profile){};
    PreviewOutput(const PreviewOutput &other) : m_output(other.m_output), m_profile(other.m_profile){};

    ~PreviewOutput() { release(); }

public:
    /**
     * 设置回调函数
     * @param callback 如果为null 就是反注册
     * @return 错误码
     */
    CamErrorCode setCallback(PreviewCallback *callback);

    /**
     * 开始预览
     * @return 错误码
     */
    CamErrorCode start();
    
    /**
     * @return 是否已开始预览
     */
    inline bool isStarted() const { return m_started; }

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
    CamProfile m_profile;
    Camera_PreviewOutput *m_output;

    bool m_started = false;
};

NAMESPACE_END