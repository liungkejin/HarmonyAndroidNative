//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include "harmony/media/AVUtils.h"
#include <cstdint>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avcodec_base.h>

NAMESPACE_DEFAULT

class AVFormat : Object {
public:
    AVFormat() {
        m_owner = true;
        m_format = OH_AVFormat_Create();
        _FATAL_IF(m_format == nullptr, "OH_AVFormat_Create failed!");
    }
    AVFormat(OH_AVFormat *format, bool owner) : m_format(format), m_owner(owner) {}

    AVFormat(const AVFormat &other) : m_format(other.m_format), m_owner(other.m_owner), Object(other) {}

    ~AVFormat() {
        if (m_owner && m_format && no_reference()) {
            OH_AVFormat_Destroy(m_format);
            m_format = nullptr;
        }
    }

public:
    inline bool valid() const { return m_format != nullptr; }

    inline OH_AVFormat *value() const { return m_format; }

    AVFormat copy() {
        OH_AVFormat *dstFormat = nullptr;
        if (!OH_AVFormat_Copy(dstFormat, m_format)) {
            _FATAL("OH_AVFormat_Copy failed!");
        }
        return AVFormat(dstFormat, true);
    }

    const char *dump() const {
        if (!m_format) {
            return "nullptr";
        }
        const char *info = OH_AVFormat_DumpInfo(m_format);
        return info ? info : "<empty>";
    }

public:
    bool setValue(const char *key, int value) { return OH_AVFormat_SetIntValue(m_format, key, value); }

    int getInt(const char *key, int defValue = 0) const {
        int value = defValue;
        if (!OH_AVFormat_GetIntValue(m_format, key, &value)) {
            _WARN("OH_AVFormat_GetIntValue failed! key: %s defValue: %d", key, defValue);
            return defValue;
        }
        return value;
    }

    bool setValue(const char *key, int64_t value) { return OH_AVFormat_SetLongValue(m_format, key, value); }

    int64_t getLong(const char *key, int64_t defValue = 0) const {
        int64_t value = defValue;
        if (!OH_AVFormat_GetLongValue(m_format, key, &value)) {
            _WARN("OH_AVFormat_GetLongValue failed! key: %s defValue: %d", key, defValue);
            return defValue;
        }
        return value;
    }

    bool setValue(const char *key, float value) { return OH_AVFormat_SetFloatValue(m_format, key, value); }

    float getFloat(const char *key, float defValue = 0.0f) const {
        float value = defValue;
        if (!OH_AVFormat_GetFloatValue(m_format, key, &value)) {
            _WARN("OH_AVFormat_GetFloatValue failed! key: %s defValue: %f", key, defValue);
            return defValue;
        }
        return value;
    }

    bool setValue(const char *key, double value) { return OH_AVFormat_SetDoubleValue(m_format, key, value); }

    double getDouble(const char *key, double defValue = 0.0) const {
        double value = defValue;
        if (!OH_AVFormat_GetDoubleValue(m_format, key, &value)) {
            _WARN("OH_AVFormat_GetDoubleValue failed! key: %s defValue: %f", key, defValue);
            return defValue;
        }
        return value;
    }

    bool setValue(const char *key, const char *value) { return OH_AVFormat_SetStringValue(m_format, key, value); }

    const char *getString(const char *key, const char *defValue = "") const {
        const char *out = nullptr;
        if (!OH_AVFormat_GetStringValue(m_format, key, &out)) {
            _WARN("OH_AVFormat_GetStringValue failed! key: %s defValue: %s", key, defValue);
            return defValue;
        }
        return out;
    }

    bool setBuffer(const char *key, const uint8_t *buffer, size_t size) {
        return OH_AVFormat_SetBuffer(m_format, key, buffer, size);
    }

    const uint8_t *getBuffer(const char *key, size_t &size) const {
        uint8_t *data = nullptr;
        if (!OH_AVFormat_GetBuffer(m_format, key, &data, &size)) {
            _WARN("OH_AVFormat_GetBuffer failed! key: %s", key);
            size = 0;
            return nullptr;
        }
        return data;
    }

public:
    
