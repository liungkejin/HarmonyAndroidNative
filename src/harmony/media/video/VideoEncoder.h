//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "common/utils/Base.h"
#include "harmony/media/AVBuffer.h"
#include "harmony/media/AVCapability.h"
#include "harmony/media/AVFormat.h"
#include "harmony/media/AVUtils.h"

#include <cstdint>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_averrors.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avbuffer.h>

NAMESPACE_DEFAULT

enum VEncState { VE_STATE_NONE, VE_STATE_CREATED, VE_STATE_CONFIGURED, VE_STATE_PREPARED, VE_STATE_ENCODING };

#define VENCODER_INPUT_LISTENER std::function<void(VideoEncoder & encoder, AVBuffer & buffer)>

#define VENCODER_OUTPUT_LISTENER std::function<void(VideoEncoder & encoder, AVBuffer & buffer)>
/**
 * create()
 * configure()
 * prepare()
 * start()
 * flush()
 * stop()
 * reset()
 * release()
 */
class VideoEncoder {
public:
    VideoEncoder(const char *name) : m_name(name) {
        m_name += "-video_encoder";
    }
    
    ~VideoEncoder() { release(); }

    bool valid() {
        if (m_codec) {
            bool valid = false;
            OH_VideoEncoder_IsValid(m_codec, &valid);
            return valid;
        }
        return false;
    }

    OH_AVErrCode createH264() { return create(OH_AVCODEC_MIMETYPE_VIDEO_AVC); }

    OH_AVErrCode createH265() { return create(OH_AVCODEC_MIMETYPE_VIDEO_HEVC); }

    OH_AVErrCode create(const char *mimeType) {
        OH_AVErrCode error = AV_ERR_OK;
        if (m_codec) {
            if (m_mime_type == mimeType && valid()) {
                _WARN("%s: already created! reset it! cur state: %d", m_name, m_state);
                error = OH_VideoEncoder_Reset(m_codec);
                if (error) {
                    _ERROR("%s: OH_VideoEncoder_Reset failed: %s, recreate encoder", m_name,
                           AVUtils::errString(error));
                } else {
                    m_state = VE_STATE_CREATED;
                    return error;
                }
            } else {
                _WARN("%s: Release old encoder, mime type: %s, cur state: %d", m_name, m_mime_type, m_state);
            }
        }

        if (m_codec) {
            OH_VideoEncoder_Destroy(m_codec);
            m_codec = nullptr;
        }
        DELETE_TO_NULL(m_cap);
        m_state = VE_STATE_NONE;

        m_mime_type = mimeType;
        m_cap = new VideoCapability(mimeType, true);
        const char *codecName = m_cap->codecName();
        m_codec = OH_VideoEncoder_CreateByName(codecName);
        if (m_codec == nullptr) {
            _ERROR("%s:OH_VideoEncoder_CreateByName failed, codec name: %s", m_name, codecName);
            DELETE_TO_NULL(m_cap);
            return AV_ERR_UNKNOWN;
        }
        m_state = VE_STATE_CREATED;
        _INFO("%s:created(mime type: %s, codec name: %s)", m_name, mimeType, codecName);
        return error;
    }

    VideoCapability &capability() {
        _FATAL_IF(m_state == VE_STATE_NONE, "%s: Illegal state: %d, pls create first", m_name, m_state);
        return *m_cap;
    }

    VideoFormat createFormat(int width, int height, OH_AVPixelFormat format) {
        _FATAL_IF(m_state != VE_STATE_CREATED, "%s: Illegal state: %d", m_name, m_state);

        VideoFormat videoFormat(m_mime_type.c_str(), width, height);
        videoFormat.setPixelFormat(format);
        return videoFormat;
    }

    OH_AVErrCode configure(VideoFormat &format);

    OH_AVErrCode prepare() {
        _ERROR_RETURN_IF(m_state != VE_STATE_CONFIGURED, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name,
                         m_state);
        OH_AVErrCode error = OH_VideoEncoder_Prepare(m_codec);
        _ERROR_RETURN_IF(error, error, "%s:OH_VideoEncoder_Prepare failed: %s", m_name, AVUtils::errString(error));
        m_state = VE_STATE_PREPARED;
        _INFO("%s: prepared", m_name);
        return error;
    }

