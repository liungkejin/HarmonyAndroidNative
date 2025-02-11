//
// Created by kejin on 25-1-23.
//

#pragma once

// 封装 libdshowcapture

#include <common/Common.h>
#include <vector>
#include <string>
#include "DSUtils.h"

NAMESPACE_DEFAULT
struct DSVideoConfig;
struct DSAudioConfig;

struct DSVideoFrame {
    int width = 0;
    int height = 0;
    DSVideoFmt fmt = DSVideoFmt::Any;
    unsigned char *data = nullptr;
    size_t size = 0;

    int64_t frame_interval = 0;
    int64_t startTime = 0;
    int64_t stopTime = 0;
    long rotation = 0;
};

typedef std::function<void(DSVideoFrame& frame)> DSVideoDataCallback;

struct DSAudioSample {
    int sampleRate = 0;
    int channels = 0;
    DSAudioFmt fmt = DSAudioFmt::Any;
    unsigned char *data = nullptr;
    size_t size = 0;
    int64_t startTime = 0;
    int64_t stopTime = 0;
};

typedef std::function<void(DSAudioSample& sample)> DSAudioDataCallback;

typedef std::function<void()> DSDeviceReactivateCallback;

class DSVideoStreamInfo {
public:
    void setMinSize(int minW, int minH) {
        this->m_min_width = minW;
        this->m_min_height = minH;
    }

    void setMaxSize(int maxW, int maxH) {
        this->m_max_width = maxW;
        this->m_max_height = maxH;
    }

    void setGranularity(int granularityX, int granularityY) {
        this->m_granularity_x = granularityX;
        this->m_granularity_y = granularityY;
    }

    void setInterval(int64_t minInterval, int64_t maxInterval) {
        this->m_min_interval = minInterval;
        this->m_max_interval = maxInterval;
    }

    void setFormat(int format) {
        m_fmt = (DSVideoFmt) format;
    }

public:
    bool isValidSize(int width, int height) const {
        return width >= m_min_width && width <= m_max_width && height >= m_min_height && height <= m_max_height;
    }

    int minWidth() const { return m_min_width; }

    int minHeight() const { return m_min_height; }

    int maxWidth() const { return m_max_width; }

    int maxHeight() const { return m_max_height; }

    int minInterval() const { return m_min_interval; }

    int maxInterval() const { return m_max_interval; }

    int minFPS() const { return m_min_interval == 0 ? 0 : (int) (10010000 / m_min_interval); }

    int maxFPS() const { return m_max_interval == 0 ? 0 : (int) (10010000 / m_max_interval); }

    DSVideoFmt format() const { return m_fmt; }

    std::string toString() const {
        std::stringstream ss;
        ss << "DSVideoStreamInfo{";
        if (m_min_width == m_max_width) {
            ss << "size: [" << m_min_width << "x" << m_min_height << "], ";
        } else {
            ss << "size: [" << m_min_width << "x" << m_min_height << " - " << m_max_width << "x" << m_max_height << "], ";
        }
        ss << "granularity: " << m_granularity_x << "x" << m_granularity_y << ", ";
        ss << "frame interval: [" << m_min_interval << " - " << m_max_interval << "], ";
        ss << "format: " << DSUtils::videoFmtString((int) m_fmt);
        ss << "}";
        return ss.str();
    }

private:
    int m_min_width = 0, m_min_height = 0;
    int m_max_width = 0, m_max_height = 0;
    // 颗粒度
    int m_granularity_x = 1, m_granularity_y = 1;
    // 帧率
    int64_t m_min_interval = 0, m_max_interval = 0;
    // 格式
    DSVideoFmt m_fmt = DSVideoFmt::Any;
};

class DSVideoDeviceInfo {
public:
    void setDeviceId(const std::wstring &name, const std::wstring &path) {
        // convert wstring to string
        m_name = WinUtils::wstringToString(name);
        m_path = WinUtils::wstringToString(path);
    }

