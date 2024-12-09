//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <camera/NdkCameraDevice.h>
#include "Namespace.h"
#include "common/Object.h"


NAMESPACE_DEFAULT

typedef ACameraDevice_request_template CamTemplate;

class CamRequest : Object {
public:
    explicit CamRequest(ACaptureRequest *request) : m_request(request) {}
    CamRequest(const CamRequest &o) : m_request(o.m_request), Object(o) {}
    ~CamRequest() {
        if (no_reference() && m_request) {
            ACaptureRequest_free(m_request);
            m_request = nullptr;
        }
    }

private:
    ACaptureRequest *m_request = nullptr;
};

class CamDevice : Object {
public:
    explicit CamDevice(ACameraDevice *device) : m_device(device) {}

    CamDevice(const CamDevice &o) : m_device(o.m_device), Object(o) {}

    ~CamDevice() {
        if (no_reference() && m_device) {
            ACameraDevice_close(m_device);
            m_device = nullptr;
        }
    }

public:
    const char *id() {
        return ACameraDevice_getId(m_device);
    }

private:
    ACameraDevice *m_device = nullptr;
};

NAMESPACE_END
