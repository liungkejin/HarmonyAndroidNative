//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "AudioEncoder.h"

#include "harmony/media/AVUtils.h"
#include "harmony/media/AVCapability.h"
#include "harmony/media/AVBuffer.h"
#include "harmony/media/AVFormat.h"
#include <multimedia/player_framework/native_averrors.h>

NAMESPACE_DEFAULT

static void _OnError(OH_AVCodec *codec, int errorCode, void *userData) {
    AudioEncoder *self = static_cast<AudioEncoder *>(userData);
    if (self) {
        self->onError(errorCode);
    } else {
        _WARN("invalid encoder instance, cannot call onError");
    }
}

static void _OnStreamChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    AudioEncoder *self = static_cast<AudioEncoder *>(userData);
    if (self) {
        AudioFormat audioFormat(format, false);
        self->onStreamChanged(audioFormat);
    } else {
        _WARN("invalid encoder instance, cannot call onStreamChanged");
    }
}

static void _OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
    AudioEncoder *self = static_cast<AudioEncoder *>(userData);
    if (self) {
        AVBuffer aVBuffer(buffer, false);
        self->onNeedInputBuffer(index, aVBuffer);
    } else {
        _WARN("invalid encoder instance, cannot call onNeedInputBuffer");
    }
}

static void _OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
    AudioEncoder *self = static_cast<AudioEncoder *>(userData);
    if (self) {
        AVBuffer aVBuffer(buffer, false);
        self->onNewOutputBuffer(index, aVBuffer);
    } else {
        _WARN("invalid encoder instance, cannot call onNewOutputBuffer");
    }
}

OH_AVErrCode AudioEncoder::configure(AudioFormat &format) {
    _ERROR_RETURN_IF(m_state != AE_STATE_CREATED, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name,
                     m_state);

    OH_AVCodecCallback callback = {.onError = _OnError,
                                   .onStreamChanged = _OnStreamChanged,
                                   .onNeedInputBuffer = _OnNeedInputBuffer,
                                   .onNewOutputBuffer = _OnNewOutputBuffer};
    OH_AVErrCode error = OH_AudioCodec_RegisterCallback(m_codec, callback, this);
    _ERROR_RETURN_IF(error, error, "%s: RegisterCallback failed: %s", m_name, AVUtils::errString(error));

    error = onConfigure(format);
    _ERROR_RETURN_IF(error, error, "%s: onConfigure error: %s", m_name, AVUtils::errString(error));

    error = OH_AudioCodec_Configure(m_codec, format.value());
    _ERROR_RETURN_IF(error, error, "%s: Configure failed: %s", m_name, AVUtils::errString(error));

    error = onConfigured(format);
    _ERROR_RETURN_IF(error, error, "%s: onConfigured error: %s", m_name, AVUtils::errString(error));

    m_state = AE_STATE_CONFIGURED;

    m_sample_rate = format.sampleRate();
    m_channel_count = format.channelCount();
    m_bit_rate = format.bitrate();
    m_sample_format = (OH_BitsPerSample)format.sampleFormat();
    _INFO("%s: configured successfully! (sampleRate: %d, channels: %d, bitrate: %ld, sampleFormat: %d)",
          m_name, m_sample_rate, m_channel_count, m_bit_rate, m_sample_format);
    return error;
}

OH_AVErrCode AudioEncoder::onConfigure(AudioFormat &format) {
    AudioCapability &cap = capability();
    // 检查是否支持
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

    return OH_AVErrCode::AV_ERR_OK;
}

NAMESPACE_END