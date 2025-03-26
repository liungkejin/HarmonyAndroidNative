//
// Created on 2024/8/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "harmony/media/Muxer.h"
#include "harmony/media/audio/AudioEncoder.h"
#include "RecordConfig.h"

NAMESPACE_DEFAULT

class Mp4SurfRecorder;
class Mp4RecAudioTrack {
    friend class Mp4SurfRecorder;
    
public:
    void inputBuffer(AVBuffer& buffer) {
        m_encoder.inputBuffer(buffer);
    }
    
    int64_t inputtedDurationUs() {
        return m_encoder.inputtedDurationUs();
    }
    
    int64_t encodedDurationUs() {
        return m_encoder.encodedDurationUs();
    }

private:
    Mp4RecAudioTrack(const char *name, Muxer &muxer) : m_encoder(name), m_muxer(muxer) {}

    OH_AVErrCode prepare(const RecAConfig &config) {
        OH_AVErrCode error = m_encoder.createAAC();
        _ERROR_RETURN_IF(error, error, "createAAC failed");

        AudioFormat format =
            m_encoder.createFormat(config.sampleRate, config.bitrate, config.channelLayout, config.sampleFormat);

        error = m_encoder.configure(format);
        _ERROR_RETURN_IF(error, error, "configure failed");

        error = m_muxer.addTrack(format, m_track_id);
        _ERROR_RETURN_IF(error, error, "addTrack failed");

        m_encoder.setOutputListener([this](AudioEncoder &encoder, AVBuffer &buffer) {
            onGotAudioPacket(encoder, buffer);
        });

        return error;
    }

    OH_AVErrCode start() {
        OH_AVErrCode error = m_encoder.prepare();
        _ERROR_RETURN_IF(error, error, "prepare failed");

        error = m_encoder.start();
        _ERROR_RETURN_IF(error, error, "start failed");
        return error;
    }

    /**
     * 音频不需要等待 EOS 信号, 直接停止
     */
    OH_AVErrCode stop() {
        OH_AVErrCode error = m_encoder.stop();
        m_encoder.release();
        return error;
    }

    void release() { m_encoder.release(); }

private:
    void onGotAudioPacket(AudioEncoder &encoder, AVBuffer &buffer) {
//        if (buffer.isEOS()) {
//            _INFO("Got audio EOS");
//            std::lock_guard<std::mutex> lock(m_mutex);
//            m_condition.notify_all();
//            return;
//        }
        m_muxer.writePacket(m_track_id, buffer);
    }

private:
    Muxer &m_muxer;

    int m_track_id = 0;
    AudioEncoder m_encoder;
};

NAMESPACE_END
