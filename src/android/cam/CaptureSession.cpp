//
// Created by LiangKeJin on 2024/12/7.
//

#include "CaptureSession.h"

NAMESPACE_DEFAULT

struct CallbackBox {
    CaptureSession *session = nullptr;
    SessionCaptureListener *cap_listener = nullptr;
    SessionStateListener *state_listener = nullptr;
};

static void gOnCaptureStarted(void *context, ACameraCaptureSession *session, const ACaptureRequest *request, int64_t timestamp) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureStarted(*box->session, request, timestamp);
    }
}

static void gOnCaptureProgressed(void *context, ACameraCaptureSession *session, ACaptureRequest *request, const ACameraMetadata *result) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureProgressed(*box->session, request, result);
    }
}

static void gOnCaptureCompleted(void *context, ACameraCaptureSession *session, ACaptureRequest *request, const ACameraMetadata *result) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureCompleted(*box->session, request, result);
    }
}

static void gOnCaptureFailed(void *context, ACameraCaptureSession *session, ACaptureRequest *request, ACameraCaptureFailure *failure) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureFailed(*box->session, request, *failure);
    }
}

static void gOnCaptureSequenceCompleted(void *context, ACameraCaptureSession *session, int sequenceId, int64_t frameNumber) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureSequenceCompleted(*box->session, sequenceId, frameNumber);
    }
}

static void gOnCaptureSequenceAborted(void *context, ACameraCaptureSession *session, int sequenceId) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureSequenceAborted(*box->session, sequenceId);
    }
}

static void gOnCaptureBufferLost(void *context, ACameraCaptureSession *session, ACaptureRequest *request, ACameraWindowType *window, int64_t frameNumber) {
    auto *box = static_cast<CallbackBox *>(context);
    if (box->cap_listener) {
        box->cap_listener->onCaptureBufferLost(*box->session, request, window, frameNumber);
    }
}

static CallbackBox gCaptureContext = {};
static ACameraCaptureSession_captureCallbacks gCaptureCallbacks = {
        .context = &gCaptureContext,
        .onCaptureStarted = gOnCaptureStarted,
        .onCaptureProgressed = gOnCaptureProgressed,
        .onCaptureCompleted = gOnCaptureCompleted,
        .onCaptureFailed = gOnCaptureFailed,
        .onCaptureSequenceCompleted = gOnCaptureSequenceCompleted,
        .onCaptureSequenceAborted = gOnCaptureSequenceAborted,
        .onCaptureBufferLost = gOnCaptureBufferLost,
};

CamStatus CaptureSession::capture(std::vector<CaptureRequest> requests, znative::SessionCaptureListener *listener) {
    ACaptureRequest *array[requests.size()];
    for (size_t i = 0; i < requests.size(); ++i) {
        array[i] = requests[i].value();
    }

    this->abortCaptures();

    ACameraCaptureSession_captureCallbacks *cb = nullptr;
    if (listener) {
        gCaptureContext.session = this;
        gCaptureContext.cap_listener = listener;
        cb = &gCaptureCallbacks;
    }
    auto error = ACameraCaptureSession_capture(
            m_session, cb, (int) requests.size(), array, nullptr);
    _ERROR_IF(error, "ACameraCaptureSession_capture failed: %s", CamUtils::errString(error));
    return error;
}

CamStatus CaptureSession::abortCaptures() {
    auto error = ACameraCaptureSession_abortCaptures(m_session);
    _ERROR_IF(error, "ACameraCaptureSession_abortCaptures failed: %s", CamUtils::errString(error));
    return error;
}

static CallbackBox gRepeatingContext = {};
static ACameraCaptureSession_captureCallbacks gRepeatingCallbacks = {
        .context = &gRepeatingContext,
        .onCaptureStarted = gOnCaptureStarted,
        .onCaptureProgressed = gOnCaptureProgressed,
        .onCaptureCompleted = gOnCaptureCompleted,
        .onCaptureFailed = gOnCaptureFailed,
        .onCaptureSequenceCompleted = gOnCaptureSequenceCompleted,
        .onCaptureSequenceAborted = gOnCaptureSequenceAborted,
        .onCaptureBufferLost = gOnCaptureBufferLost,
};

CamStatus CaptureSession::setRepeatingRequest(std::vector<CaptureRequest> requests, znative::SessionCaptureListener *listener) {
    ACaptureRequest *array[requests.size()];
    for (size_t i = 0; i < requests.size(); ++i) {
        array[i] = requests[i].value();
    }

    this->stopRepeating();

    ACameraCaptureSession_captureCallbacks *cb = nullptr;
    if (listener) {
        gRepeatingContext.session = this;
        gRepeatingContext.cap_listener = listener;
        cb = &gRepeatingCallbacks;
    }
    auto error = ACameraCaptureSession_setRepeatingRequest(
            m_session, cb, (int) requests.size(), array, nullptr);
    _ERROR_IF(error, "ACameraCaptureSession_setRepeatingRequest failed: %s", CamUtils::errString(error));
    return error;
}

CamStatus CaptureSession::stopRepeating() {
    auto error = ACameraCaptureSession_stopRepeating(m_session);
    _ERROR_IF(error, "ACameraCaptureSession_stopRepeating failed: %s", CamUtils::errString(error));
    return error;
}

NAMESPACE_END