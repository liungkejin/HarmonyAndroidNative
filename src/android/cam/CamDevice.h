//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <camera/NdkCameraDevice.h>
#include "Namespace.h"
#include "common/Object.h"
#include "CaptureRequest.h"
#include "CaptureSession.h"


NAMESPACE_DEFAULT

class CamDevice;

class CamDeviceStateCallbacks {
public:
    virtual void onDisconnected(ACameraDevice *device) {}

    virtual void onError(ACameraDevice *device, int error) {}
};

class CamDevice : Object {
public:
    explicit CamDevice(ACameraDevice *device, bool own = false)
            : m_device(device), m_stateListener(nullptr), m_owned(own) {}

    CamDevice(ACameraDevice *device, ACameraDevice_StateCallbacks *listener)
            : m_device(device), m_stateListener(listener), m_owned(true) {}

    CamDevice(const CamDevice &o) : m_device(o.m_device),
                                    m_stateListener(o.m_stateListener), m_owned(o.m_owned), Object(o) {}

    ~CamDevice() {
        if (m_owned && no_reference() && m_device) {
            ACameraDevice_close(m_device);
            m_device = nullptr;

            delete m_stateListener;
            m_stateListener = nullptr;
        }
    }

public:
    const char *id() {
        return ACameraDevice_getId(m_device);
    }

    /**
     * 创建一个请求
     * @param templateType 模板类型
     * @return CaptureRequest
     */
    CaptureRequest createCaptureRequest(CamTemplate templateType) {
        ACaptureRequest *request = nullptr;
        auto ret = ACameraDevice_createCaptureRequest(m_device, templateType, &request);
        _FATAL_IF(ret, "ACameraDevice_createCaptureRequest failed: %s", CamUtils::errString(ret));
        return CaptureRequest(request);
    }

    CaptureSession createCaptureSession(CaptureSessionOutputContainer &container, SessionStateListener *stateListener);

private:
    ACameraDevice *m_device = nullptr;
    bool m_owned = true;

    ACameraDevice_StateCallbacks *m_stateListener = nullptr;
};

NAMESPACE_END
