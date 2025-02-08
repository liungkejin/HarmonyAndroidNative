//
// Created on 2024/8/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "RecordConfig.h"
#include "harmony/media/Muxer.h"
#include "harmony/media/video/VSurfaceEncoder.h"

NAMESPACE_DEFAULT

class Mp4SurfRecorder;
class Mp4RecSurfVideoTrack {
    friend class Mp4SurfRecorder;

public:
    NNativeWindow *getSurface() { return m_encoder.getSurface(); }

    int64_t inputtedDurationUs() { return m_encoder.inputtedDurationUs(); }

    int64_t encodedDurationUs() { return m_encoder.encodedDurationUs(); }

    void pushTimestampUs(int64_t ptUs) { m_encoder.pushTimestampUs(ptUs); }

private:
    Mp4RecSurfVideoTrack(const char *name, Muxer &muxer) : m_encoder(name), m_muxer(muxer) {}

    OH_AVErrCode prepare(const RecVConfig &config) {
        OH_AVErrCode error = m_encoder.createH264();
        _ERROR_RETURN(error, error);

        VideoFormat format = m_encoder.createFormat(config.width, config.height, config.pixelFormat);
        format.setFrameRate(config.frameRate);
        format.setIFrameInterval(config.iFrameInterval);
        format.setBitrate(config.bitRate);
        format.setEncodeBitrateMode(config.bitrateMode);

        error = m_encoder.configure(format);
        _ERROR_RETURN(error, error);

        error = m_muxer.addTrack(format, m_track_id);
        _ERROR_RETURN(error, error);

        m_eof_received = false;
        m_encoder.setOutputListener(
            [this](VideoEncoder &encoder, AVBuffer &buffer) { onGotVideoPacket(encoder, buffer); });
        return error;
    }

    OH_AVErrCode start() {
        OH_AVErrCode error = m_encoder.prepare();
        _ERROR_RETURN(error, error);

        error = m_encoder.start();
        _ERROR_RETURN(error, error);
        return error;
    }

    OH_AVErrCode notifyEOS() {
        OH_AVErrCode error = m_encoder.notifyEOS();
        if (error != AV_ERR_OK) {
            m_eof_received = true;
        }
        return error;
    }

    OH_AVErrCode stop() {
        // 等待 EOS 信号
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_eof_received) {
            std::cv_status status = m_condition.wait_for(lock, std::chrono::milliseconds(1000));
            if (status == std::cv_status::timeout) {
                _ERROR("wait video EOS timeout!");
            }
        }

        OH_AVErrCode error = m_encoder.stop();
        m_encoder.release();
        _INFO_IF(!error, "video encoder stopped successfully");
        return error;
    }

    void release() { 
        m_encoder.release();
    }

private:
    void onGotVideoPacket(VideoEncoder &encoder, AVBuffer &buffer) {
        if (buffer.isEOS()) {
            onGotVideoEOS();
            return;
        }
        m_muxer.writePacket(m_track_id, buffer);
    }

    void onGotVideoEOS() {
        _INFO("Got video EOS");
        std::lock_guard<std::mutex> lock(m_mutex);
        m_eof_received = true;
        m_condition.notify_all();
    }


private:
    Muxer &m_muxer;

    int m_track_id = 0;
    VSurfaceEncoder m_encoder;

    bool m_eof_received = false;
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

NAMESPACE_END
