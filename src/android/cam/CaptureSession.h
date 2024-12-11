//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <camera/NdkCameraCaptureSession.h>
#include "Namespace.h"
#include "common/Object.h"
#include "CaptureRequest.h"
#include "CamCharacteristics.h"

NAMESPACE_DEFAULT

class CaptureSession;

class CaptureSessionOutput : Object {
public:
    static CaptureSessionOutput create(ACameraWindowType *window) {
        ACaptureSessionOutput *output = nullptr;
        auto ret = ACaptureSessionOutput_create(window, &output);
        _FATAL_IF(ret, "ACameraOutputTarget_create failed: %s", CamUtils::errString(ret));
        return CaptureSessionOutput(output);
    }

public:
    explicit CaptureSessionOutput(ACaptureSessionOutput *output) : m_output(output) {}

    CaptureSessionOutput(const CaptureSessionOutput &o) : m_output(o.m_output), Object(o) {}

    ~CaptureSessionOutput() {
        if (no_reference() && m_output) {
            ACaptureSessionOutput_free(m_output);
            m_output = nullptr;
        }
    }

public:
    bool valid() const {
        return m_output != nullptr;
    }

    const ACaptureSessionOutput *value() const {
        return m_output;
    }

private:
    ACaptureSessionOutput *m_output;
};

class CaptureSessionOutputContainer : Object {
public:
    CaptureSessionOutputContainer() {
        ACaptureSessionOutputContainer *container = nullptr;
        auto ret = ACaptureSessionOutputContainer_create(&container);
        _FATAL_IF(ret, "ACaptureSessionOutputContainer_create failed: %s", CamUtils::errString(ret));
        m_container = container;
    }

    CaptureSessionOutputContainer(const CaptureSessionOutputContainer &o) : m_container(o.m_container), Object(o) {}

    ~CaptureSessionOutputContainer() {
        if (no_reference() && m_container) {
            for (auto &it : m_outputs) {
                ACaptureSessionOutputContainer_remove(m_container, it.second.value());
            }
            m_outputs.clear();
            ACaptureSessionOutputContainer_free(m_container);
            m_container = nullptr;
        }
    }

public:
    bool valid() const {
        return m_container != nullptr;
    }

    const ACaptureSessionOutputContainer *value() const {
        return m_container;
    }

    CamStatus addOutput(ACameraWindowType *window) {
        CaptureSessionOutput output = CaptureSessionOutput::create(window);
        auto status = ACaptureSessionOutputContainer_add(m_container, output.value());
        _ERROR_RETURN_IF(status, status, "ACaptureSessionOutputContainer_add failed: %s", CamUtils::errString(status));
        m_outputs.insert(std::make_pair(window, output));
        return status;
    }

    CamStatus removeOutput(ACameraWindowType *window) {
        auto it = m_outputs.find(window);
        if (it == m_outputs.end()) {
            _ERROR("window not found");
            return CamStatus::ACAMERA_ERROR_UNKNOWN;
        }
        m_outputs.erase(it);
        auto status = ACaptureSessionOutputContainer_remove(m_container, it->second.value());
        _ERROR_RETURN_IF(status, status, "ACaptureSessionOutputContainer_remove failed: %s",
                         CamUtils::errString(status));
        return status;
    }

private:
    ACaptureSessionOutputContainer *m_container;
    std::unordered_map<ACameraWindowType *, CaptureSessionOutput> m_outputs;
};

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

    virtual void onCaptureBufferLost(CaptureSession &session, ACaptureRequest *request, ACameraWindowType *window,int64_t frameNumber) {}
};

class CaptureSession : Object {
public:
    explicit CaptureSession(ACameraCaptureSession *session, bool own = true) : m_session(session), m_owned(own) {}

    CaptureSession(const CaptureSession &o) : m_session(o.m_session), m_owned(o.m_owned), Object(o) {}

    ~CaptureSession() {
        if (m_owned && no_reference() && m_session) {
            ACameraCaptureSession_close(m_session);
            m_session = nullptr;
        }
    }

public:

    CamStatus capture(std::vector<CaptureRequest> requests, SessionCaptureListener *listener = nullptr);

    CamStatus abortCaptures();

    CamStatus setRepeatingRequest(std::vector<CaptureRequest> requests, SessionCaptureListener *listener = nullptr);

    CamStatus stopRepeating();

private:
    ACameraCaptureSession *m_session;
    bool m_owned = true;
};

NAMESPACE_END
