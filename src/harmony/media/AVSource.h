//
// Created on 2024/8/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/utils/FileUtils.h"
#include "AVFormat.h"
#include <cstdint>
#include <cstdio>
#include <multimedia/player_framework/native_avsource.h>

NAMESPACE_DEFAULT

class AVSource {
public:
    static AVSource *createWithURI(char *uri) {
        OH_AVSource *source = OH_AVSource_CreateWithURI(uri);
        _ERROR_RETURN_IF(source == nullptr, nullptr, "create AVSource failed with uri(%s)", uri);

        return new AVSource(uri, source);
    }

    static AVSource *createWithFile(const std::string &filepath, int64_t offset = 0, int64_t size = -1) {
        FILE *file = fopen(filepath.c_str(), "r");
        _WARN_RETURN_IF(!file, nullptr, "open file(%s) failed", filepath.c_str());

        int fd = fileno(file);
        if (size <= 0) {
            size = FileUtils::fileLength(file);
        }
        OH_AVSource *source = OH_AVSource_CreateWithFD(fd, offset, size);
        if (source == nullptr) {
            fclose(file);
            _WARN("create AVSource failed with file(%s)", filepath);
            return nullptr;
        }

        return new AVSource(filepath, source, file);
    }
    
    static int64_t getVideoDurUs(const std::string &filepath, bool checkValid) {
        AVSource * source = createWithFile(filepath);
        if (source == nullptr) {
            return 0;
        }
        bool valid = source->hasAudioTrack() && source->hasVideoTrack();
        int64_t durUs = source->durationUs();
        if (checkValid && !valid) {
            durUs = 0;
        }
        delete source;
        return durUs;
    }

public:
    ~AVSource() {
        if (m_source) {
            OH_AVSource_Destroy(m_source);
            m_source = nullptr;
        }

        if (m_fd) {
            fclose(m_fd);
            m_fd = nullptr;
        }

        DELETE_TO_NULL(m_src_format);
    }

    OH_AVSource *value() { return m_source; }

    const AVFormat &sourceFormat() {
        _FATAL_IF(m_source == nullptr, "Illegal state!");
        if (m_src_format) {
            return *m_src_format;
        }
        OH_AVFormat *format = OH_AVSource_GetSourceFormat(m_source);
        _FATAL_IF(format == nullptr, "get source format failed!");
        m_src_format = new AVFormat(format, true);
        return *m_src_format;
    }

    int trackCount() { return sourceFormat().getInt(OH_MD_KEY_TRACK_COUNT); }

    int64_t durationUs() { return sourceFormat().getLong(OH_MD_KEY_DURATION); }

    int64_t startTimeUs() { return sourceFormat().getLong(OH_MD_KEY_START_TIME); }

    const AVFormat &trackFormat(uint32_t trackIndex) {
        _FATAL_IF(m_source == nullptr, "Illegal state!");
        OH_AVFormat *format = OH_AVSource_GetTrackFormat(m_source, trackIndex);
        _FATAL_IF(format == nullptr, "get track format failed with index(%u)", trackIndex);
        if (m_track_formats.find(trackIndex) == m_track_formats.end()) {
            if (trackIndex == m_video_track_index) {
                m_track_formats[trackIndex] = VideoFormat(format, true);
            } else if (trackIndex == m_audio_track_index) {
                m_track_formats[trackIndex] = AudioFormat(format, true);
            } else {
                m_track_formats[trackIndex] = AVFormat(format, true);
            }
        }

        return m_track_formats[trackIndex];
    }

    bool hasVideoTrack() { return m_video_track_index >= 0; }

    int videoTrackIndex() { return m_video_track_index; }

    VideoFormat &videoTrackFormat() {
        _FATAL_IF(!hasVideoTrack(), "No video track!");
        return (VideoFormat&) trackFormat(m_video_track_index);
    }

    bool hasAudioTrack() { return m_audio_track_index >= 0; }

    int audioTrackIndex() { return m_audio_track_index; }

    AudioFormat &audioTrackFormat() {
        _FATAL_IF(!hasAudioTrack(), "No audio track!");
        return (AudioFormat&)trackFormat(m_audio_track_index);
    }

    int maxInputSize() { return m_max_input_size; }

    std::string toString() {
        std::stringstream ss;
        ss << "{\nurl: " << m_url << "\ntrack count: " << trackCount() << "\ndurationUs: " << durationUs() << "us"
           << "\nmax input size: " << maxInputSize() << "\n";
        if (hasVideoTrack()) {
            const VideoFormat& fmt = videoTrackFormat();
            ss << "video track(" << m_video_track_index << ") " << fmt.dump() << "\n";
        }
        if (hasAudioTrack()) {
            const AudioFormat& fmt = audioTrackFormat();
            ss << "audio track(" << m_audio_track_index << ") " << fmt.dump() << "\n";
        }
        ss << "}";
        return ss.str();
    }

private:
    AVSource(std::string src, OH_AVSource *source, FILE *fd = nullptr) : m_url(src), m_source(source), m_fd(fd) {
        int count = trackCount();
        int fMaxInputSize = 0;
        for (int i = 0; i < count; ++i) {
            const AVFormat &fmt = trackFormat(i);
            int trackType = fmt.getInt(OH_MD_KEY_TRACK_TYPE);
            int maxInputSize = fmt.getInt(OH_MD_KEY_MAX_INPUT_SIZE);
            if (trackType == OH_MediaType::MEDIA_TYPE_VID) {
                m_video_track_index = i;
                if (maxInputSize < 1) {
                    maxInputSize = fmt.getInt(OH_MD_KEY_WIDTH) * fmt.getInt(OH_MD_KEY_HEIGHT) * 3 / 2;
                }
            } else if (trackType == OH_MediaType::MEDIA_TYPE_AUD) {
                m_audio_track_index = i;
                if (maxInputSize < 1) {
                    maxInputSize = fmt.getInt(OH_MD_KEY_AUD_SAMPLE_RATE) * 2;
                }
            } else {
                _WARN("unsupported track type: %d", trackType);
            }
            if (maxInputSize > fMaxInputSize) {
                fMaxInputSize = maxInputSize;
            }
        }
        if (fMaxInputSize > 0) {
            m_max_input_size = fMaxInputSize;
        }
    }

private:
    std::string m_url;
    OH_AVSource *m_source = nullptr;
    FILE *m_fd = nullptr; // For playing local files

    AVFormat *m_src_format = nullptr;
    std::unordered_map<uint32_t, AVFormat> m_track_formats;

    int m_video_track_index = -1;
    int m_audio_track_index = -1;

    int m_max_input_size = 1024 * 1024 * 8;
};
NAMESPACE_END
