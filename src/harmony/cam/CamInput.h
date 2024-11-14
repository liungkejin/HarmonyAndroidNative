//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "Namespace.h"
#include "CamDevice.h"

NAMESPACE_DEFAULT

class CamInput;
class CaptureSession;

class CamInputCallback {
public:
    virtual void onError(CamInput &camInput, CamErrorCode errorCode) = 0;
};

// 封装 Camera_Input
class CamInput {
    friend class CaptureSession;

public:
    CamInput(const CamDevice &device, Camera_Input *input);
    ~CamInput();

public:
    inline const CamDevice device() const { return m_device; }

    /**
     * 注册 onError 回调
     */
    CamErrorCode registerCallback(CamInputCallback *callback);
    /**
     * 注销回调
     */
    CamErrorCode unregisterCallback(CamInputCallback *callback);

    /**
     * 打开相机输入
     */
    CamErrorCode open();

    /**
     * 关闭
     */
    CamErrorCode close();

private:
    /**
     * 释放
     */
    CamErrorCode release();

private:
    CamDevice m_device;
    Camera_Input *m_input;

    bool m_opened = false;
};

NAMESPACE_END