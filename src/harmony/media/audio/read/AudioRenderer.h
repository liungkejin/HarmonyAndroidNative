//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/Object.h"
#include "ASUtils.h"
#include <ohaudio/native_audiorenderer.h>

NAMESPACE_DEFAULT

class AudioRenderer : public Object {
public:
    AudioRenderer(OH_AudioRenderer *capturer, bool owner = true) : m_renderer(capturer), m_owner(owner) {}

    AudioRenderer(const AudioRenderer &other) : m_renderer(other.m_renderer), m_owner(other.m_owner), Object(other) {}

    ~AudioRenderer() {
        if (m_owner && m_renderer && no_reference()) {
            OH_AudioRenderer_Release(m_renderer);
            m_renderer = nullptr;
        }
    }

public:
    inline bool valid() const { return m_renderer != nullptr; }

    OH_AudioStream_Result start() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_Start(m_renderer);
        _ERROR_IF(!result, "OH_AudioRenderer_Start failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_Result pause() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_Pause(m_renderer);
        _ERROR_IF(!result, "OH_AudioRenderer_Pause failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_Result stop() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_Stop(m_renderer);
        _ERROR_IF(!result, "OH_AudioRenderer_Stop failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_Result flush() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_Flush(m_renderer);
        _ERROR_IF(!result, "OH_AudioRenderer_Flush failed: %s", ASUtils::resultString(result));
        return result;
    }

public:
    OH_AudioStream_State curState() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_State state = AUDIOSTREAM_STATE_INVALID;
        OH_AudioStream_Result result = OH_AudioRenderer_GetCurrentState(m_renderer, &state);
        _ERROR_IF(!result, "OH_AudioRenderer_GetCurrentState failed: %s", ASUtils::resultString(result));
        return state;
    }

    bool isLatencyMode() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_LatencyMode mode = AUDIOSTREAM_LATENCY_MODE_NORMAL;
        OH_AudioStream_Result result = OH_AudioRenderer_GetLatencyMode(m_renderer, &mode);
        _ERROR_IF(!result, "OH_AudioRenderer_IsLatencyMode failed: %s", ASUtils::resultString(result));
        return mode != AUDIOSTREAM_LATENCY_MODE_NORMAL;
    }

    uint32_t streamId() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        uint32_t id = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetStreamId(m_renderer, &id);
        _ERROR_IF(!result, "OH_AudioRenderer_GetStreamId failed: %s", ASUtils::resultString(result));
        return id;
    }

    int samplingRate() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        int rate = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetSamplingRate(m_renderer, &rate);
        _ERROR_IF(!result, "OH_AudioRenderer_GetSamplingRate failed: %s", ASUtils::resultString(result));
        return rate;
    }

    int channelCount() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        int count = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetChannelCount(m_renderer, &count);
        _ERROR_IF(!result, "OH_AudioRenderer_GetChannelCount failed: %s", ASUtils::resultString(result));
        return count;
    }

    OH_AudioStream_SampleFormat sampleFormat() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_SampleFormat format = AUDIOSTREAM_SAMPLE_U8;
        OH_AudioStream_Result result = OH_AudioRenderer_GetSampleFormat(m_renderer, &format);
        _ERROR_IF(!result, "OH_AudioRenderer_GetSampleFormat failed: %s", ASUtils::resultString(result));
        return format;
    }

    OH_AudioStream_EncodingType encodingType() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_EncodingType type = AUDIOSTREAM_ENCODING_TYPE_RAW;
        OH_AudioStream_Result result = OH_AudioRenderer_GetEncodingType(m_renderer, &type);
        _ERROR_IF(!result, "OH_AudioRenderer_GetEncodingType failed: %s", ASUtils::resultString(result));
        return type;
    }

    OH_AudioStream_Usage usage() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Usage _usage = AUDIOSTREAM_USAGE_UNKNOWN;
        OH_AudioStream_Result result = OH_AudioRenderer_GetRendererInfo(m_renderer, &_usage);
        _ERROR_IF(!result, "OH_AudioRenderer_GetRendererInfo failed: %s", ASUtils::resultString(result));
        return _usage;
    }

    int64_t framesWritten() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        int64_t count = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetFramesWritten(m_renderer, &count);
        _ERROR_IF(!result, "OH_AudioRenderer_GetFramesWritten failed: %s", ASUtils::resultString(result));
        return count;
    }

    int frameSize() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        int size = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetFrameSizeInCallback(m_renderer, &size);
        _ERROR_IF(!result, "OH_AudioRenderer_GetFrameSize failed: %s", ASUtils::resultString(result));
        return size;
    }

    int64_t timestamp(clockid_t clockId, int64_t &framePosition) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        int64_t timestamp = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetTimestamp(m_renderer, clockId, &framePosition, &timestamp);
        _ERROR_IF(!result, "OH_AudioRenderer_GetTimestamp failed: %s", ASUtils::resultString(result));
        return timestamp;
    }

