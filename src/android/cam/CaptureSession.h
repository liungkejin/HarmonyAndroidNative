//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <vector>
#include <camera/NdkCameraCaptureSession.h>
#include "Namespace.h"
#include "common/Object.h"
#include "CaptureRequest.h"
#include "CamCharacteristics.h"

NAMESPACE_DEFAULT

class CaptureSession;

class SessionStateListener {
    friend class CaptureSession;
public:
    virtual void onClosed(CaptureSession &session) {}
    virtual void onReady(CaptureSession &session) {}
    virtual void onActive(CaptureSession &session) {}
};

class SessionCaptureListener {
    friend class CaptureSession;
public:
    virtual void onCaptureStarted(CaptureSession &session, const ACaptureRequest *request, int64_t timestamp) {}
    virtual void onCaptureProgressed(CaptureSession &session, ACaptureRequest *request, const ACameraMetadata *result) {}
    virtual void onCaptureCompleted(CaptureSession &session, ACaptureRequest *request, const ACameraMetadata *result) {}
    virtual void onCaptureFailed(CaptureSession &session, const ACaptureRequest *request, const ACameraCaptureFailure &failure) {}
    virtual void onCaptureSequenceCompleted(CaptureSession &session, int sequenceId, int64_t frameNumber) {}
    virtual void onCaptureSequenceAborted(CaptureSession &session, int sequenceId) {}
    virtual void onCaptureBufferLost(CaptureSession &session, ACaptureRequest *request, ACameraWindowType *window, int64_t frameNumber) {}
};

class CaptureSession : Object {
public:
    explicit CaptureSession(ACameraCaptureSession *session) : m_session(session) {}
    CaptureSession(const CaptureSession &o) : m_session(o.m_session), Object(o) {}
    ~CaptureSession() {
        if (no_reference() && m_session) {
            ACameraCaptureSession_close(m_session);
            m_session = nullptr;
        }
    }

public:

    CamStatus capture(std::vector<CaptureRequest> requests, SessionCaptureListener *listener = nullptr);

    CamStatus abortCaptures();

    CamStatus setRepeatingRequest(std::vector<CaptureRequest> requests, SessionCaptureListener *listener = nullptr);

    CamStatus stopRepeating();

    void close() {
        ACameraCaptureSession_close(m_session);
    }

private:

    void onCaptureStarted(const ACaptureRequest *request, int64_t timestamp) {}

    void onCaptureProgressed(const ACaptureRequest *request, const CamCharacteristics &result) {}

    void onCaptureCompleted(const ACaptureRequest *request, const CamCharacteristics &result) {}

    void onCaptureFailed(const ACaptureRequest *request, const ACameraCaptureFailure &failure) {}

    void onCaptureSequenceCompleted(int sequenceId, int64_t frameNumber) {}

    void onCaptureSequenceAborted(int sequenceId) {}

    void onCaptureBufferLost(const ACaptureRequest *request, ACameraWindowType *window, int64_t frameNumber) {}

private:
    ACameraCaptureSession *m_session;
};

NAMESPACE_END
