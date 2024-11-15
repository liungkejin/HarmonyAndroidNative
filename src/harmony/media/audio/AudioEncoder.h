//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Common.h"
#include "harmony/media/AVBuffer.h"
#include "harmony/media/AVCapability.h"
#include "harmony/media/AVUtils.h"
#include "AudioUtils.h"
#include "harmony/media/AVFormat.h"
#include <multimedia/player_framework/native_avcodec_audiocodec.h>
#include <multimedia/native_audio_channel_layout.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avbuffer.h>
#include <queue>

NAMESPACE_DEFAULT

enum AEncState { AE_STATE_NONE, AE_STATE_CREATED, AE_STATE_CONFIGURED, AE_STATE_PREPARED, AE_STATE_ENCODING };

class AudioEncoder {
public:
    explicit AudioEncoder(const char *name) : m_name(name) { m_name += "-audio_encoder"; }

    ~AudioEncoder() { release(); }

    bool valid() {
        if (m_codec) {
            bool valid = false;
            OH_AudioCodec_IsValid(m_codec, &valid);
            return valid;
        }
        return false;
    }

    OH_AVErrCode createAAC() { return create(OH_AVCODEC_MIMETYPE_AUDIO_AAC); }

    OH_AVErrCode create(const char *mimeType) {
        OH_AVErrCode error = AV_ERR_OK;
        if (m_codec) {
            if (m_mime_type == mimeType && valid()) {
                _WARN("%s: already created! reset it! cur state: %d", m_name, m_state);
                error = OH_AudioCodec_Reset(m_codec);
                if (error) {
                    _ERROR("%s: OH_AudioCodec_Reset failed: %s, recreate encoder", m_name, AVUtils::errString(error));
                } else {
                    m_state = AE_STATE_CREATED;
                    return error;
                }
            } else {
                _WARN("%s: Release old encoder, mime type: %s, cur state: %d", m_name, m_mime_type, m_state);
            }
        }

        if (m_codec) {
            OH_AudioCodec_Destroy(m_codec);
            m_codec = nullptr;
        }
        DELETE_TO_NULL(m_cap);
        m_state = AE_STATE_NONE;

        m_mime_type = mimeType;
        m_cap = new AudioCapability(mimeType, true);
        const char *codecName = m_cap->codecName();
        m_codec = OH_AudioCodec_CreateByName(codecName);
        if (m_codec == nullptr) {
            _ERROR("%s: OH_AudioCodec_CreateByName failed, codec name: %s", m_name, codecName);
            DELETE_TO_NULL(m_cap);
            return AV_ERR_UNKNOWN;
        }
        m_state = AE_STATE_CREATED;
        _INFO("%s: created(mime type: %s, codec name: %s)", m_name, mimeType, codecName);
        return error;
    }

    AudioCapability &capability() {
        _FATAL_IF(m_state == AE_STATE_NONE, "%s: Illegal state: %d, pls create first", m_name, m_state);
        return *m_cap;
    }

    AudioFormat createFormat(int sampleRate, int64_t bitrate, OH_AudioChannelLayout channelLayout,
                             OH_BitsPerSample sampleFormat) {
        _FATAL_IF(m_state != AE_STATE_CREATED, "%s: Illegal state: %d", m_name, m_state);
        int channels = AudioUtils::layoutChannelCount(channelLayout);
        AudioFormat format(m_mime_type.c_str(), sampleRate, channels);
        format.setBitrate(bitrate);
        format.setChannelLayout(channelLayout);
        format.setSampleFormat(sampleFormat);

        if (m_mime_type == OH_AVCODEC_MIMETYPE_AUDIO_AAC) {
            float timePerFrame = 0.02;
            int maxInputSize = (int) ((float)sampleRate * timePerFrame * (float)channels * (float)AudioUtils::sampleBytes(sampleFormat));
            format.setMaxInputSize(maxInputSize);
        }

        return format;
    }

    OH_AVErrCode configure(AudioFormat &format);

    OH_AVErrCode prepare() {
        _ERROR_RETURN_IF(m_state != AE_STATE_CONFIGURED, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name,
                         m_state);
        OH_AVErrCode error = OH_AudioCodec_Prepare(m_codec);
        _ERROR_RETURN_IF(error, error, "%s: OH_AudioCodec_Prepare failed: %s", m_name, AVUtils::errString(error));
        m_state = AE_STATE_PREPARED;
        _INFO("%s: configured", m_name);
        return error;
    }