public:
    float getSpeed() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        float speed = 0.0f;
        OH_AudioStream_Result result = OH_AudioRenderer_GetSpeed(m_renderer, &speed);
        _ERROR_IF(!result, "OH_AudioRenderer_GetSpeed failed: %s", ASUtils::resultString(result));
        return speed;
    }

    OH_AudioStream_Result setSpeed(float speed) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_SetSpeed(m_renderer, speed);
        _ERROR_IF(!result, "OH_AudioRenderer_SetSpeed failed: %s", ASUtils::resultString(result));
        return result;
    }

    float getVolume() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        float volume = 0.0f;
        OH_AudioStream_Result result = OH_AudioRenderer_GetVolume(m_renderer, &volume);
        _ERROR_IF(!result, "OH_AudioRenderer_GetVolume failed: %s", ASUtils::resultString(result));
        return volume;
    }

    OH_AudioStream_Result setVolume(float volume) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_SetVolume(m_renderer, volume);
        _ERROR_IF(!result, "OH_AudioRenderer_SetVolume failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_Result setVolumeWithRamp(float volume, int durationMs) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_SetVolumeWithRamp(m_renderer, volume, durationMs);
        _ERROR_IF(!result, "OH_AudioRenderer_SetVolumeWithRamp failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_Result setMarkPosition(uint32_t samplePos, OH_AudioRenderer_OnMarkReachedCallback callback,
                                          void *userData) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_SetMarkPosition(m_renderer, samplePos, callback, userData);
        _ERROR_IF(!result, "OH_AudioRenderer_SetMarkPosition failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_Result cancelMark() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_CancelMark(m_renderer);
        _ERROR_IF(!result, "OH_AudioRenderer_CancelMark failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioChannelLayout channelLayout() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioChannelLayout layout = CH_LAYOUT_UNKNOWN;
        OH_AudioStream_Result result = OH_AudioRenderer_GetChannelLayout(m_renderer, &layout);
        _ERROR_IF(!result, "OH_AudioRenderer_GetChannelLayout failed: %s", ASUtils::resultString(result));
        return layout;
    }

    OH_AudioStream_AudioEffectMode effectMode() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_AudioEffectMode mode = EFFECT_NONE;
        OH_AudioStream_Result result = OH_AudioRenderer_GetEffectMode(m_renderer, &mode);
        _ERROR_IF(!result, "OH_AudioRenderer_GetEffectMode failed: %s", ASUtils::resultString(result));
        return mode;
    }

    OH_AudioStream_Result setEffectMode(OH_AudioStream_AudioEffectMode mode) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_SetEffectMode(m_renderer, mode);
        _ERROR_IF(!result, "OH_AudioRenderer_SetEffectMode failed: %s", ASUtils::resultString(result));
        return result;
    }

    OH_AudioStream_PrivacyType privacyType() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_PrivacyType privacy = AUDIO_STREAM_PRIVACY_TYPE_PUBLIC;
        OH_AudioStream_Result result = OH_AudioRenderer_GetRendererPrivacy(m_renderer, &privacy);
        _ERROR_IF(!result, "OH_AudioRenderer_GetRendererPrivacy failed: %s", ASUtils::resultString(result));
        return privacy;
    }

    uint32_t underflowCount() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        uint32_t count = 0;
        OH_AudioStream_Result result = OH_AudioRenderer_GetUnderflowCount(m_renderer, &count);
        _ERROR_IF(!result, "OH_AudioRenderer_GetUnderflowCount failed: %s", ASUtils::resultString(result));
        return count;
    }

    OH_AudioStream_Result setSilentModeAndMixWithOthers(bool on) {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        OH_AudioStream_Result result = OH_AudioRenderer_SetSilentModeAndMixWithOthers(m_renderer, on);
        _ERROR_IF(!result, "OH_AudioRenderer_SetSilentModeAndMixWithOthers failed: %s", ASUtils::resultString(result));
        return result;
    }

    bool isSilentModeAndMixWithOthers() {
        _FATAL_IF(!m_renderer, "m_capturer == nullptr");

        bool isSilent = false;
        OH_AudioStream_Result result = OH_AudioRenderer_GetSilentModeAndMixWithOthers(m_renderer, &isSilent);
        _ERROR_IF(!result, "OH_AudioRenderer_IsSilentMode failed: %s", ASUtils::resultString(result));
        return isSilent;
    }

private:
    bool m_owner = false;
    OH_AudioRenderer *m_renderer;
};

NAMESPACE_END
