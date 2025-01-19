//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "common/Object.h"
#include "ASUtils.h"
#include "AudioCapturer.h"
#include "AudioRenderer.h"
#include <ohaudio/native_audiostream_base.h>
#include <ohaudio/native_audiostreambuilder.h>

NAMESPACE_DEFAULT

class AudioStreamBuilder : public Object {
public:
    explicit AudioStreamBuilder(OH_AudioStream_Type type) : m_type(type) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_Create(&m_builder, type);
        _ERROR_IF(result, "OH_AudioStreamBuilder_Create failed: %s", ASUtils::resultString(result));
    }

    AudioStreamBuilder(const AudioStreamBuilder &other)
        : m_type(other.m_type), m_builder(other.m_builder), Object(other) {}

    ~AudioStreamBuilder() {
        if (m_builder && no_reference()) {
            OH_AudioStreamBuilder_Destroy(m_builder);
            m_builder = nullptr;
        }
    }

public:
    AudioStreamBuilder &setSamplingRate(int rate) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetSamplingRate(m_builder, rate);
        _ERROR_IF(result, "SetSamplingRate(%d) failed: %s", rate, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setChannelCount(int count) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetChannelCount(m_builder, count);
        _ERROR_IF(result, "SetChannelCount(%d) failed: %s", count, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setSampleFormat(OH_AudioStream_SampleFormat format) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetSampleFormat(m_builder, format);
        _ERROR_IF(result, "SetSampleFormat(%d) failed: %s", format, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setEncodingType(OH_AudioStream_EncodingType encoding) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetEncodingType(m_builder, encoding);
        _ERROR_IF(result, "SetEncodingType(%d) failed: %s", encoding, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setLatencyMode(OH_AudioStream_LatencyMode mode) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetLatencyMode(m_builder, mode);
        _ERROR_IF(result, "SetLatencyMode(%d) failed: %s", mode, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setChannelLayout(OH_AudioChannelLayout layout) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetChannelLayout(m_builder, layout);
        _ERROR_IF(result, "SetChannelLayout(%d) failed: %s", layout, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setFrameSizeInCallback(int frameSize) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetFrameSizeInCallback(m_builder, frameSize);
        _ERROR_IF(result, "SetFrameSizeInCallback(%d) failed: %s", frameSize, ASUtils::resultString(result));
        return *this;
    }

public:
    AudioStreamBuilder &setCapturerSource(OH_AudioStream_SourceType type) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetCapturerInfo(m_builder, type);
        _ERROR_IF(result, "SetCapturerInfo(%d) failed: %s", type, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setCapturerCallback(OH_AudioCapturer_Callbacks callbaks, void *userData) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetCapturerCallback(m_builder, callbaks, userData);
        _ERROR_IF(result, "SetCapturerCallback failed: %s", ASUtils::resultString(result));
        return *this;
    }

    AudioCapturer *createCapturer() {
        OH_AudioCapturer *stream = nullptr;
        OH_AudioStream_Result result = OH_AudioStreamBuilder_GenerateCapturer(m_builder, &stream);
        _ERROR_RETURN_IF(result, nullptr, "CreateCapturerStream failed: %s", ASUtils::resultString(result));
        return new AudioCapturer(stream, true);
    }

public:
    AudioRenderer *createRenderer() {
        OH_AudioRenderer *stream = nullptr;
        OH_AudioStream_Result result = OH_AudioStreamBuilder_GenerateRenderer(m_builder, &stream);
        _ERROR_RETURN_IF(result, nullptr, "CreateRendererStream failed: %s", ASUtils::resultString(result));
        return new AudioRenderer(stream, true);
    }

    AudioStreamBuilder &setRendererInfo(OH_AudioStream_Usage usage) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetRendererInfo(m_builder, usage);
        _ERROR_IF(result, "SetRendererInfo(%d) failed: %s", usage, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setRendererCallback(OH_AudioRenderer_Callbacks callbaks, void *userData) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetRendererCallback(m_builder, callbaks, userData);
        _ERROR_IF(result, "SetRendererCallback failed: %s", ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setRendererOutputDeviceChangeCallback(OH_AudioRenderer_OutputDeviceChangeCallback callback,
                                                              void *userData) {
        OH_AudioStream_Result result =
            OH_AudioStreamBuilder_SetRendererOutputDeviceChangeCallback(m_builder, callback, userData);
        _ERROR_IF(result, "SetRendererOutputDeviceChangeCallback failed: %s", ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setRendererPrivacy(OH_AudioStream_PrivacyType type) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetRendererPrivacy(m_builder, type);
        _ERROR_IF(result, "SetRendererPrivacy(%d) failed: %s", type, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setWriteDataWithMetadataCallback(OH_AudioRenderer_WriteDataWithMetadataCallback callback,
                                                         void *userData) {
        OH_AudioStream_Result result =
            OH_AudioStreamBuilder_SetWriteDataWithMetadataCallback(m_builder, callback, userData);
        _ERROR_IF(result, "SetWriteDataMetadataCallback failed: %s", ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setRendererInterruptMode(OH_AudioInterrupt_Mode mode) {
        OH_AudioStream_Result result = OH_AudioStreamBuilder_SetRendererInterruptMode(m_builder, mode);
        _ERROR_IF(result, "SetRendererInterruptMode(%d) failed: %s", mode, ASUtils::resultString(result));
        return *this;
    }

    AudioStreamBuilder &setRendererWriteDataCallback(OH_AudioRenderer_OnWriteDataCallback callback, void *userData) {
        OH_AudioStream_Result result =
            OH_AudioStreamBuilder_SetRendererWriteDataCallback(m_builder, callback, userData);
        _ERROR_IF(result, "SetRendererWriteDataCallback failed: %s", ASUtils::resultString(result));
        return *this;
    }

private:
    OH_AudioStreamBuilder *m_builder = nullptr;
    const OH_AudioStream_Type m_type;
};

NAMESPACE_END
