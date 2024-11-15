//
// Created on 2024/8/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include <multimedia/native_audio_channel_layout.h>
#include <multimedia/player_framework/native_avcodec_base.h>

NAMESPACE_DEFAULT

class AudioUtils {
public:
    static int64_t calPCMDurationUs(int64_t sumBytes, int sampleRate, int channelCount, OH_BitsPerSample sample) {
        return calPCMDurationUs(sumBytes, sampleRate, channelCount, sampleBytes(sample));
    }
    
    /**
     * 计算 pcm 的时长
     * 时长 = 数据长度 / (采样率 * 通道数 * 位深 / 8)
     */
    static int64_t calPCMDurationUs(int64_t sumBytes, int sampleRate, int channelCount, int sampleBytes) {
        return (int64_t) (sumBytes * 1000.0 / (sampleRate * channelCount * sampleBytes) * 1000.0);
    }
    
    static int64_t calPCMFrameBytes(int sampleRate, int channelCount, OH_BitsPerSample sample) {
        return (int64_t) (sampleRate * channelCount * sampleBytes(sample) * 0.02);
    }
    
    static const char *sampleFormatString(OH_BitsPerSample sample) {
        switch (sample) {
            case SAMPLE_U8 :
                return "U8";
            case SAMPLE_S16LE :
                return "S16LE";
            case SAMPLE_S24LE :
                return "S24LE";
            case SAMPLE_S32LE :
                return "S32LE";
            case SAMPLE_F32LE :
                return "F32LE";
            case SAMPLE_U8P :
                return "U8P";
            case SAMPLE_S16P :
                return "S16P";
            case SAMPLE_S24P :
                return "S24P";
            case SAMPLE_S32P :
                return "S32P";
            case SAMPLE_F32P :
                return "F32P";
            default :
                _ERROR("unknown OH_BitsPerSample");
                return "UNKNOWN";
        }
    }
    
    static int sampleBytes(OH_BitsPerSample bitsPerSample) {
        switch (bitsPerSample) {
        case SAMPLE_U8 :
            return 1;
        case SAMPLE_S16LE :
            return 2;
        case SAMPLE_S24LE :
            return 3;
        case SAMPLE_S32LE :
        case SAMPLE_F32LE :
            return 4;
        case SAMPLE_U8P :
            return 1;
        case SAMPLE_S16P :
            return 2;
        case SAMPLE_S24P :
            return 3;
        case SAMPLE_S32P :
            return 4;
        case SAMPLE_F32P :
            return 4;
        default :
            _ERROR("unknown OH_BitsPerSample");
            return 1;
        }
    }
    
    static int layoutChannelCount(OH_AudioChannelLayout layout) {
        switch (layout) {
        case CH_LAYOUT_MONO :
            return 1;
        case CH_LAYOUT_STEREO :
        case CH_LAYOUT_STEREO_DOWNMIX :
            return 2;
        case CH_LAYOUT_2POINT1 :
        case CH_LAYOUT_3POINT0 :
        case CH_LAYOUT_SURROUND :
            return 3;
        case CH_LAYOUT_3POINT1 :
        case CH_LAYOUT_4POINT0 :
        case CH_LAYOUT_QUAD_SIDE :
        case CH_LAYOUT_QUAD :
        case CH_LAYOUT_2POINT0POINT2 :
        case CH_LAYOUT_AMB_ORDER1_ACN_N3D :
        case CH_LAYOUT_AMB_ORDER1_ACN_SN3D :
        case CH_LAYOUT_AMB_ORDER1_FUMA :
            return 4;
        case CH_LAYOUT_4POINT1 :
        case CH_LAYOUT_5POINT0 :
        case CH_LAYOUT_5POINT0_BACK :
        case CH_LAYOUT_2POINT1POINT2 :
        case CH_LAYOUT_3POINT0POINT2 :
            return 5;

        case CH_LAYOUT_5POINT1 :
        case CH_LAYOUT_5POINT1_BACK :
        case CH_LAYOUT_6POINT0 :
        case CH_LAYOUT_3POINT1POINT2 :
        case CH_LAYOUT_6POINT0_FRONT :
        case CH_LAYOUT_HEXAGONAL :
            return 6;

        case CH_LAYOUT_6POINT1 :
        case CH_LAYOUT_6POINT1_BACK :
        case CH_LAYOUT_6POINT1_FRONT :
        case CH_LAYOUT_7POINT0 :
        case CH_LAYOUT_7POINT0_FRONT :
            return 7;
        case CH_LAYOUT_7POINT1 :
        case CH_LAYOUT_OCTAGONAL :
        case CH_LAYOUT_5POINT1POINT2 :
        case CH_LAYOUT_7POINT1_WIDE :
        case CH_LAYOUT_7POINT1_WIDE_BACK :
            return 8;

        case CH_LAYOUT_AMB_ORDER2_ACN_N3D :
        case CH_LAYOUT_AMB_ORDER2_ACN_SN3D :
        case CH_LAYOUT_AMB_ORDER2_FUMA :
            return 9;

        case CH_LAYOUT_5POINT1POINT4 :
        case CH_LAYOUT_7POINT1POINT2 :
            return 10;

        case CH_LAYOUT_7POINT1POINT4 :
        case CH_LAYOUT_10POINT2 :
            return 12;

        case CH_LAYOUT_9POINT1POINT4 :
            return 14;
        case CH_LAYOUT_9POINT1POINT6 :
        case CH_LAYOUT_HEXADECAGONAL :
        case CH_LAYOUT_AMB_ORDER3_ACN_N3D :
        case CH_LAYOUT_AMB_ORDER3_ACN_SN3D :
        case CH_LAYOUT_AMB_ORDER3_FUMA :
            return 16;

        case CH_LAYOUT_22POINT2 :
            return 24;

        default :
            _ERROR("unknown channel layout: %llu", layout);
        }
        return 0;
    }
};
NAMESPACE_END