    int64_t durationUs() const { return getLong(OH_MD_KEY_DURATION); }

    bool setTrackType(OH_MediaType type) { return setValue(OH_MD_KEY_TRACK_TYPE, static_cast<int>(type)); }

    virtual int trackType() const {
        std::string mime = codecMime();
        if (mime == OH_AVCODEC_MIMETYPE_VIDEO_HEVC || mime == OH_AVCODEC_MIMETYPE_VIDEO_AVC ||
            mime == OH_AVCODEC_MIMETYPE_VIDEO_MPEG4 || mime == OH_AVCODEC_MIMETYPE_VIDEO_VVC) {
            return MEDIA_TYPE_VID;
        } else if (mime == OH_AVCODEC_MIMETYPE_AUDIO_AAC || mime == OH_AVCODEC_MIMETYPE_AUDIO_AMR_NB ||
                   mime == OH_AVCODEC_MIMETYPE_AUDIO_AMR_WB || mime == OH_AVCODEC_MIMETYPE_AUDIO_APE ||
                   mime == OH_AVCODEC_MIMETYPE_AUDIO_FLAC || mime == OH_AVCODEC_MIMETYPE_AUDIO_G711MU ||
                   mime == OH_AVCODEC_MIMETYPE_AUDIO_MPEG || mime == OH_AVCODEC_MIMETYPE_AUDIO_VIVID ||
                   mime == OH_AVCODEC_MIMETYPE_AUDIO_OPUS || mime == OH_AVCODEC_MIMETYPE_AUDIO_VORBIS) {
            return MEDIA_TYPE_AUD;
        }
        _WARN("unknown track type! mime: %s", mime);
        return MEDIA_TYPE_AUD;
    }

    bool isVideoTrack() const { return trackType() == MEDIA_TYPE_VID; }

    bool isAudioTrack() const { return trackType() == MEDIA_TYPE_AUD; }

    bool setCodecMime(const char *mime) { return setValue(OH_MD_KEY_CODEC_MIME, mime); }

    const char *codecMime() const { return getString(OH_MD_KEY_CODEC_MIME); }

    bool setBitrate(int64_t bitrate) { return setValue(OH_MD_KEY_BITRATE, bitrate); }

    int64_t bitrate() const { return getLong(OH_MD_KEY_BITRATE); }

    bool setMaxInputSize(int maxSize) { return setValue(OH_MD_KEY_MAX_INPUT_SIZE, maxSize); }

    int maxInputSize() const { return getInt(OH_MD_KEY_MAX_INPUT_SIZE); }

    bool setProfile(int profile) { return setValue(OH_MD_KEY_PROFILE, profile); }

    int profile() const { return getInt(OH_MD_KEY_PROFILE); }

    bool setQuality(int quality) { return setValue(OH_MD_KEY_QUALITY, quality); }

    int quality() const { return getInt(OH_MD_KEY_QUALITY); }

    bool setTitle(const char *title) { return setValue(OH_MD_KEY_TITLE, title); }
    
    const char *title() const { return getString(OH_MD_KEY_TITLE); }

    bool setArtist(const char *artist) { return setValue(OH_MD_KEY_ARTIST, artist); }
    
    const char *artist() { return getString(OH_MD_KEY_ARTIST); }

    bool setAlbum(const char *album) { return setValue(OH_MD_KEY_ALBUM, album); }
    
    const char *album() const { return getString(OH_MD_KEY_ALBUM); }

    bool setAlbumArtist(const char *artist) { return setValue(OH_MD_KEY_ALBUM_ARTIST, artist); }
    
    const char *albumArtist() const { return getString(OH_MD_KEY_ALBUM_ARTIST); }

    bool setDate(const char *date) { return setValue(OH_MD_KEY_DATE, date); }
    
    const char *date() const { return getString(OH_MD_KEY_DATE); }

    bool setComment(const char *comment) { return setValue(OH_MD_KEY_COMMENT, comment); }
    
    const char *comment() const { return getString(OH_MD_KEY_COMMENT); }

    bool setGenre(const char *genre) { return setValue(OH_MD_KEY_GENRE, genre); }
    
