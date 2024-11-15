//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "AVBuffer.h"
#include "AVFormat.h"
#include "AVUtils.h"
#include "harmony/media/audio/AudioUtils.h"
#include <cstdint>
#include <cstdio>
#include <multimedia/player_framework/native_averrors.h>
#include <multimedia/player_framework/native_avmuxer.h>

NAMESPACE_DEFAULT

class Muxer {
public:
    Muxer() {}

    ~Muxer() { stop(); }

public:
    bool create(const std::string &filepath, OH_AVOutputFormat format = AV_OUTPUT_FORMAT_MPEG_4) {
        _INFO("file path: %s", filepath.c_str());
        m_file = fopen(filepath.c_str(), "w+");
        _ERROR_RETURN_IF(!m_file, false, "Couldn't open file(%s)", filepath.c_str());
        int fd = fileno(m_file);
        if (!create(fd, format)) {
            fclose(m_file);
            m_file = nullptr;
            return false;
        }
        return true;
    }

    /**
     * 如果单独使用这个函数，需要自己控制 fd 的关闭
     */
    bool create(int fd, OH_AVOutputFormat format = AV_OUTPUT_FORMAT_MPEG_4) {
        std::lock_guard<std::mutex> locker(m_lck_mutex);

        m_muxer = OH_AVMuxer_Create(fd, format);
        _ERROR_RETURN_IF(!m_muxer, false, "OH_AVMuxer_Create failed");
        return true;
    }

    bool setRotation(int rotation) {
        std::lock_guard<std::mutex> locker(m_lck_mutex);
        _ERROR_RETURN_IF(!m_muxer, false, "setRotation failed, illegal state, muxer not created");
        _ERROR_RETURN_IF(m_start_flag, false, "setRotation failed, illegal state, muxer already started");
        return OH_AVMuxer_SetRotation(m_muxer, rotation) == AV_ERR_OK;
    }

    OH_AVErrCode addTrack(const AVFormat &format, int &outTrackId) {
        std::lock_guard<std::mutex> locker(m_lck_mutex);
        _ERROR_RETURN_IF(!m_muxer, AV_ERR_OPERATE_NOT_PERMIT, "addTrack failed, illegal state, muxer not created");
        _ERROR_RETURN_IF(m_start_flag, AV_ERR_OPERATE_NOT_PERMIT,
                         "addTrack failed, illegal state, muxer already started");

        // 这里之所以需要复制一份，是因为遇到了一个内存错误，如果 format 提前释放，在muxer destroy的时候会报内存错误
        AVFormat cpyFormat(format);
        outTrackId = -1;
        OH_AVErrCode error = OH_AVMuxer_AddTrack(m_muxer, &outTrackId, cpyFormat.value());
        _ERROR_RETURN_IF(error || outTrackId < 0, error, "add track (%s) failed: %s", cpyFormat.dump(),
                         AVUtils::errString(error));
//        m_track_fmt_map.insert(std::make_pair(outTrackId, cpyFormat));
        
        m_sum_tracks += 1;
        if (format.isVideoTrack()) {
            m_video_track_id = outTrackId;
        } else if (format.isAudioTrack()) {
            m_audio_track_id = outTrackId;
        }
        _INFO("addTrack succeeded! sumTracks(%d), trackIndex(%d): %s", m_sum_tracks, outTrackId, cpyFormat.dump());
        return error;
    }

    int trackCount() const { return m_sum_tracks; }

    int videoTrackId() const { return m_video_track_id; }
    
    bool hasVideoTrack() const { return m_video_track_id >= 0;}
    
    int64_t videoDurationUs() const { return m_video_duration_us; }

    int audioTrackId() const { return m_audio_track_id; }
    
    bool hasAudioTrack() const { return m_audio_track_id >= 0;}
    
    int64_t audioDurationUs() const { return m_audio_duration_us; }

    OH_AVErrCode start() {
        std::lock_guard<std::mutex> locker(m_lck_mutex);
        _ERROR_RETURN_IF(!m_muxer, AV_ERR_OPERATE_NOT_PERMIT, "start failed, illegal state, muxer not created");
        _ERROR_RETURN_IF(m_start_flag, AV_ERR_OPERATE_NOT_PERMIT, "start failed, illegal state, muxer already started");

        OH_AVErrCode error = OH_AVMuxer_Start(m_muxer);
        _ERROR_RETURN_IF(error, error, "start muxer failed: %s", AVUtils::errString(error));
        _INFO("start muxer succeeded");
        m_start_flag = true;
        return error;
    }

    inline bool isStarted() const { return m_start_flag; }
    
    OH_AVErrCode writeVideoPacket(AVBuffer &buffer) {
        return writePacket(m_video_track_id, buffer);
    }
    
    OH_AVErrCode writeAudioPacket(AVBuffer &buffer) {
        return writePacket(m_audio_track_id, buffer);
    }

    OH_AVErrCode writePacket(int trackId, AVBuffer &buffer) {
        std::lock_guard<std::mutex> locker(m_lck_mutex);
        _ERROR_RETURN_IF(!m_muxer, AV_ERR_OPERATE_NOT_PERMIT, "writePacket failed, illegal state, muxer not created");
        _ERROR_RETURN_IF(!m_start_flag, AV_ERR_OPERATE_NOT_PERMIT,
                         "writePacket failed, illegal state, muxer not started");
        _ERROR_RETURN_IF(trackId < 0, AV_ERR_INVALID_VAL, "writePacket failed, illegal trackId");

        OH_AVErrCode error = OH_AVMuxer_WriteSampleBuffer(m_muxer, (uint32_t)trackId, buffer.value());
        _ERROR_RETURN_IF(error, error, "writePacket failed: %s", AVUtils::errString(error));
        if (trackId == m_video_track_id) {
            if (m_video_duration_us < buffer.ptUs()) {
                m_video_duration_us = buffer.ptUs();
            }
        } else if (trackId == m_audio_duration_us) {
            if (m_audio_duration_us < buffer.ptUs()) {
                m_audio_duration_us = buffer.ptUs();
            }
        }
        return error;
    }

    OH_AVErrCode stop() {
        std::lock_guard<std::mutex> locker(m_lck_mutex);
        if (m_muxer == nullptr) {
            return AV_ERR_OK;
        }

        OH_AVErrCode error;
        if (m_start_flag) {
            error = OH_AVMuxer_Stop(m_muxer);
            _WARN_IF(error, "stop muxer failed: %s", AVUtils::errString(error));
            m_start_flag = false;
        }
        if (m_file) {
            fclose(m_file);
            m_file = nullptr;
        }
        m_sum_tracks = 0;
        error = OH_AVMuxer_Destroy(m_muxer);
        m_muxer = nullptr;
        _WARN_IF(error, "destroy muxer failed: %s", AVUtils::errString(error));
        _INFO_IF(!error, "stop muxer succeeded");
        return error;
    }

private:
    FILE *m_file = nullptr;

    OH_AVMuxer *m_muxer = nullptr;
    bool m_start_flag = false;
    
//    std::unordered_map<int, AVFormat> m_track_fmt_map;

    int m_sum_tracks = 0;
    int m_video_track_id = -1;
    int64_t m_video_duration_us = 0;
    
    int m_audio_track_id = -1;
    int64_t m_audio_duration_us = 0;

    std::mutex m_lck_mutex;
};
NAMESPACE_END
