//
// Created on 2024/8/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "harmony/media/audio/AudioUtils.h"
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avformat.h>

NAMESPACE_DEFAULT

struct RecVConfig {
    RecVConfig() {}

    RecVConfig(int w, int h, int rot) {
        width = w;
        height = h;
        bitRate = (int64_t)(1024 * 1024 * 6 * ((double)w * (double)h / (720.0 * 960.0)));
        rotation = rot;
    }

    RecVConfig(const RecVConfig &config) { memcpy(this, &config, sizeof(config)); }

    RecVConfig &operator=(const RecVConfig &config) {
        memcpy(this, &config, sizeof(config));
        return *this;
    }

    std::string dump() const {
        std::stringstream ss;
        ss << "{size: " << width << "x" << height << ", rotation: " << rotation << ", pixelFormat: " << pixelFormat
           << ", frameRate: " << frameRate << ", bitRate: " << bitRate << ", iFrameInterval: " << iFrameInterval
           << ", bitrateMode: " << bitrateMode << "}";
        return ss.str();
    }

    int width = 0;
    int height = 0;
    int rotation = 0;

    OH_AVPixelFormat pixelFormat = AV_PIXEL_FORMAT_SURFACE_FORMAT;
    int frameRate = 30;
    int64_t bitRate = 1024 * 1024 * 8;
    int iFrameInterval = 1000;
    OH_VideoEncodeBitrateMode bitrateMode = VBR;
};

struct RecAConfig {
    RecAConfig() {}

    RecAConfig(const RecAConfig &config) { memcpy(this, &config, sizeof(config)); }

    RecAConfig &operator=(const RecAConfig &config) {
        memcpy(this, &config, sizeof(config));
        return *this;
    }

    std::string dump() const {
        std::stringstream ss;
        ss << "{sampleRate: " << sampleRate << ", sampleFormat: " << AudioUtils::sampleFormatString(sampleFormat)
           << ", bitrate: " << bitrate << "}";
        return ss.str();
    }

    int sampleRate = 48000;
    OH_BitsPerSample sampleFormat = SAMPLE_S16LE;
    OH_AudioChannelLayout channelLayout = CH_LAYOUT_STEREO;
    int64_t bitrate = 128000;
};

struct RecordConfig {
    RecordConfig() {}

    RecordConfig(const std::string &path, int w, int h, int durLimitMs, int rot) : vConfig(w, h, rot) {
        savePath = path;
        durationLimitMs = durLimitMs;
    }

    RecordConfig(const RecordConfig &config) : vConfig(config.vConfig), aConfig(config.aConfig) {
        savePath = config.savePath;
        durationLimitMs = config.durationLimitMs;
    }

    RecordConfig &operator=(const RecordConfig &config) {
        savePath = config.savePath;
        durationLimitMs = config.durationLimitMs;
        vConfig = config.vConfig;
        aConfig = config.aConfig;
        return *this;
    }

    std::string dump() const {
        std::stringstream ss;
        ss << "{\n savePath: " << savePath << ", durationLimitMs: " << durationLimitMs << "\n videoConfig"
           << vConfig.dump() << "\n audioConfig" << aConfig.dump() << "\n"
           << "}";
        return ss.str();
    }

    std::string savePath;
    // 时长限制单位 ms
    int durationLimitMs = -1;

    // 视频配置
    RecVConfig vConfig;
    // 音频配置
    RecAConfig aConfig;
};

NAMESPACE_END