    const char *genre() const { return getString(OH_MD_KEY_GENRE); }

    bool setCopyright(const char *copyright) { return setValue(OH_MD_KEY_COPYRIGHT, copyright); }
    
    const char *copyright() const { return getString(OH_MD_KEY_COPYRIGHT); }

    bool setLanguage(const char *lang) { return setValue(OH_MD_KEY_LANGUAGE, lang); }
    
    const char *language() const { return getString(OH_MD_KEY_LANGUAGE); }

    bool setDescription(const char *desc) { return setValue(OH_MD_KEY_DESCRIPTION, desc); }
    
    const char *description() const { return getString(OH_MD_KEY_DESCRIPTION); }

    bool setLyrics(const char *s) { return setValue(OH_MD_KEY_LYRICS, s); }
    
    const char * lyrics() { return getString(OH_MD_KEY_LYRICS); }

protected:
    OH_AVFormat *m_format = nullptr;
    bool m_owner = false;
};

class VideoFormat : public AVFormat {
public:
    static VideoFormat h264(int width, int height) { return VideoFormat(OH_AVCODEC_MIMETYPE_VIDEO_AVC, width, height); }

    static VideoFormat h265(int width, int height) {
        return VideoFormat(OH_AVCODEC_MIMETYPE_VIDEO_HEVC, width, height);
    }

public:
    VideoFormat() : AVFormat() {}

    VideoFormat(const char *mime, int width, int height) {
        m_owner = true;
        m_format = OH_AVFormat_CreateVideoFormat(mime, width, height);
    }

    VideoFormat(OH_AVFormat *format, bool owner) : AVFormat(format, owner) {}

    VideoFormat(const AVFormat &format) {
        m_owner = true;
        OH_AVFormat_Copy(m_format, format.value());
        _FATAL_IF(m_format == nullptr, "OH_AVFormat_Copy failed!");
    }

    VideoFormat(const VideoFormat &format) : AVFormat(format) {}

    ~VideoFormat() { AVFormat::~AVFormat(); }

public:
    VideoFormat copy() {
        OH_AVFormat *dstFormat = nullptr;
        if (!OH_AVFormat_Copy(dstFormat, m_format)) {
            _FATAL("OH_AVFormat_Copy failed!");
        }
        return VideoFormat(dstFormat, true);
    }

public:
    int trackType() const override {
        return OH_MediaType::MEDIA_TYPE_AUD;
    }
    
    bool setWidth(int width) { return setValue(OH_MD_KEY_WIDTH, width); }

    int width() const { return getInt(OH_MD_KEY_WIDTH); }

    bool setHeight(int height) { return setValue(OH_MD_KEY_HEIGHT, height); }

    int height() const { return getInt(OH_MD_KEY_HEIGHT); }

    bool setPixelFormat(OH_AVPixelFormat format) { return setValue(OH_MD_KEY_PIXEL_FORMAT, format); }

    int pixelFormat() const { return getInt(OH_MD_KEY_PIXEL_FORMAT); }

    bool setFrameRate(double fps) { return setValue(OH_MD_KEY_FRAME_RATE, fps); }

    double frameRate() const { return getDouble(OH_MD_KEY_FRAME_RATE); }

    bool setEncoderQPMax(int max) { return setValue(OH_MD_KEY_VIDEO_ENCODER_QP_MAX, max); }

    bool setEncoderQPMin(int min) { return setValue(OH_MD_KEY_VIDEO_ENCODER_QP_MIN, min); }

    bool setEncodeBitrateMode(OH_VideoEncodeBitrateMode mode) {
        return setValue(OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE, mode);
    }

    int encodeBitrateMode() const { return getInt(OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE, -1); }

    /**
     * 设置 I 帧间隔
     * @param interval < 0 表示第一帧之后就不再请求I帧，= 0 表示每一帧都是 I 帧，> 0 表示I帧的间隔，单位为 ms
     */
    bool setIFrameInterval(int interval) { return setValue(OH_MD_KEY_I_FRAME_INTERVAL, interval); }