    void setAudioAttached(bool attached) {
        m_audio_attached = attached;
    }

    void setSeparateAudioFilter(bool separate) {
        m_separate_audio_filter = separate;
    }

    void addStream(const DSVideoStreamInfo &stream) {
        m_streams.push_back(stream);
    }

public:
    bool valid() const {
        return !m_name.empty();
    }

    std::string name() const { return m_name; }

    std::string path() const { return m_path; }

    bool audioAttached() const { return m_audio_attached; }

    bool separateAudioFilter() const { return m_separate_audio_filter; }

    std::vector<DSVideoFmt> supportedFormats() const {
        std::vector<DSVideoFmt> formats;
        for (auto &stream: m_streams) {
            // 去重
            if (std::find(formats.begin(), formats.end(), stream.format()) == formats.end()) {
                formats.push_back(stream.format());
            }
        }
        return formats;
    }

    std::vector<DSVideoStreamInfo> getStreams(const DSVideoFmt fmt) const {
        std::vector<DSVideoStreamInfo> streams;
        for (auto &stream: m_streams) {
            if (stream.format() == fmt) {
                streams.push_back(stream);
            }
        }
        // 按分辨率排序从大到小
        std::sort(streams.begin(), streams.end(), [](const DSVideoStreamInfo &a, const DSVideoStreamInfo &b) {
            return a.minWidth() * a.minHeight() > b.minWidth() * b.minHeight();
        });
        return streams;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "DSVideoDeviceInfo{\n";
        ss << "name: " << m_name << "\n";
        ss << "path: " << m_path << "\n";
        ss << "audio_attached: " << m_audio_attached << "\n";
        ss << "separate_audio_filter: " << m_separate_audio_filter << "\n";
        ss << "streams: [\n";
        for (auto &stream: m_streams) {
            ss << "\t" << stream.toString() << "\n";
        }
        ss << "]\n";
        ss << "}";
        return ss.str();
    }

private:
    std::string m_name;
    std::string m_path;
    bool m_audio_attached = false;
    bool m_separate_audio_filter = false;
    std::vector<DSVideoStreamInfo> m_streams;
};

struct DSAudioStreamInfo {
    int m_min_channels = 0;
    int m_max_channels = 0;
    int m_min_sample_rate = 0;
    int m_max_sample_rate = 0;
    int m_channels_granularity = 1;
    int m_sample_rate_granularity = 1;
    DSAudioFmt m_format = DSAudioFmt::Any;
};

struct DSAudioDeviceInfo {
    std::string m_name;
    std::string m_path;

    std::vector<DSAudioStreamInfo> m_streams;
};

struct DSVideoConfig {
    DSVideoDataCallback data_cb;
    DSDeviceReactivateCallback reactivate_cb;

    std::string name;
    std::string path;

    int width = 0;
    int height = 0;
    bool flip = false;
    int64_t frame_interval = 0;

    DSVideoFmt internal_fmt = DSVideoFmt::Any;

    DSVideoFmt desire_fmt = DSVideoFmt::Any;
};

struct DSAudioConfig {
    DSAudioDataCallback data_cb;

    std::string name;
    std::string path;
    /**
     * 是否使用视频设备的音频，如果为true，则name和path字段将被忽略
     */
    bool use_video_device = false;
    bool use_separate_audio_filter = false;
    int sample_rate = 0;
    int channels = 0;

    DSAudioFmt format = DSAudioFmt::Any;

    DSAudioMode mode = DSAudioMode::Capture;
};

class DSDataQueue {
public:
    explicit DSDataQueue(int size) : m_que_size(size) {}

    void reset() {
        LOCK_MUTEX(m_mutex);
        m_front = 0;
        m_back = -1;
    }

protected:
    void frontForward() {
        if (m_back < 0) {
            m_back = m_front;
        }

        m_front += 1;
        if (m_front - m_back >= m_que_size) {
            m_back = (m_back + 1);
        }
        if (m_front > m_que_size && m_back > m_que_size) {
            m_front -= m_que_size;
            m_back -= m_que_size;
        }
    }