    OH_AVErrCode start() {
        _ERROR_RETURN_IF(m_state != AE_STATE_PREPARED, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_AudioCodec_Start(m_codec);
        _ERROR_RETURN_IF(error, error, "%s: OH_AudioCodec_Start failed: %s", m_name, AVUtils::errString(error));
        m_state = AE_STATE_ENCODING;
        m_input_sum_bytes = 0;
        m_input_duration_us = 0;
        m_enc_start_ptUs = -1;
        m_enc_duration_us = 0;
        _INFO("%s: started", m_name);

        resetInputQueue();
        return error;
    }

    OH_AVErrCode stop() {
        _ERROR_RETURN_IF(m_state != AE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);

        resetInputQueue();
        
        OH_AVErrCode error = OH_AudioCodec_Stop(m_codec);
        _ERROR_RETURN_IF(error, error, "%s: OH_AudioCodec_Stop failed: %s", m_name, AVUtils::errString(error));
        m_state = AE_STATE_PREPARED;
        _INFO("%s: stopped, inputted dur us: %lld, encoded dur us: %lld", m_name, inputtedDurationUs(), encodedDurationUs());
        return error;
    }

    OH_AVErrCode flush() {
        _ERROR_RETURN_IF(m_state != AE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        
        resetInputQueue();
        
        OH_AVErrCode error = OH_AudioCodec_Flush(m_codec);
        _ERROR_RETURN_IF(error, error, "%s: OH_AudioCodec_Flush failed: %s", m_name, AVUtils::errString(error));

        m_state = AE_STATE_PREPARED;
        _INFO("%s: flushed", m_name);
        return error;
    }

    virtual OH_AVErrCode reset() {
        _ERROR_RETURN_IF(m_state == AE_STATE_NONE, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);

        resetInputQueue();
        
        OH_AVErrCode error = OH_AudioCodec_Reset(m_codec);
        _ERROR_RETURN_IF(error, error, "%s: OH_AudioCodec_Reset failed: %s", m_name, AVUtils::errString(error));

        m_state = AE_STATE_CREATED;
        _INFO("%s: reset", m_name);
        return error;
    }

    void release() {
        if (m_state == AE_STATE_NONE) {
            return;
        }

        if (m_state == AE_STATE_ENCODING) {
            stop();
        }

        resetInputQueue();
        if (m_codec) {
            OH_AudioCodec_Destroy(m_codec);
            m_codec = nullptr;
        }
        DELETE_TO_NULL(m_cap);
        setOutputListener(nullptr);

        m_state = AE_STATE_NONE;
        _INFO("%s: audio encoder released", m_name);
    }

public:
    AudioFormat getOutputFormat() {
        OH_AVFormat *format = OH_AudioCodec_GetOutputDescription(m_codec);
        return {format, true};
    }

    bool isStarted() { return m_state == AE_STATE_ENCODING; }

    int64_t inputtedDurationUs() const { return m_input_duration_us; }

    int64_t encodedDurationUs() const { return m_enc_duration_us; }

    void inputBuffer(AVBuffer &buffer) {
        _WARN_RETURN_IF(m_state != AE_STATE_ENCODING, void(), "%s: Illegal state: %d", m_name, m_state);
        std::lock_guard<std::mutex> lock(m_queue_mutex);

        m_input_buffers.push(buffer);
        m_queue_cv.notify_all();
    }

//    void notifyEOS() {
//        AVBuffer eosBuffer(1);
//        eosBuffer.setAttr(0, 0, 0, AVCODEC_BUFFER_FLAGS_EOS);
//        inputBuffer(eosBuffer);
//    }

public:
    void setOutputListener(const std::function<void(AudioEncoder &, AVBuffer &)>& callback) { m_output_listener = callback; }

    virtual void onError(int error) { _ERROR("%s: onError: %d(%s)", m_name, error, AVUtils::errString(error)); }

    virtual void onStreamChanged(AudioFormat &format) { _INFO("%s: onStreamChanged: %s", m_name, format.dump()); }

    virtual void onNeedInputBuffer(uint32_t index, AVBuffer &buffer) {
        {
            // 等待输入
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            if (m_input_buffers.empty() && m_state == AE_STATE_ENCODING) {
                m_queue_cv.wait_for(lock, std::chrono::milliseconds(1000));
            }

            if (m_state != AE_STATE_ENCODING) {
                _WARN("state incorrect: %d", m_state);
                buffer.setAttr(0, m_input_frame_bytes, 0, 0);
                memset(buffer.addr(), 0, m_input_frame_bytes);
                pushInputBuffer(index, true);
                return;
            }

            if (m_input_buffers.empty()) {
                _WARN("No input available");
                buffer.setAttr(0, m_input_frame_bytes, 0, 0);
                memset(buffer.addr(), 0, m_input_frame_bytes);
                pushInputBuffer(index, true);
                return;
            }

            AVBuffer input = m_input_buffers.front();
            m_input_buffers.pop();

            if (input.isEOS()) {
                buffer.setAttr(0, 0, 0, AVCODEC_BUFFER_FLAGS_EOS);
                pushInputBuffer(index, false);
                return;
            }

            memcpy(buffer.addr(), input.addr(), input.size());
            buffer.setAttr(0, input.size(), 0, AVCODEC_BUFFER_FLAGS_NONE);
        }

        m_input_sum_bytes += buffer.size();
        m_input_duration_us = AudioUtils::calPCMDurationUs(
            m_input_sum_bytes, m_sample_rate, m_channel_count, m_sample_format);
        m_input_frame_bytes = buffer.size();

        pushInputBuffer(index, false);
    }

    virtual void onNewOutputBuffer(uint32_t index, AVBuffer &buffer) {
        //        _INFO("%s: onOutput: %d, buffer(ptUs: %ld, size: %d, flags: %u)", m_name, index, buffer.ptUs(),
        //        buffer.size(),
        //              buffer.flags());

        if (m_enc_start_ptUs < 0) {
            m_enc_start_ptUs = buffer.ptUs();
        }
        long durUs = buffer.ptUs() - m_enc_start_ptUs;
        if (durUs > m_enc_duration_us) {
            m_enc_duration_us = durUs;
        }

        // 回调数据给应用层，由应用层来处理数据
        if (m_output_listener) {
            m_output_listener(*this, buffer);
        }

        OH_AVErrCode error = OH_AudioCodec_FreeOutputBuffer(m_codec, index);
        _ERROR_IF(error, "OH_AudioCodec_FreeOutputBuffer: error: %s", AVUtils::errString(error));
    }

protected:
    virtual OH_AVErrCode onConfigure(AudioFormat &format);

    virtual OH_AVErrCode onConfigured(const AudioFormat &format) { return OH_AVErrCode::AV_ERR_OK; }

    void pushInputBuffer(uint32_t index, bool skip) {
        OH_AVErrCode error = OH_AudioCodec_PushInputBuffer(m_codec, index);
        if (skip) {
            _WARN_IF(error, "skip input: %d, error: %s", index, AVUtils::errString(error));
        } else {
            _ERROR_IF(error, "push input buffer error: %s", AVUtils::errString(error));
        }
    }
    /**
     * 在编码过程中动态配置编码器参数
     * @param format
     */
    OH_AVErrCode request(AudioFormat &format) {
        _ERROR_RETURN_IF(m_state != AE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_AudioCodec_SetParameter(m_codec, format.value());
        _ERROR_RETURN_IF(error, error, "OH_AudioCodec_SetParameter failed: %s", AVUtils::errString(error));
        return error;
    }

    void resetInputQueue() {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if (!m_input_buffers.empty()) {
            _WARN("Clearing input buffer queue size: %d", m_input_buffers.size());
        }
        while (!m_input_buffers.empty())
            m_input_buffers.pop();
        m_queue_cv.notify_all();
    }

protected:
    std::string m_name;
    std::string m_mime_type;

    int m_sample_rate = 0;
    int m_channel_count = 0;
    int m_bit_rate = 0;
    OH_BitsPerSample m_sample_format = SAMPLE_S16LE;

    int64_t m_input_sum_bytes = 0;
    int64_t m_input_frame_bytes = 0;
    int64_t m_input_duration_us = 0;

    int64_t m_enc_start_ptUs = -1;
    int64_t m_enc_duration_us = 0;

    OH_AVCodec *m_codec = nullptr;
    AudioCapability *m_cap = nullptr;
    AEncState m_state = AE_STATE_NONE;

    std::mutex m_queue_mutex;
    std::condition_variable m_queue_cv;
    std::queue<AVBuffer> m_input_buffers;

    std::function<void(AudioEncoder &, AVBuffer &)> m_output_listener;
};

NAMESPACE_END
