//
// Created on 2024/8/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "AVBuffer.h"
#include "AVSource.h"
#include "AVUtils.h"
#include <cstdint>
#include <multimedia/player_framework/native_avdemuxer.h>
#include <multimedia/player_framework/native_averrors.h>
#include <multimedia/player_framework/native_avsource.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avbuffer.h>

NAMESPACE_DEFAULT

class Demuxer {
public:
    static Demuxer * open(const std::string& filepath, int64_t offset = 0, int64_t size = -1) {
        AVSource *source = AVSource::createWithFile(filepath, offset, size);
        _ERROR_RETURN_IF(source == nullptr, nullptr, "Open source failed");
        
        OH_AVDemuxer *demuxer = OH_AVDemuxer_CreateWithSource(source->value());
        if (demuxer == nullptr) {
            DELETE_TO_NULL(source);
            _ERROR("Create demuxer failed! source: %s", filepath.c_str());
            return nullptr;
        }
        return new Demuxer(demuxer, source);
    }
    
public:
    ~Demuxer() {
        if (m_demuxer) {
            OH_AVDemuxer_Destroy(m_demuxer);
            m_demuxer = nullptr;
        }
        DELETE_TO_NULL(m_source);
        DELETE_TO_NULL(m_buffer);
    }
    
    AVSource& source() {
        return *m_source;
    }
    
    OH_AVErrCode selectVideoTrack() {
        return selectTrack(m_source->videoTrackIndex());
    }
    
    OH_AVErrCode selectAudioTrack() {
        return selectTrack(m_source->audioTrackIndex());
    }
    
    OH_AVErrCode selectTrack(uint32_t index) {
        OH_AVErrCode error = OH_AVDemuxer_SelectTrackByID(m_demuxer, index);
        _ERROR_IF(error, "select track(%d) failed, error: %s", index, AVUtils::errString(error));
        _INFO("select track: %u", index);
        if (index == m_source->audioTrackIndex()) {
            m_audio_selected = true;
        } else if (index == m_source->videoTrackIndex()) {
            m_video_selected = true;
        }
        return error;
    }
    
    OH_AVErrCode unselectTrack(uint32_t index) {
        OH_AVErrCode error = OH_AVDemuxer_UnselectTrackByID(m_demuxer, index);
        _ERROR_IF(error, "unselect track(%d) failed, error: %s", index, AVUtils::errString(error));
        _INFO("unselect track: %u", index);
        if (index == m_source->audioTrackIndex()) {
            m_audio_selected = false;
        } else if (index == m_source->videoTrackIndex()) {
            m_video_selected = false;
        }
        return error;
    }
    
    OH_AVErrCode seekToTime(int64_t ms, OH_AVSeekMode mode = OH_AVSeekMode::SEEK_MODE_CLOSEST_SYNC) {
        OH_AVErrCode error = OH_AVDemuxer_SeekToTime(m_demuxer, ms, mode);
        _ERROR_IF(error, "seek to time(%lld ms) failed, error: %s", ms, AVUtils::errString(error));
        return error;
    }
    
    AVBuffer* readSampleBuffer(uint32_t trackIndex) {
        OH_AVErrCode error = OH_AVDemuxer_ReadSampleBuffer(m_demuxer, trackIndex, m_buffer->value());
        _ERROR_RETURN_IF(error, nullptr, "read sample buffer failed: %s", AVUtils::errString(error));
        m_buffer->updateAttr();
        return m_buffer;
    }
    
    AVBuffer* readVideoSample() {
        return readSampleBuffer(m_source->videoTrackIndex());
    }
    
    AVBuffer* readAudioSample() {
        return readSampleBuffer(m_source->audioTrackIndex());
    }

    void retrieve(std::function<void(AVBuffer *vsample, AVBuffer *asample)> callback) {
        bool videoEnd = !m_source->hasVideoTrack() || !m_video_selected;
        bool audioEnd = !m_source->hasAudioTrack() || !m_audio_selected;
        AVBuffer *vsample = nullptr;
        AVBuffer *asample = nullptr;
        while (!videoEnd || !audioEnd) {
            vsample = nullptr;
            if (!videoEnd) {
                vsample = readVideoSample();
                if (vsample != nullptr) {
                    callback(vsample, nullptr);
                }
                if (vsample == nullptr || vsample->isEOS()) {
                    videoEnd = true;
                }
            }

            asample = nullptr;
            if (!audioEnd) {
                asample = readAudioSample();
                if (asample != nullptr) {
                    callback(nullptr, asample);
                }
                if (asample == nullptr || asample->isEOS()) {
                    audioEnd = true;
                }
            }
        }
    }

    std::string toString() const {
        return m_source->toString();
    }
    
private:
    Demuxer(OH_AVDemuxer *demuxer, AVSource *source) : m_demuxer(demuxer), m_source(source) {
        m_buffer = new AVBuffer(source->maxInputSize());
    }
    
private:
    AVSource *m_source;
    OH_AVDemuxer *m_demuxer;
    
    AVBuffer *m_buffer = nullptr;
    bool m_video_selected = false;
    bool m_audio_selected = false;
};

NAMESPACE_END
