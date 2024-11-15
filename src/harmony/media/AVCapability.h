//
// Created on 2024/8/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "harmony/media/AVFormat.h"
#include "harmony/media/AVUtils.h"
#include <cstdint>
#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_averrors.h>

NAMESPACE_DEFAULT

class AVCapability {
public:
    /**
     * @param mime OH_AVCODEC_MIMETYPE_VIDEO_AVC / OH_AVCODEC_MIMETYPE_AUDIO_AAC
     * @param isEncoder 是否为编码器
     * @param category 对应 OH_AVCodecCategory, 如果为 -1 表示使用系统推荐
     */
    AVCapability(const char *mime, bool isEncoder, int category = -1) {
        m_is_encoder = isEncoder;
        if (category != HARDWARE && category != SOFTWARE) {
            // 系统推荐的，可能是 hardware, 也可能是 software
            m_capability = OH_AVCodec_GetCapability(mime, isEncoder);
        } else {
            m_capability = OH_AVCodec_GetCapabilityByCategory(mime, isEncoder, (OH_AVCodecCategory)category);
        }
        if (m_capability == nullptr) {
            _WARN("Failed to get codec capability (mime: %s, isEncoder: %d, category: %d", mime, isEncoder, category);
        }
    }

    AVCapability(const AVCapability &other) {
        m_capability = other.m_capability;
        m_is_encoder = other.m_is_encoder;
    }

    ~AVCapability() {}

public:
    inline bool valid() const { return m_capability != nullptr; }

    const char *codecName() {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        return OH_AVCapability_GetName(m_capability);
    }

    bool isHardware() {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        return OH_AVCapability_IsHardware(m_capability);
    }

    int maxSupportedInstances() {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        return OH_AVCapability_GetMaxSupportedInstances(m_capability);
    }

    bool isEncoderBitrateModeSupported(OH_BitrateMode mode) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        if (!m_is_encoder) {
            _ERROR("Codec capability is not for encoder!");
            return false;
        }
        return OH_AVCapability_IsEncoderBitrateModeSupported(m_capability, mode);
    }

    bool getEncoderBitrateRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        min = max = -1;
        if (!m_is_encoder) {
            _ERROR("Codec capability is not for encoder!");
            return false;
        }
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetEncoderBitrateRange(m_capability, &range);
        _ERROR_RETURN_IF(code, false, "get encoder bitrate range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return true;
    }

    bool getEncoderQualityRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        min = max = -1;
        if (!m_is_encoder) {
            _ERROR("Codec capability is not for encoder!");
            return false;
        }
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetEncoderQualityRange(m_capability, &range);
        _ERROR_RETURN_IF(code, false, "get encoder quality range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return true;
    }

    bool getEncoderComplexityRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        min = max = -1;
        if (!m_is_encoder) {
            _ERROR("Codec capability is not for encoder!");
            return false;
        }
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetEncoderComplexityRange(m_capability, &range);
        _ERROR_RETURN_IF(code, false, "get encoder complexity range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return true;
    }

    std::vector<int> getSupportedProfiles() {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        std::vector<int> profiles;

        const int32_t *pprofiles = nullptr;
        uint32_t count = 0;
        OH_AVErrCode code = OH_AVCapability_GetSupportedProfiles(m_capability, &pprofiles, &count);
        _ERROR_RETURN_IF(code, profiles, "get supported profiles failed: %s", AVUtils::errString(code));
        for (uint32_t i = 0; i < count; ++i) {
            profiles.push_back(pprofiles[i]);
        }
        return profiles;
    }

    std::vector<int> getSupportedLevelsForProfile(int32_t profile) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        std::vector<int> levels;

        const int32_t *plevels = nullptr;
        uint32_t count = 0;
        OH_AVErrCode code = OH_AVCapability_GetSupportedLevelsForProfile(m_capability, profile, &plevels, &count);
        _ERROR_RETURN_IF(code, levels, "get supported levels for profile failed: %s", AVUtils::errString(code));
        for (uint32_t i = 0; i < count; ++i) {
            levels.push_back(plevels[i]);
        }
        return levels;
    }

    bool areProfileAndLevelSupported(int32_t profile, int32_t level) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        return OH_AVCapability_AreProfileAndLevelSupported(m_capability, profile, level);
    }

    bool isFeatureSupported(OH_AVCapabilityFeature feature) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        return OH_AVCapability_IsFeatureSupported(m_capability, feature);
    }

    AVFormat getFeatureProperties(OH_AVCapabilityFeature feature) {
        _FATAL_IF(m_capability == nullptr, "Invalid codec capability!");
        OH_AVFormat *format = OH_AVCapability_GetFeatureProperties(m_capability, feature);
        return AVFormat(format, true);
    }

protected:
    OH_AVCapability *m_capability = nullptr;

    bool m_is_encoder = false;
};


class AudioCapability : public AVCapability {
public:
    static AudioCapability aac(bool isEncoder, int category = -1) {
        return AudioCapability(OH_AVCODEC_MIMETYPE_AUDIO_AAC, isEncoder, category);
    }

public:
    AudioCapability(const char *mime, bool isEncoder, int category = -1) : AVCapability(mime, isEncoder, category) {}

