//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "CamInput.h"
#include "CamUtils.h"
#include "MetadataOutput.h"
#include "PhotoOutput.h"
#include "PreviewOutput.h"
#include "VideoOutput.h"
#include <functional>
#include <ohcamera/capture_session.h>

NAMESPACE_DEFAULT

typedef Camera_FocusState CamFocusState;

class CaptureSession;

class SessionListener {
public:
    virtual void onSessionFocusStateChange(CaptureSession &session, CamFocusState state) {}

    virtual void onSessionError(CaptureSession &session, CamErrorCode error) {}
};

typedef std::function<CamErrorCode(CaptureSession &session)> ConfigRunnable;

// 封装 Camera_CaptureSession
class CaptureSession {
public:
    explicit CaptureSession(Camera_CaptureSession *session);
    ~CaptureSession();

public:
    /**
     * 注册聚焦状态 和 error时的回调
     */
    CamErrorCode registerListener(SessionListener *listener);

    /**
     * 注销回调
     */
    CamErrorCode unregisterListener(SessionListener *listener);

    CamErrorCode beginConfig();

    CamErrorCode commitConfig();

    /**
     * 内部会执行
     * beginConfig()
     * run()
     * commitConfig()
     */
    CamErrorCode config(const ConfigRunnable &run);

    CamInput *addInput(const CamDevice &device);

    CamErrorCode removeInput(const CamDevice &device);

    CamErrorCode removeInput(const CamInput *input);

    PreviewOutput *addPreviewOutput(const CamProfile *profile, const char *surfaceId);

    CamErrorCode addPreviewOutput(PreviewOutput *output);

    CamErrorCode removePreviewOutput(const PreviewOutput *output);

    PhotoOutput *addPhotoOutput(const CamProfile *profile, const char *surfaceId);
    
    PhotoOutput *addPhotoOutputWithoutSurface(const CamProfile *profile);

    CamErrorCode addPhotoOutput(PhotoOutput *output);

    CamErrorCode removePhotoOutput(const PhotoOutput *output);

    VideoOutput *addVideoOutput(const CamProfile *profile, const char *surfaceId);

    CamErrorCode addVideoOutput(VideoOutput *output);

    CamErrorCode removeVideoOutput(const VideoOutput *output);

    MetadataOutput *addMetadataOutput(const Camera_MetadataObjectType type);

    CamErrorCode addMetadataOutput(MetadataOutput *output);

    CamErrorCode removeMetadataOutput(const MetadataOutput *output);

    inline bool isStarted() const { return m_started; }

    CamErrorCode start();

    CamErrorCode stop();

public:
    // flash
    bool hasFlash() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        bool hasFlash = false;
        CamErrorCode error = OH_CaptureSession_HasFlash(m_session, &hasFlash);
        _WARN_IF(error, "check HasFlash failed: %s", CamUtils::errString(error));

