//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Common.h"
#include "Namespace.h"
#include "harmony/media/Muxer.h"
#include "harmony/media/video/VSurfaceEncoder.h"
#include "RecordConfig.h"
#include "Mp4RecSurfVideoTrack.h"
#include "Mp4RecAudioTrack.h"
#include <multimedia/player_framework/native_averrors.h>
#include <cstdint>
#include <thread>

NAMESPACE_DEFAULT

class Mp4Recorder {
public:
    Mp4Recorder(const char *name) : m_name(name) {}

public:
    OH_AVErrCode prepare(const RecordConfig &config) {
        if (!m_muxer.create(config.savePath)) {
            return AV_ERR_UNKNOWN;
        }
        m_config = config;
        OH_AVErrCode error = onPrepare(config);
        if (error != AV_ERR_OK) {
            return error;
        }
        _INFO("Mp4Recorder(%s) prepared successfully, config: %s", m_name, config.dump());
        return AV_ERR_OK;
    }

    OH_AVErrCode start() {
        OH_AVErrCode error = m_muxer.start();
        if (error) {
            return error;
        }

        error = onStart();
        if (error) {
            return error;
        }
        _INFO("Mp4Recorder(%s) started successfully", m_name);
        return AV_ERR_OK;
    }

    bool isStarted() { return m_muxer.isStarted(); }

    virtual int64_t inputtedDurationUs() = 0;

    virtual int64_t encodedDurationUs() = 0;

    OH_AVErrCode stop() {
        _INFO("stop() called");
        OH_AVErrCode error = onStop();
        m_muxer.stop();
        _ERROR_RETURN_IF(error, error, "Mp4Recorder(%s) stop failed", m_name);
        _INFO("Mp4Recorder(%s) stopped successfully，inputted durationUs: %ld, encoded durationUs: %ld", m_name,
              inputtedDurationUs(), encodedDurationUs());
        return AV_ERR_OK;
    }

    void release(int code = 0) {
        _INFO("release() called, code: %d", AVUtils::errString(code));
        onRelease();
        m_muxer.stop();
    }

public:
    const RecordConfig &config() const { return m_config; }

protected:
    virtual OH_AVErrCode onPrepare(const RecordConfig &config) = 0;

    virtual OH_AVErrCode onStart() = 0;

    virtual OH_AVErrCode onStop() = 0;

    virtual void onRelease() = 0;

protected:
    const std::string m_name;

    RecordConfig m_config;

    Muxer m_muxer;
};

class Mp4SurfRecorder : public Mp4Recorder {
public:
    Mp4SurfRecorder(const char *name) : Mp4Recorder(name), m_video_track(name, m_muxer), m_audio_track(name, m_muxer) {}
    ~Mp4SurfRecorder() { release(); }

public:
    Mp4RecSurfVideoTrack &videoTrack() { return m_video_track; }

    Mp4RecAudioTrack &audioTrack() { return m_audio_track; }

    int64_t inputtedDurationUs() override {
        int64_t adurUs = m_audio_track.inputtedDurationUs();
        int64_t vdurUs = m_video_track.inputtedDurationUs();
        return std::max(adurUs, vdurUs);
    }

    int64_t encodedDurationUs() override {
        int64_t adurUs = m_audio_track.encodedDurationUs();
        int64_t vdurUs = m_video_track.encodedDurationUs();
        return std::max(adurUs, vdurUs);
    }

protected:
    OH_AVErrCode onPrepare(const RecordConfig &config) override {
        OH_AVErrCode error = m_video_track.prepare(config.vConfig);
        _ERROR_RETURN(error, error);

        error = m_audio_track.prepare(config.aConfig);

        return error;
    }

    OH_AVErrCode onStart() override {
        OH_AVErrCode error = m_video_track.start();
        _ERROR_RETURN(error, error);

        error = m_audio_track.start();
        return error;
    }

    OH_AVErrCode onStop() override {
        m_video_track.notifyEOS();
        OH_AVErrCode error = m_video_track.stop();
        _INFO("video track stop result: %s", AVUtils::errString(error));
        int64_t deltaUs = m_video_track.encodedDurationUs() - m_audio_track.encodedDurationUs();
        if (deltaUs > 0) {
            // 稍等等一下
            int64_t waitMs = std::min(200L, deltaUs/1000L); // 最多等200ms
            waitMs = std::max(waitMs, 40L); // 最少等40ms
            _WARN("Audio track is behind video track by %lld ms, waiting audio: %lld ms", deltaUs/1000L, waitMs);
            int64_t startMs = TimeUtils::nowMs();
            while (waitMs > 0 && deltaUs > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                deltaUs = m_video_track.encodedDurationUs() - m_audio_track.encodedDurationUs();
                waitMs -= 10;
            }
            _WARN("waiting audio track finish cost: %lld ms", (TimeUtils::nowMs()-startMs));
        }
        error = m_audio_track.stop();
        _INFO("audio track stop result: %s", AVUtils::errString(error));
        return error;
    }

    void onRelease() override {
        m_video_track.release();
        m_audio_track.release();
    }

private:
    Mp4RecSurfVideoTrack m_video_track;
    Mp4RecAudioTrack m_audio_track;
};

NAMESPACE_END