    int iframeInterval() const { return getInt(OH_MD_KEY_I_FRAME_INTERVAL); }

    bool setRotation(int rot) { return setValue(OH_MD_KEY_ROTATION, rot); }

    int rotation() const { return getInt(OH_MD_KEY_ROTATION); }

    bool setColorRange(bool fullRange) { return setValue(OH_MD_KEY_RANGE_FLAG, fullRange); }

    bool isColorRangeFull() const { return getInt(OH_MD_KEY_RANGE_FLAG) == 1; }

    bool setColorSpace(OH_ColorPrimary colorSpace) { return setValue(OH_MD_KEY_COLOR_PRIMARIES, colorSpace); }

    int colorSpace() const { return getInt(OH_MD_KEY_COLOR_PRIMARIES); }

    bool setColorTransfer(OH_TransferCharacteristic colorTransfer) {
        return setValue(OH_MD_KEY_TRANSFER_CHARACTERISTICS, colorTransfer);
    }

    int colorTransfer() const { return getInt(OH_MD_KEY_TRANSFER_CHARACTERISTICS); }

    bool setMatrixCoefficients(OH_MatrixCoefficient mat) { return setValue(OH_MD_KEY_MATRIX_COEFFICIENTS, mat); }

    int matrixCoefficients() const { return getInt(OH_MD_KEY_MATRIX_COEFFICIENTS); }
};

class AudioFormat : public AVFormat {
public:
    static AudioFormat aac(int sampleRate, int channelCount) {
        return AudioFormat(OH_AVCODEC_MIMETYPE_AUDIO_AAC, sampleRate, channelCount);
    }

public:
    AudioFormat() : AVFormat() {}

    AudioFormat(const char *mime, int sampleRate, int channelCount) {
        m_owner = true;
        m_format = OH_AVFormat_CreateAudioFormat(mime, sampleRate, channelCount);
    }

    AudioFormat(OH_AVFormat *format, bool owner) : AVFormat(format, owner) {}

    AudioFormat(const AVFormat &format) {
        m_owner = true;
        OH_AVFormat_Copy(m_format, format.value());
        _FATAL_IF(m_format == nullptr, "OH_AVFormat_Copy failed!");
    }

    AudioFormat(const AudioFormat &format) : AVFormat(format) {}

    ~AudioFormat() { AVFormat::~AVFormat(); }

public:
    AudioFormat copy() {
        OH_AVFormat *dstFormat = nullptr;
        if (!OH_AVFormat_Copy(dstFormat, m_format)) {
            _FATAL("OH_AVFormat_Copy failed!");
        }
        return AudioFormat(dstFormat, true);
    }
    
public:
    int trackType() const override {
        return OH_MediaType::MEDIA_TYPE_VID;
    }
    
    bool setSampleFormat(OH_BitsPerSample sample) { return setValue(OH_MD_KEY_AUDIO_SAMPLE_FORMAT, sample); }

    int sampleFormat() const { return getInt(OH_MD_KEY_AUDIO_SAMPLE_FORMAT); }

    int setChannelCount(int count) { return setValue(OH_MD_KEY_AUD_CHANNEL_COUNT, count); }

    int channelCount() const { return getInt(OH_MD_KEY_AUD_CHANNEL_COUNT); }

    bool setSampleRate(int sampleRate) { return setValue(OH_MD_KEY_AUD_SAMPLE_RATE, sampleRate); }

    int sampleRate() const { return getInt(OH_MD_KEY_AUD_SAMPLE_RATE); }

    // 只支持 encoder
    bool setChannelLayout(int64_t layout) { return setValue(OH_MD_KEY_CHANNEL_LAYOUT, layout); }

    int64_t channelLayout() const { return getLong(OH_MD_KEY_CHANNEL_LAYOUT); }

    bool setBitsPerCodedSample(OH_BitsPerSample sample) { return setValue(OH_MD_KEY_BITS_PER_CODED_SAMPLE, sample); }

    int bitsPerCodedSample() const { return getInt(OH_MD_KEY_BITS_PER_CODED_SAMPLE); }
};
NAMESPACE_END
