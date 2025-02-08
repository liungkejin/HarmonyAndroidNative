//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "CamDevice.h"
#include <ohcamera/camera.h>
#include <ohcamera/video_output.h>

NAMESPACE_DEFAULT

class VideoOutput;
class CaptureSession;

class VideoCallback {
public:
    virtual void onFrameStart(VideoOutput &video) {}

    virtual void onFrameEnd(VideoOutput &video, int frameCount) {}

    virtual void onError(VideoOutput &video, Camera_ErrorCode err) {}
};

// 封装 Camera_PreviewOutput
class VideoOutput {
    friend class CaptureSession;

public:
    VideoOutput(Camera_VideoOutput *p, const CamProfile *profile) : m_output(p), m_profile(*profile){};
    VideoOutput(const VideoOutput &other) : m_output(other.m_output), m_profile(other.m_profile){};

    ~VideoOutput() { release(); }

public:
    /**
     * 设置回调函数
     * @param callback 如果为null,就表示反注册
     * @return 错误码
     */
    CamErrorCode setCallback(VideoCallback *callback);

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
    CamProfile m_profile;
    Camera_VideoOutput *m_output;

    bool m_started = false;
};

NAMESPACE_END