    OH_AVErrCode start() {
        _ERROR_RETURN_IF(m_state != VE_STATE_PREPARED, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_VideoEncoder_Start(m_codec);
        _ERROR_RETURN_IF(error, error, "%s:OH_VideoEncoder_Start failed: %s", m_name, AVUtils::errString(error));
        m_state = VE_STATE_ENCODING;
        m_input_start_ptUs = -1;
        m_input_duration_us = 0;
        m_enc_start_ptUs = -1;
        m_enc_duration_us = 0;
        _INFO("%s: started", m_name);
        return error;
    }

    OH_AVErrCode stop() {
        _ERROR_RETURN_IF(m_state != VE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_VideoEncoder_Stop(m_codec);
        _ERROR_RETURN_IF(error, error, "%s:OH_VideoEncoder_Stop failed: %s", m_name, AVUtils::errString(error));
        m_state = VE_STATE_PREPARED;
        _INFO("%s: stopped, inputted dur us: %lld, encoded dur us: %lld", m_name, inputtedDurationUs(), encodedDurationUs());
        return error;
    }

    bool isStarted() { return m_state == VE_STATE_ENCODING; }

    int64_t inputtedDurationUs() {
        if (m_input_duration_us > 0 && m_frame_duration_us > 0) {
            return m_input_duration_us + m_frame_duration_us;
        }
        return m_input_duration_us;
    }

    int64_t encodedDurationUs() {
        if (m_enc_duration_us > 0 && m_frame_duration_us > 0) {
            return m_enc_duration_us + m_frame_duration_us;
        }
        return m_enc_duration_us;
    }

    OH_AVErrCode flush() {
        _ERROR_RETURN_IF(m_state != VE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_VideoEncoder_Flush(m_codec);
        _ERROR_RETURN_IF(error, error, "%s:OH_VideoEncoder_Flush failed: %s", m_name, AVUtils::errString(error));
        m_state = VE_STATE_PREPARED;
        _INFO("%s: flushed", m_name);
        return error;
    }

    virtual OH_AVErrCode reset() {
        _ERROR_RETURN_IF(m_state == VE_STATE_NONE, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_VideoEncoder_Reset(m_codec);
        _ERROR_RETURN_IF(error, error, "%s:OH_VideoEncoder_Reset failed: %s", m_name, AVUtils::errString(error));
        m_state = VE_STATE_CREATED;
        _INFO("%s: reset", m_name);
        return error;
    }

    virtual void release() {
        if (m_state == VE_STATE_NONE) {
            return;
        }
        if (m_state == VE_STATE_ENCODING) {
            stop();
        }
        if (m_codec) {
            OH_VideoEncoder_Destroy(m_codec);
            m_codec = nullptr;
        }
        DELETE_TO_NULL(m_cap);
        m_state = VE_STATE_NONE;
        setOutputListener(nullptr);
        _INFO("%s: released", m_name);
    }

    OH_AVErrCode notifyEOS() {
        _ERROR_RETURN_IF(m_state != VE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_VideoEncoder_NotifyEndOfStream(m_codec);
        _ERROR_RETURN_IF(error, error, "%s:OH_VideoEncoder_NotifyEndOfStream failed: %s", m_name,
                         AVUtils::errString(error));
        return error;
    }

    VideoFormat getInputFormat() {
        _FATAL_IF(m_state == VE_STATE_NONE || m_state == VE_STATE_CREATED, "%s: Illegal state: %d", m_name, m_state);
        OH_AVFormat *format = OH_VideoEncoder_GetInputDescription(m_codec);
        return VideoFormat(format, true);
    }

    VideoFormat getOutputFormat() {
        OH_AVFormat *format = OH_VideoEncoder_GetOutputDescription(m_codec);
        return VideoFormat(format, true);
    }

    OH_AVErrCode requestIFrame() {
        VideoFormat format;
        format.setValue(OH_MD_KEY_REQUEST_I_FRAME, true);
        return request(format);
    }

public:
    void setInputListener(VENCODER_INPUT_LISTENER listener) { m_input_listener = listener; }

    void setOutputListener(VENCODER_OUTPUT_LISTENER callback) { m_output_listener = callback; }

    virtual void onError(int error) { _ERROR("%s: onError: %d(%s)", m_name, error, AVUtils::errString(error)); }

    virtual void onStreamChanged(VideoFormat &format) { _INFO("%s: onStreamChanged: %s", m_name, format.dump()); }

    virtual void onNeedInputBuffer(uint32_t index, AVBuffer &buffer) {
        //        _INFO("%s: onNeedInput: %d", m_name, index);
        if (m_input_listener) {
            m_input_listener(*this, buffer);
        }
        if (m_input_start_ptUs < 0) {
            m_input_start_ptUs = buffer.ptUs();
        }
        m_input_duration_us = buffer.ptUs() - m_input_start_ptUs;

        pushInputBuffer(index);
    }

    virtual void onNewOutputBuffer(uint32_t index, AVBuffer &buffer) {
        //        _INFO("%s: onOutput: %d, buffer(ptUs: %ld, size: %d, flags: %u)", m_name, index, buffer.ptUs(),
        //        buffer.size(), buffer.flags());

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

        freeOutputBuffer(index);
    }

    virtual void onNeedInputParameter(uint32_t index, VideoFormat &format) {
        format.setEncoderQPMin(20);
        format.setEncoderQPMax(30);
        _INFO("%s: onNeedParam: %d, format: %s", m_name.c_str(), index, format.dump());
        pushInputParameter(index);
    }

protected:
    virtual OH_AVErrCode onConfigure(VideoFormat &format);

    virtual OH_AVErrCode onConfigured(const VideoFormat &format) { return OH_AVErrCode::AV_ERR_OK; }

protected:
    OH_AVErrCode freeOutputData(uint32_t index) { return OH_VideoEncoder_FreeOutputData(m_codec, index); }

    OH_AVErrCode pushInputData(uint32_t index, OH_AVCodecBufferAttr attr) {
        return OH_VideoEncoder_PushInputData(m_codec, index, attr);
    }

    OH_AVErrCode pushInputBuffer(uint32_t index) { return OH_VideoEncoder_PushInputBuffer(m_codec, index); }

    OH_AVErrCode pushInputParameter(uint32_t index) { return OH_VideoEncoder_PushInputParameter(m_codec, index); }

    OH_AVErrCode freeOutputBuffer(uint32_t index) { return OH_VideoEncoder_FreeOutputBuffer(m_codec, index); }

    /**
     * 在编码过程中动态配置编码器参数
     * @param format
     */
    OH_AVErrCode request(VideoFormat &format) {
        _ERROR_RETURN_IF(m_state != VE_STATE_ENCODING, AV_ERR_INVALID_STATE, "%s: Illegal state: %d", m_name, m_state);
        OH_AVErrCode error = OH_VideoEncoder_SetParameter(m_codec, format.value());
        _ERROR_RETURN_IF(error, error, "OH_VideoEncoder_SetParameter failed: %s", AVUtils::errString(error));
        return error;
    }

protected:
    std::string m_name;
    std::string m_mime_type = "";

    int m_width = 0;
    int m_height = 0;
    int m_pixel_format = 0;
    double m_frame_rate = 30;
    int64_t m_frame_duration_us = 0;

    int64_t m_input_start_ptUs = -1;
    int64_t m_input_duration_us = 0;

    int64_t m_enc_start_ptUs = -1;
    int64_t m_enc_duration_us = 0;

    OH_AVCodec *m_codec = nullptr;
    VideoCapability *m_cap = nullptr;
    VEncState m_state = VE_STATE_NONE;

    VENCODER_INPUT_LISTENER m_input_listener;
    VENCODER_OUTPUT_LISTENER m_output_listener;
};

NAMESPACE_END