    int frontIndex() {
        return m_front % m_que_size;
    }

    int latestIndex() {
        if (m_back < 0) return -1;
        return (m_front - 1 + m_que_size) % m_que_size;
    }

    void backForward(bool toNewest) {
        if (toNewest) {
            m_back = -1;
        } else {
            m_back += 1;
            if (m_back >= m_front) {
                m_back = -1;
            }
        }
    }

    int backIndex(bool toNewest) {
        if (toNewest) {
            return latestIndex();
        }
        return m_back < 0 ? m_back : m_back % m_que_size;
    }

    int queSize() const { return m_que_size; }

    std::mutex m_mutex;
private:
    const int m_que_size;

    int m_front = 0;
    int m_back = -1;
};

class DSVideoFrameQueue : public DSDataQueue {
public:
    explicit DSVideoFrameQueue(int size = 2) : DSDataQueue(size) {
        m_frames = new DSVideoFrame[size];
        m_data = new Array[size];
    }

    ~DSVideoFrameQueue() {
        delete[] m_frames;
        delete[] m_data;
    }
public:
    void pushFrame(int width, int height, DSVideoFmt fmt, int64_t frame_interval,
                  const uint8_t *data, size_t size, long long startTime, long long stopTime, long rotation) {
        LOCK_MUTEX(m_mutex);
        int index = frontIndex();
        auto &frame = m_frames[index];
        frame.width = width;
        frame.height = height;
        frame.fmt = fmt;
        frame.frame_interval = frame_interval;
        auto dst = m_data[index].obtain<uint8_t>(size, false);
        memcpy(dst, data, size);
        frame.data = dst;
        frame.size = size;
        frame.startTime = startTime;
        frame.stopTime = stopTime;
        frame.rotation = rotation;

        DSDataQueue::frontForward();
    }

    const DSVideoFrame *popFrame(bool getNewest = false) {
        LOCK_MUTEX(m_mutex);
        int index = backIndex(getNewest);
        if (index < 0) {
            return nullptr;
        }

        auto &frame = m_frames[index];
        // copy data
        auto dst = m_output_data.obtain<uint8_t>(frame.size, false);
        memcpy(dst, frame.data, frame.size);
        m_output_frame = frame;
        m_output_frame.data = dst;

        DSDataQueue::backForward(getNewest);
        return &m_output_frame;
    }

    void release() {
        LOCK_MUTEX(m_mutex);
        for (int i = 0; i < queSize(); i++) {
            m_data[i].free();
        }
        m_output_data.free();
    }

private:
    DSVideoFrame *m_frames;
    Array *m_data;

    DSVideoFrame m_output_frame;
    Array m_output_data;
};

class DSAudioSampleQueue : public DSDataQueue {
public:
    explicit DSAudioSampleQueue(int size = 5) : DSDataQueue(size) {
        m_frames = new DSAudioSample[size];
        m_data = new Array[size];
    }

    ~DSAudioSampleQueue() {
        delete[] m_frames;
        delete[] m_data;
    }
public:
    void pushSample(int sampleRate, int channels, DSAudioFmt fmt,
            const uint8_t *data, size_t size, long long startTime, long long stopTime) {
        LOCK_MUTEX(m_mutex);

        int index = frontIndex();
        auto &frame = m_frames[index];
        frame.sampleRate = sampleRate;
        frame.channels = channels;
        frame.fmt = fmt;
        auto dst = m_data[index].obtain<uint8_t>(size, false);
        memcpy(dst, data, size);
        frame.data = dst;
        frame.size = size;
        frame.startTime = startTime;
        frame.stopTime = stopTime;

        DSDataQueue::frontForward();
    }

