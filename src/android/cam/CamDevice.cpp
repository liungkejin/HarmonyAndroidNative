//
// Created by LiangKeJin on 2024/12/7.
//

#include "CamDevice.h"

NAMESPACE_DEFAULT

static void gOnSessionClosed(void *context, ACameraCaptureSession *session) {
    auto listener = reinterpret_cast<SessionStateListener *>(context);
    if (listener) {
        CaptureSession s(session, false);
        listener->onClosed(s);
    }
}

static void gOnSessionReady(void *context, ACameraCaptureSession *session) {
    auto listener = reinterpret_cast<SessionStateListener *>(context);
    if (listener) {
        CaptureSession s(session, false);
        listener->onReady(s);
    }
}

static void gOnSessionActive(void *context, ACameraCaptureSession *session) {
    auto listener = reinterpret_cast<SessionStateListener *>(context);
    if (listener) {
        CaptureSession s(session, false);
        listener->onActive(s);
    }
}

static ACameraCaptureSession_stateCallbacks gSessionStateCallbacks = {
        .context = nullptr,
        .onClosed = gOnSessionClosed,
        .onReady = gOnSessionReady,
        .onActive = gOnSessionActive,
};

CaptureSession CamDevice::createCaptureSession(CaptureSessionOutputContainer &container,
                                               SessionStateListener *stateListener) {
    ACameraCaptureSession *session = nullptr;
    gSessionStateCallbacks.context = stateListener;
    auto ret = ACameraDevice_createCaptureSession(
            m_device, container.value(), &gSessionStateCallbacks, &session);
    _FATAL_IF(ret, "ACameraDevice_createCaptureSession failed: %s", CamUtils::errString(ret));
    return CaptureSession(session);
}

NAMESPACE_END