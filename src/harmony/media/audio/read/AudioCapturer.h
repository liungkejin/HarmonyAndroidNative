//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "ZNamespace.h"
#include "common/Object.h"
#include "ASUtils.h"
#include <cstdint>
#include <multimedia/native_audio_channel_layout.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <ohaudio/native_audiostream_base.h>
#include <ohaudio/native_audiocapturer.h>

NAMESPACE_DEFAULT

class AudioCapturer : public Object {
public:
    explicit AudioCapturer(OH_AudioCapturer *capturer, bool owner = true) : m_capturer(capturer), m_owner(owner) {}

    AudioCapturer(const AudioCapturer &other) : m_capturer(other.m_capturer), m_owner(other.m_owner), Object(other) {}

    ~AudioCapturer() {
        if (m_owner && m_capturer && no_reference()) {
            OH_AudioCapturer_Release(m_capturer);
            m_capturer = nullptr;
        }
    }

public:
    inline bool valid() const { return m_capturer != nullptr; }
    
    OH_AudioStream_Result start() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_Result result = OH_AudioCapturer_Start(m_capturer);
        _ERROR_IF(result, "OH_AudioCapturer_Start failed: %s", ASUtils::resultString(result));
        return result;
    }
    
    OH_AudioStream_Result pause() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_Result result = OH_AudioCapturer_Pause(m_capturer);
        _ERROR_IF(result, "OH_AudioCapturer_Pause failed: %s", ASUtils::resultString(result));
        return result;
    }
    
    OH_AudioStream_Result stop() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_Result result = OH_AudioCapturer_Stop(m_capturer);
        _ERROR_IF(result, "OH_AudioCapturer_Stop failed: %s", ASUtils::resultString(result));
        return result;
    }
    
    OH_AudioStream_Result flush() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_Result result = OH_AudioCapturer_Flush(m_capturer);
        _ERROR_IF(result, "OH_AudioCapturer_Flush failed: %s", ASUtils::resultString(result));
        return result;
    }
    
public:
    OH_AudioStream_State curState() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_State state = AUDIOSTREAM_STATE_INVALID;
        OH_AudioStream_Result result = OH_AudioCapturer_GetCurrentState(m_capturer, &state);
        _ERROR_IF(result, "OH_AudioCapturer_GetCurrentState failed: %s", ASUtils::resultString(result));
        return state;
    }
    
    bool isLatencyMode() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_LatencyMode mode = AUDIOSTREAM_LATENCY_MODE_NORMAL;
        OH_AudioStream_Result result = OH_AudioCapturer_GetLatencyMode(m_capturer, &mode);
        _ERROR_IF(result, "OH_AudioCapturer_IsLatencyMode failed: %s", ASUtils::resultString(result));
        return mode != AUDIOSTREAM_LATENCY_MODE_NORMAL;
    }
    
    uint32_t streamId() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        uint32_t id = 0;
        OH_AudioStream_Result result = OH_AudioCapturer_GetStreamId(m_capturer, &id);
        _ERROR_IF(result, "OH_AudioCapturer_GetStreamId failed: %s", ASUtils::resultString(result));
        return id;
    }
    
    int samplingRate() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        int rate = 0;
        OH_AudioStream_Result result = OH_AudioCapturer_GetSamplingRate(m_capturer, &rate);
        _ERROR_IF(result, "OH_AudioCapturer_GetSamplingRate failed: %s", ASUtils::resultString(result));
        return rate;
    }

    OH_AudioChannelLayout channelLayout() const {
        int channels = channelCount();
        if (channels == 1) {
            return CH_LAYOUT_MONO;
        } else if (channels == 2) {
            return CH_LAYOUT_STEREO;
        } else if (channels == 3) {
            return CH_LAYOUT_3POINT0;
        } else if (channels == 4) {
            return CH_LAYOUT_4POINT0;
        } else {
            _FATAL("unsupported channels for AudioCapturer: %d, sample format: %d", channels, sampleFormat());
        }
    }

    int channelCount() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        int count = 0;
        OH_AudioStream_Result result = OH_AudioCapturer_GetChannelCount(m_capturer, &count);
        _ERROR_IF(result, "OH_AudioCapturer_GetChannelCount failed: %s", ASUtils::resultString(result));
        return count;
    }

    OH_BitsPerSample bitsPerSample() const {
        OH_AudioStream_SampleFormat format = sampleFormat();
        switch (format) {
        case AUDIOSTREAM_SAMPLE_U8 :
            return OH_BitsPerSample::SAMPLE_U8;
        case AUDIOSTREAM_SAMPLE_S16LE :
            return OH_BitsPerSample::SAMPLE_S16LE;
        case AUDIOSTREAM_SAMPLE_S24LE :
            return OH_BitsPerSample::SAMPLE_S24LE;
        case AUDIOSTREAM_SAMPLE_S32LE :
            return OH_BitsPerSample::SAMPLE_S32LE;
        default :
            _ERROR("unknown audio sample format: %d", format);
            return OH_BitsPerSample::INVALID_WIDTH;
        }
    }

    OH_AudioStream_SampleFormat sampleFormat() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_SampleFormat format = AUDIOSTREAM_SAMPLE_U8;
        OH_AudioStream_Result result = OH_AudioCapturer_GetSampleFormat(m_capturer, &format);
        _ERROR_IF(result, "OH_AudioCapturer_GetSampleFormat failed: %s", ASUtils::resultString(result));
        return format;
    }
    
    OH_AudioStream_EncodingType encodingType() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_EncodingType type = AUDIOSTREAM_ENCODING_TYPE_RAW;
        OH_AudioStream_Result result = OH_AudioCapturer_GetEncodingType(m_capturer, &type);
        _ERROR_IF(result, "OH_AudioCapturer_GetEncodingType failed: %s", ASUtils::resultString(result));
        return type;
    }
    
    OH_AudioStream_SourceType sourceType() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        OH_AudioStream_SourceType type = AUDIOSTREAM_SOURCE_TYPE_INVALID;
        OH_AudioStream_Result result = OH_AudioCapturer_GetCapturerInfo(m_capturer, &type);
        _ERROR_IF(result, "OH_AudioCapturer_GetSourceType failed: %s", ASUtils::resultString(result));
        return type;
    }
    
    int frameSize() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        int size = 0;
        OH_AudioStream_Result result = OH_AudioCapturer_GetFrameSizeInCallback(m_capturer, &size);
        _ERROR_IF(result, "OH_AudioCapturer_GetFrameSize failed: %s", ASUtils::resultString(result));
        return size;
    }
    
    int64_t timestamp(clockid_t clockId, int64_t& framePosition) const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        int64_t timestamp = 0;
        OH_AudioStream_Result result = OH_AudioCapturer_GetTimestamp(m_capturer, clockId, &framePosition, &timestamp);
        _ERROR_IF(result, "OH_AudioCapturer_GetTimestamp failed: %s", ASUtils::resultString(result));
        return timestamp;
    }
    
    int64_t framesRead() const {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        
        int64_t count = 0;
        OH_AudioStream_Result result = OH_AudioCapturer_GetFramesRead(m_capturer, &count);
        _ERROR_IF(result, "OH_AudioCapturer_GetFramesRead failed: %s", ASUtils::resultString(result));
        return count;
    }

private:
    bool m_owner = false;
    OH_AudioCapturer *m_capturer;
};

NAMESPACE_END