    AudioCapability(const AudioCapability &other) : AVCapability(other) {}

public:
    std::vector<int> getSupportedSampleRates() {
        _FATAL_IF(m_capability == nullptr, "Invalid audio codec capability!");
        std::vector<int> sampleRates;

        const int32_t *psampleRates = nullptr;
        uint32_t count = 0;
        OH_AVErrCode code = OH_AVCapability_GetAudioSupportedSampleRates(m_capability, &psampleRates, &count);
        _ERROR_RETURN_IF(code, sampleRates, "get supported sample rates failed: %s", AVUtils::errString(code));
        for (uint32_t i = 0; i < count; ++i) {
            sampleRates.push_back(psampleRates[i]);
        }
        return sampleRates;
    }

    OH_AVErrCode getChannelCountRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid audio codec capability!");
        min = max = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetAudioChannelCountRange(m_capability, &range);
        _ERROR_RETURN_IF(code, code, "get audio channel count range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return code;
    }
};


class VideoCapability : public AVCapability {
public:
    static VideoCapability h264(bool isEncoder, int category = -1) {
        return VideoCapability(OH_AVCODEC_MIMETYPE_VIDEO_AVC, isEncoder, category);
    }

    static VideoCapability h265(bool isEncoder, int category = -1) {
        return VideoCapability(OH_AVCODEC_MIMETYPE_VIDEO_HEVC, isEncoder, category);
    }

public:
    VideoCapability(const char *mime, bool isEncoder, int category = -1) : AVCapability(mime, isEncoder, category) {}

    VideoCapability(const VideoCapability &other) : AVCapability(other) {}

public:
    int alignWidth(int width) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        OH_AVErrCode code = OH_AVCapability_GetVideoWidthAlignment(m_capability, &width);
        _ERROR_RETURN_IF(code, -1, "OH_AVCapability_GetVideoWidthAlignment failed: %s", AVUtils::errString(code));
        return width;
    }

    int alignHeight(int height) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        OH_AVErrCode code = OH_AVCapability_GetVideoHeightAlignment(m_capability, &height);
        _ERROR_RETURN_IF(code, -1, "OH_AVCapability_GetVideoHeightAlignment failed: %s", AVUtils::errString(code));
        return height;
    }

    OH_AVErrCode getWidthRangeForHeight(int height, int &widthMin, int &widthMax) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        widthMin = widthMax = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetVideoWidthRangeForHeight(m_capability, height, &range);
        _ERROR_RETURN_IF(code, code, "get video width range for height failed: %s", AVUtils::errString(code));
        widthMin = range.minVal;
        widthMax = range.maxVal;
        return code;
    }

    OH_AVErrCode getHeightRangeForWidth(int width, int &heightMin, int &heightMax) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        heightMin = heightMax = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetVideoHeightRangeForWidth(m_capability, width, &range);
        _ERROR_RETURN_IF(code, code, "get video height range for width failed: %s", AVUtils::errString(code));
        heightMin = range.minVal;
        heightMax = range.maxVal;
        return code;
    }

    OH_AVErrCode getWidthRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        min = max = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetVideoWidthRange(m_capability, &range);
        _ERROR_RETURN_IF(code, code, "get video width range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return code;
    }

    OH_AVErrCode getHeightRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        min = max = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetVideoHeightRange(m_capability, &range);
        _ERROR_RETURN_IF(code, code, "get video height range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return code;
    }

    bool isSizeSupported(int width, int height) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        return OH_AVCapability_IsVideoSizeSupported(m_capability, width, height);
    }

    OH_AVErrCode getFrameRateRange(int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        min = max = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetVideoFrameRateRange(m_capability, &range);
        _ERROR_RETURN_IF(code, code, "get video frame rate range failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return code;
    }

    OH_AVErrCode getFrameRateRangeForSize(int width, int height, int &min, int &max) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        min = max = -1;
        OH_AVRange range = {-1, -1};
        OH_AVErrCode code = OH_AVCapability_GetVideoFrameRateRangeForSize(m_capability, width, height, &range);
        _ERROR_RETURN_IF(code, code, "get video frame rate range for size failed: %s", AVUtils::errString(code));
        min = range.minVal;
        max = range.maxVal;
        return code;
    }

    bool isFrameRateSupported(int width, int height, int frameRate) {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        return OH_AVCapability_AreVideoSizeAndFrameRateSupported(m_capability, width, height, frameRate);
    }

    std::vector<int> getSupportedPixelFormats() {
        _FATAL_IF(m_capability == nullptr, "Invalid video codec capability!");
        std::vector<int> pixelFormats;

        const int32_t *pformats = nullptr;
        uint32_t count = 0;
        OH_AVErrCode code = OH_AVCapability_GetVideoSupportedPixelFormats(m_capability, &pformats, &count);
        _ERROR_RETURN_IF(code, pixelFormats, "get supported pixel formats failed: %s", AVUtils::errString(code));
        for (uint32_t i = 0; i < count; ++i) {
            pixelFormats.push_back(pformats[i]);
        }
        return pixelFormats;
    }
};

NAMESPACE_END