    const DSAudioSample *popSample(bool getNewest = false) {
        LOCK_MUTEX(m_mutex);

        int index = backIndex(getNewest);
        if (index < 0) {
            return nullptr;
        }

        auto &frame = m_frames[index];
        // copy data
        auto dst = m_output_data.obtain<uint8_t>(frame.size, false);
        memcpy(dst, frame.data, frame.size);
        m_output_frame = frame;
        m_output_frame.data = dst;

        DSDataQueue::backForward(getNewest);
        return &m_output_frame;
    }

    void release() {
        LOCK_MUTEX(m_mutex);
        for (int i = 0; i < queSize(); i++) {
            m_data[i].free();
        }
        m_output_data.free();
    }

private:
    DSAudioSample *m_frames;
    Array *m_data;

    DSAudioSample m_output_frame;
    Array m_output_data;
};

class DSCaptureMgr {
public:
    static std::vector<DSVideoDeviceInfo> enumVideoDevices();
    static std::vector<DSAudioDeviceInfo> enumAudioDevices();

public:
    explicit DSCaptureMgr(bool init = true);
    ~DSCaptureMgr();

public:
    bool setVideoConfig(const DSVideoConfig &config);

    const DSVideoConfig &getVideoConfig() const;

    bool setAudioConfig(const DSAudioConfig &config);

    const DSAudioConfig &getAudioConfig() const;

    bool valid() const;

    bool prepare();

    bool start();

    bool isStarted() const {
        return m_started;
    }

    // 获取最新的视频帧, 只有在 config 中没有配置 data_cb 的时候才能使用
    // @param requestNewest 是否忽略掉缓存的帧，只请求最新的帧
    const DSVideoFrame *obtainVideoFrame(bool requestNewest = false);

    // 获取最新的音频帧, 只有在 config 中没有配置 data_cb 的时候才能使用
    // @param requestNewest 是否忽略掉缓存的帧，只请求最新的帧
    const DSAudioSample *obtainAudioSample(bool requestNewest = false);

    void stop();

    bool reset();

    void release();

private:
    DSVideoConfig m_video_cfg;

    DSAudioConfig m_audio_cfg;

    void *m_context = nullptr;
    bool m_started = false;

    DSVideoFrameQueue m_video_frame_queue;
    DSAudioSampleQueue m_audio_sample_queue;
};

class DSCamDeviceMgr {
public:
    bool valid() const { return !m_video_devices.empty(); }

    /**
     * 更新设备列表
     */
    void updateDeviceList() {
        m_video_devices = DSCaptureMgr::enumVideoDevices();
        // 默认选择第一个设备，如果当前有选择的设备，那就查找当前设备，如果找到了就选择当前设备
        DSVideoDeviceInfo oldDev;
        for (auto & d : m_video_devices) {
            if (d.name() == m_video_cfg.name && d.path() == m_video_cfg.path) {
                oldDev = d;
                break;
            }
        }
        if (!oldDev.valid() && !m_video_devices.empty()) {
            oldDev = m_video_devices[0];
        }
        setDevice(oldDev);
    }

    const std::vector<DSVideoDeviceInfo>& getAllDevices() const {
        return m_video_devices;
    }

    std::vector<std::string> getAllDeviceNames() const {
        std::vector<std::string> names;
        for (auto &d : m_video_devices) {
            names.push_back(d.name());
        }
        return names;
    }

    DSVideoDeviceInfo findDevice(const std::string &name) const {
        for (auto &d : m_video_devices) {
            if (d.name() == name) {
                return d;
            }
        }
        return DSVideoDeviceInfo();
    }

    std::vector<DSVideoFmt> getSupportedFormats() const {
        return m_cur_device.supportedFormats();
    }

    std::vector<DSVideoStreamInfo> getStreams() const {
        return m_cur_device.getStreams(m_video_cfg.internal_fmt);
    }

public:

    const DSVideoDeviceInfo& currentDevice() {
        return m_cur_device;
    }