        return hasFlash;
    }

    bool isFlashModeSupported(Camera_FlashMode mode) const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        bool isSupported = false;
        CamErrorCode error = OH_CaptureSession_IsFlashModeSupported(m_session, mode, &isSupported);
        _WARN_IF(error, "check IsFlashModeSupported failed: %s", CamUtils::errString(error))
        return isSupported;
    }

    Camera_FlashMode getFlashMode() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        Camera_FlashMode mode = Camera_FlashMode::FLASH_MODE_CLOSE;
        CamErrorCode error = OH_CaptureSession_GetFlashMode(m_session, &mode);
        _WARN_IF(error, "GetFlashMode failed: %s", CamUtils::errString(error))
        return mode;
    }

    CamErrorCode setFlashMode(Camera_FlashMode mode) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_SetFlashMode(m_session, mode);
        _WARN_IF(error, "SetFlashMode(%s) failed: %s", CamUtils::flashModeStr(mode), CamUtils::errString(error));
        return error;
    }


    // exposure mode
    bool isExposureModeSupported(Camera_ExposureMode mode) const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        bool supported = false;
        CamErrorCode error = OH_CaptureSession_IsExposureModeSupported(m_session, mode, &supported);
        _WARN_IF(error, "check IsExposureModeSupported(%s) failed: %s", CamUtils::exposureModeStr(mode),
                 CamUtils::errString(error));
        return supported;
    }

    Camera_ExposureMode getExposureMode() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        Camera_ExposureMode result = Camera_ExposureMode::EXPOSURE_MODE_AUTO;
        CamErrorCode error = OH_CaptureSession_GetExposureMode(m_session, &result);
        _WARN_IF(error, "GetExposureMode failed: %s", CamUtils::errString(error))
        return result;
    }

    CamErrorCode setExposureMode(Camera_ExposureMode mode) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_SetExposureMode(m_session, mode);
        _WARN_IF(error, "SetExposureMode(%s) failed: %s", CamUtils::exposureModeStr(mode), CamUtils::errString(error));
        return error;
    }

    // metering
    CamErrorCode setMeteringPoint(double x, double y) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        Camera_Point p = {x, y};
        CamErrorCode error = OH_CaptureSession_SetFocusPoint(m_session, p);
        _WARN_IF(error, "SetFocusPoint(%f, %f) failed: %s", x, y, CamUtils::errString(error));
        return error;
    }

    // exposure bias
    CamErrorCode getExposureBiasRange(float &min, float &max, float &step) const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_GetExposureBiasRange(m_session, &min, &max, &step);
        if (error) {
            min = max = step = 0;
            _WARN("GetExposureBiasRange() failed: %s", CamUtils::errString(error));
        }
        return error;
    }

    CamErrorCode setExposureBias(float bias) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_SetExposureBias(m_session, bias);
        _WARN_IF(error, "SetExposureBias(%f) failed: %s", bias, CamUtils::errString(error));
        return error;
    }

    float getExposureBias() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        float bias = 0;
        CamErrorCode error = OH_CaptureSession_GetExposureBias(m_session, &bias);
        _WARN_IF(error, "GetExposureBias() failed: %s", CamUtils::errString(error));
        return bias;
    }

    // focus mode
    bool isFocusModeSupported(Camera_FocusMode mode) const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        bool supported = false;
        CamErrorCode error = OH_CaptureSession_IsFocusModeSupported(m_session, mode, &supported);
        _WARN_IF(error, "IsFocusModeSupported(%s) failed: %s", CamUtils::focusModeStr(mode),
                 CamUtils::errString(error));
        return supported;
    }

    Camera_FocusMode getFocusMode() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")
        Camera_FocusMode mode = FOCUS_MODE_AUTO;
        CamErrorCode error = OH_CaptureSession_GetFocusMode(m_session, &mode);
        _WARN_IF(error, "GetFocusMode() failed: %s", CamUtils::errString(error));
        return mode;
    }

    CamErrorCode setFocusMode(Camera_FocusMode mode) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")
        CamErrorCode error = OH_CaptureSession_SetFocusMode(m_session, mode);
        _WARN_IF(error, "SetFocusMode(%s) failed: %s", CamUtils::focusModeStr(mode), CamUtils::errString(error));
        return error;
    }

    // focus point
    /**
     * 聚焦点，坐标系为横方向的左上角为原点
     * (0,0)-------------(1,0)
     *  |                 |
     *  |                 |
     * (0,1)-------------(1,1)
     */
    CamErrorCode setFocusPoint(double x, double y) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        Camera_Point point = {x, y};
        CamErrorCode error = OH_CaptureSession_SetFocusPoint(m_session, point);
        _WARN_IF(error, "SetFocusPoint(%f, %f) failed: %s", x, y, CamUtils::errString(error));
        return error;
    }

    // zoom
    CamErrorCode getZoomRange(float &min, float &max) const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_GetZoomRatioRange(m_session, &min, &max);
        _WARN_IF(error, "GetZoomRange() failed: %s", CamUtils::errString(error));
        return error;
    }

    float getZoom() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session  == nullptr")

        float value = 0.0f;
        CamErrorCode error = OH_CaptureSession_GetZoomRatio(m_session, &value);
        _WARN_IF(error, "GetZoomRatio() failed: %s", CamUtils::errString(error));
        return value;
    }

    CamErrorCode setZoom(float value) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_SetZoomRatio(m_session, value);
        _WARN_IF(error, "SetZoomRatio(%f) failed: %s", value, CamUtils::errString(error));
        return error;
    }

    // Video stabilization mode
    bool isVideoStabilizationModeSupported(Camera_VideoStabilizationMode mode) const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        bool isSupported = false;
        CamErrorCode error = OH_CaptureSession_IsVideoStabilizationModeSupported(m_session, mode, &isSupported);
        _WARN_IF(error, "IsVideoStabilizationModeSupported(%s) failed: %s", CamUtils::videoStabModeStr(mode),
                 CamUtils::errString(error));
        return isSupported;
    }

    Camera_VideoStabilizationMode getVideoStabilizationMode() const {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        Camera_VideoStabilizationMode mode = STABILIZATION_MODE_OFF;
        CamErrorCode error = OH_CaptureSession_GetVideoStabilizationMode(m_session, &mode);
        _WARN_IF(error, "GetVideoStabilizationMode() failed: %s", CamUtils::errString(error));
        return mode;
    }

    CamErrorCode setVideoStabilizationMode(Camera_VideoStabilizationMode mode) {
        _FATAL_IF(m_session == nullptr, "IllegalStateError: m_session == nullptr")

        CamErrorCode error = OH_CaptureSession_SetVideoStabilizationMode(m_session, mode);
        _WARN_IF(error, "SetVideoStabilizationMode(%s) failed: %s", CamUtils::videoStabModeStr(mode),
                 CamUtils::errString(error));
        return error;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "    inputs: " << m_inputs.size() << "\n";
        ss << "    preview outputs: " << m_preview_outputs.size() << "\n";
        ss << "    photo outputs: " << m_photo_outputs.size() << "\n";
        ss << "    video outputs: " << m_video_outputs.size() << "\n";
        ss << "    metadata outputs: " << m_metadata_outputs.size() << "\n";
        ss << "    has flash: " << (hasFlash() ? "true" : "false") << "\n";
        ss << "    cur flash mode: " << CamUtils::flashModeStr(getFlashMode()) << "\n";
        ss << "    flash mode support: [";
        for (int i = 0, j = 0; i <= FLASH_MODE_ALWAYS_OPEN; ++i) {
            if (isFlashModeSupported((Camera_FlashMode)i)) {
                if (j++ > 0) {
                    ss << ", ";
                }
                ss << CamUtils::flashModeStr((Camera_FlashMode)i);
            }
        }
        ss << "]\n";
        ss << "    cur exposure mode: " << CamUtils::exposureModeStr(getExposureMode()) << "\n";
        ss << "    exposure mode support: [";
        for (int i = 0, j = 0; i <= EXPOSURE_MODE_CONTINUOUS_AUTO; ++i) {
            if (isExposureModeSupported((Camera_ExposureMode)i)) {
                if (j++ > 0) {
                    ss << ", ";
                }
                ss << CamUtils::exposureModeStr((Camera_ExposureMode)i);
            }
        }
        ss << "]\n";
        ss << "    cur exposure bias: " << getExposureBias() << "\n";
        float expBiasMin = 0, expBiasMax = 0, expBiasStep = 0;
        getExposureBiasRange(expBiasMin, expBiasMax, expBiasStep);
        ss << "    exposure bias range: [" << expBiasMin << ", " << expBiasMax << ", " << expBiasStep << "]\n";
        ss << "    cur focus mode: " << CamUtils::focusModeStr(getFocusMode()) << "\n";
        ss << "    focus mode support: [";
        for (int i = 0, j = 0; i <= FOCUS_MODE_LOCKED; ++i) {
            if (isFocusModeSupported((Camera_FocusMode)i)) {
                if (j++ > 0) {
                    ss << ", ";
                }
                ss << CamUtils::focusModeStr((Camera_FocusMode)i);
            }
        }
        float zoomMin = 0, zoomMax = 0;
        getZoomRange(zoomMin, zoomMax);
        ss << "]\n";
        ss << "    cur zoom: " << getZoom() << "\n";
        ss << "    zoom range: [" << zoomMin << ", " << zoomMax << "]\n";
        ss << "    cur video stabilization mode: " << CamUtils::videoStabModeStr(getVideoStabilizationMode()) << "\n";
        ss << "    video stabilization mode support: [";
        for (int i = 0, j = 0; i <= STABILIZATION_MODE_AUTO; ++i) {
            if (isVideoStabilizationModeSupported((Camera_VideoStabilizationMode)i)) {
                if (j++ > 0) {
                    ss << ", ";
                }
                ss << CamUtils::videoStabModeStr((Camera_VideoStabilizationMode)i);
            }
        }
        ss << "]\n";
        ss << "}";
        return ss.str();
    }

private:
    CamErrorCode release();

private:
    Camera_CaptureSession *m_session;

    std::vector<CamInput *> m_inputs;
    std::vector<PreviewOutput *> m_preview_outputs;
    std::vector<PhotoOutput *> m_photo_outputs;
    std::vector<VideoOutput *> m_video_outputs;
    std::vector<MetadataOutput *> m_metadata_outputs;

    bool m_started = false;
};

NAMESPACE_END