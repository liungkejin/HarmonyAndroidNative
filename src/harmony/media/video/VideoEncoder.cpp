//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "VideoEncoder.h"

#include "harmony/media/AVUtils.h"
#include "harmony/media/AVCapability.h"
#include "harmony/media/AVBuffer.h"
#include <multimedia/player_framework/native_averrors.h>

NAMESPACE_DEFAULT

static void _OnError(OH_AVCodec *codec, int errorCode, void *userData) {
    VideoEncoder *self = static_cast<VideoEncoder *>(userData);
    if (self) {
        self->onError(errorCode);
    } else {
        _WARN("invalid encoder instance, cannot call onError");
    }
}

static void _OnStreamChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    VideoEncoder *self = static_cast<VideoEncoder *>(userData);
    if (self) {
        VideoFormat videoFormat(format, false);
        self->onStreamChanged(videoFormat);
    } else {
        _WARN("invalid encoder instance, cannot call onStreamChanged");
    }
}

static void _OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
    VideoEncoder *self = static_cast<VideoEncoder *>(userData);
    if (self) {
        AVBuffer aVBuffer(buffer, false);
        self->onNeedInputBuffer(index, aVBuffer);
    } else {
        _WARN("invalid encoder instance, cannot call onNeedInputBuffer");
    }
}

static void _OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
    VideoEncoder *self = static_cast<VideoEncoder *>(userData);
    if (self) {
        AVBuffer aVBuffer(buffer, false);
        self->onNewOutputBuffer(index, aVBuffer);
    } else {
        _WARN("invalid encoder instance, cannot call onNewOutputBuffer");
    }
}

static void _OnNeedInputParameter(OH_AVCodec *codec, uint32_t index, OH_AVFormat *parameter, void *userData) {
    VideoEncoder *self = static_cast<VideoEncoder *>(userData);
    if (self) {
        VideoFormat videoFormat(parameter, false);
        self->onNeedInputParameter(index, videoFormat);
    } else {
        _WARN("invalid encoder instance, cannot call onNeedInputParameter");
    }
}

OH_AVErrCode VideoEncoder::configure(VideoFormat &format) {
    _ERROR_RETURN_IF(m_state != VE_STATE_CREATED, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);

    OH_AVCodecCallback callback = {.onError = _OnError,
                                   .onStreamChanged = _OnStreamChanged,
                                   .onNeedInputBuffer = _OnNeedInputBuffer,
                                   .onNewOutputBuffer = _OnNewOutputBuffer};
    OH_AVErrCode error = OH_VideoEncoder_RegisterCallback(m_codec, callback, this);
    _ERROR_RETURN_IF(error, error, "%s: RegisterCallback failed: %s", m_name, AVUtils::errString(error));

    //    error = OH_VideoEncoder_RegisterParameterCallback(m_codec, _OnNeedInputParameter, this);
    //    _ERROR_RETURN_IF(error, error, "%s: RegisterParameterCallback failed: %s", m_name, AVUtils::errString(error));

    error = onConfigure(format);
    _ERROR_RETURN_IF(error, error, "%s: onConfigure error: %s", m_name, AVUtils::errString(error));

    error = OH_VideoEncoder_Configure(m_codec, format.value());
    _ERROR_RETURN_IF(error, error, "%s: Configure failed: %s", m_name, AVUtils::errString(error));

    error = onConfigured(format);
    _ERROR_RETURN_IF(error, error, "%s: onConfigured error: %s", m_name, AVUtils::errString(error));

    m_state = VE_STATE_CONFIGURED;
    m_width = format.width();
    m_height = format.height();
    m_pixel_format = format.pixelFormat();
    m_frame_rate = format.frameRate();
    m_frame_duration_us = m_frame_rate > 0 ? (int64_t)(1000000L / m_frame_rate) : 0;
    _FATAL_IF(m_width < 1 || m_height < 1, "Invalid encode size(%d x %d)", m_width, m_height);

    int64_t bitrate = format.bitrate();
    double framerate = format.frameRate();
    _INFO("%s: configured successfully! (width: %d, height: %d, framerate: %.2lf, bitrate: %ld)", m_name, m_width,
          m_height, framerate, bitrate);
    return error;
}

OH_AVErrCode VideoEncoder::onConfigure(VideoFormat &format) {
    VideoCapability &cap = capability();
    // 检查是否支持
    int width = format.width();
    int height = format.height();
    if (!cap.isSizeSupported(width, height)) {
        _ERROR("%s: Size(%d, %d) is not supported", m_name, width, height);
        return AV_ERR_UNSUPPORT;
    }

    int64_t bitrate = format.bitrate();
    int minBr = -1, maxBr = -1;
    cap.getEncoderBitrateRange(minBr, maxBr);
    if (minBr < 0 || maxBr < 0) {
        _ERROR("%s: Bitrate(%ld) is not supported, range(%d, %d)", m_name, bitrate, minBr, maxBr);
        return AV_ERR_UNSUPPORT;
    }
    if (bitrate > maxBr) {
        _WARN("%s: Bitrate(%d) exceeds maximum(%d), force set to maximum", m_name, bitrate, maxBr);
        format.setBitrate(maxBr);
    } else if (bitrate < minBr) {
        _WARN("%s: Bitrate(%d) is below minimum(%d), force set to minimum", m_name, bitrate, minBr);
        format.setBitrate(minBr);
    }

    double framerate = format.frameRate();
    if (!cap.isFrameRateSupported(width, height, (int)framerate)) {
        _ERROR("%s: FrameRate(%.2lf) is not supported, size(%d, %d)", m_name, framerate, width, height);
        return AV_ERR_UNSUPPORT;
    }

    int bitrateMode = format.encodeBitrateMode();
    if (bitrateMode >= 0) {
        OH_BitrateMode mode = (OH_BitrateMode)bitrateMode;
        if (!cap.isEncoderBitrateModeSupported(mode)) {
            _ERROR("%s: BitrateMode(%d) is not supported", m_name, bitrateMode);
            return AV_ERR_UNSUPPORT;
        }
    }

    return OH_AVErrCode::AV_ERR_OK;
}

NAMESPACE_END