    void setDevice(const DSVideoDeviceInfo &device) {
        m_cur_device = device;
        // 更新配置, 如果当前配置不支持当前设备，那么选择第一个支持的配置
        m_video_cfg.name = device.name();
        m_video_cfg.path = device.path();
        // 判断分辨率是否支持
        auto formats = device.supportedFormats();
        if (formats.empty()) {
            return;
        }
        m_video_cfg.internal_fmt = formats[0];
        m_video_cfg.desire_fmt = formats[0];
        auto streams = device.getStreams(m_video_cfg.internal_fmt);
        if (!streams.empty()) {
            m_video_cfg.width = streams[0].minWidth();
            m_video_cfg.height = streams[0].minHeight();
            m_video_cfg.frame_interval = streams[0].minInterval();
        } else {
            m_video_cfg.width = 0;
            m_video_cfg.height = 0;
        }
    }

    void setCfgSize(int width, int height) {
        m_video_cfg.width = width;
        m_video_cfg.height = height;
    }

    void setCfgFlip(bool flip) {
        m_video_cfg.flip = flip;
    }

    void setCfgFrameInterval(int64_t interval) {
        m_video_cfg.frame_interval = interval;
    }

    void setCfgInternalFmt(DSVideoFmt fmt) {
        m_video_cfg.internal_fmt = fmt;

        auto streams = m_cur_device.getStreams(m_video_cfg.internal_fmt);
        // 判断分辨率是否支持
        bool found = false;
        for (auto &stream : streams) {
            if (stream.isValidSize(m_video_cfg.width, m_video_cfg.height)) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (!streams.empty()) {
                m_video_cfg.width = streams[0].minWidth();
                m_video_cfg.height = streams[0].minHeight();
            } else {
                m_video_cfg.width = 0;
                m_video_cfg.height = 0;
            }
        }
    }

    void setCfgDesireFmt(DSVideoFmt fmt) {
        m_video_cfg.desire_fmt = fmt;
    }

    std::string getCfgName() const {
        return m_video_cfg.name;
    }

    int getCfgWidth() const {
        return m_video_cfg.width;
    }

    int getCfgHeight() const {
        return m_video_cfg.height;
    }

    std::string getCfgSizeString() const {
        return std::to_string(m_video_cfg.width) + "x" + std::to_string(m_video_cfg.height);
    }

    DSVideoFmt getCfgInternalFmt() const {
        return m_video_cfg.internal_fmt;
    }

    std::string getCfgInternalFmtString() const {
        return DSUtils::videoFmtString((int) m_video_cfg.internal_fmt);
    }

    DSVideoFmt getCfgDesireFmt() const {
        return m_video_cfg.desire_fmt;
    }

    std::string getCfgDesireFmtString() const {
        return DSUtils::videoFmtString((int) m_video_cfg.desire_fmt);
    }

    const DSVideoConfig &getConfig() const {
        return m_video_cfg;
    }

public:
    bool openCamera() {
        if (capture_mgr == nullptr) {
            capture_mgr = new DSCaptureMgr();
        } else {
            capture_mgr->reset();
        }
        capture_mgr->setVideoConfig(m_video_cfg);
        _ERROR_IF(!capture_mgr->prepare(), "prepare failed");
        _ERROR_RETURN_IF(!capture_mgr->start(), false, "start capture failed!");
        return true;
    }

    bool isOpened() const {
        return capture_mgr != nullptr && capture_mgr->isStarted();
    }

    void closeCamera() {
        if (capture_mgr != nullptr) {
            capture_mgr->stop();
            delete capture_mgr;
            capture_mgr = nullptr;
        }
    }

    // 获取最新的视频帧, 只有在 config 中没有配置 data_cb 的时候才能使用
    const DSVideoFrame *obtainFrame(bool requestNewest = false) {
        return capture_mgr ? capture_mgr->obtainVideoFrame(requestNewest) : nullptr;
    }

private:
    std::vector<DSVideoDeviceInfo> m_video_devices;
    DSVideoDeviceInfo m_cur_device;

    DSVideoConfig m_video_cfg;

    std::mutex m_mutex;
    DSVideoFrame m_frame;
    DSCaptureMgr *capture_mgr = nullptr;
};

NAMESPACE_END
