//
// Created by kejin on 25-1-23.
//

#include "DSCaptureMgr.h"

#include <dshowcapture.hpp>

NAMESPACE_DEFAULT

std::vector<DSVideoDeviceInfo> DSCaptureMgr::enumVideoDevices() {
    std::vector<DShow::VideoDevice> devices;
    bool result = DShow::Device::EnumVideoDevices(devices);
    if (!result) {
        _ERROR("DSCaptureMgr::enumVideoDevices() failed");
        return {};
    }

    // convert to DSVideoDeviceInfo
    _INFO("enumVideoDevices() count=%d", devices.size());
    int index = 0;
    std::vector<DSVideoDeviceInfo> dsDevices;
    for (auto &device: devices) {
        DSVideoDeviceInfo dsDevice;
        dsDevice.setDeviceId(device.name, device.path);
        dsDevice.setAudioAttached(device.audioAttached);
        dsDevice.setSeparateAudioFilter(device.separateAudioFilter);
        for (auto &cap: device.caps) {
            DSVideoStreamInfo dsStream;
            dsStream.setMinSize(cap.minCX, cap.minCY);
            dsStream.setMaxSize(cap.maxCX, cap.maxCY);
            dsStream.setGranularity(cap.granularityCX, cap.granularityCY);
            dsStream.setInterval(cap.minInterval, cap.maxInterval);
            dsStream.setFormat((int) cap.format);
            dsDevice.addStream(dsStream);
        }
        dsDevices.push_back(dsDevice);
        _INFO("video device[%d]: %s", index, dsDevice.toString());
        index++;
    }

    return dsDevices;
}

std::vector<DSAudioDeviceInfo> DSCaptureMgr::enumAudioDevices() {
    std::vector<DShow::AudioDevice> devices;
    bool result = DShow::Device::EnumAudioDevices(devices);
    if (!result) {
        _ERROR("DSCaptureMgr::enumAudioDevices() failed");
        return {};
    }

    std::vector<DSAudioDeviceInfo> dsDevices;
    for (auto &d : devices) {
        DSAudioDeviceInfo dsDevice;
        dsDevice.m_name = WinUtils::wstringToString(d.name);
        dsDevice.m_path = WinUtils::wstringToString(d.path);
        for (auto &cap : d.caps) {
            DSAudioStreamInfo stream;
            stream.m_min_channels = cap.minChannels;
            stream.m_max_channels = cap.maxChannels;
            stream.m_min_sample_rate = cap.minSampleRate;
            stream.m_max_sample_rate = cap.maxSampleRate;
            stream.m_channels_granularity = cap.channelsGranularity;
            stream.m_sample_rate_granularity = cap.sampleRateGranularity;
            stream.m_format = (DSAudioFmt) cap.format;
            dsDevice.m_streams.push_back(stream);
        }
    }

    return dsDevices;
}

DSCaptureMgr::DSCaptureMgr(bool init) {
    m_context = new DShow::Device(init ? DShow::InitGraph::True : DShow::InitGraph::False);
}

DSCaptureMgr::~DSCaptureMgr() {
    release();
}

bool DSCaptureMgr::setVideoConfig(const DSVideoConfig &config) {
    DShow::Device &device = *(DShow::Device *) m_context;

    DShow::VideoConfig videoConfig;
    videoConfig.callback = [this](const DShow::VideoConfig &cfg, unsigned char *data, size_t size, long long startTime, long long stopTime, long rotation) {
        _INFO("video frame callback: size=%d, startTime=%lld, stopTime=%lld, rotation=%ld", size, startTime, stopTime, rotation);
        if (m_video_cfg.data_cb) {
            DSVideoFrame frame = {
                cfg.cx,
                cfg.cy_abs,
                (DSVideoFmt) cfg.format,
                data,
                size,
                cfg.frameInterval,
                startTime,
                stopTime,
                rotation
            };
            m_video_cfg.data_cb(frame);
        } else {
            // copy data
            m_video_frame_queue.pushFrame(
                cfg.cx,
                cfg.cy_abs,
                (DSVideoFmt) cfg.format,
                cfg.frameInterval,
                data,
                size,
                startTime,
                stopTime,
                rotation
            );
        }
    };
    videoConfig.reactivateCallback = [this]() {
        if (m_video_cfg.reactivate_cb) {
            m_video_cfg.reactivate_cb();
        }
    };
    videoConfig.cx = config.width;
    videoConfig.cy_abs = config.height;
    videoConfig.cy_flip = config.flip;
    videoConfig.useDefaultConfig = false;
    videoConfig.frameInterval = config.frame_interval;
    videoConfig.internalFormat = (DShow::VideoFormat) config.internal_fmt;
    videoConfig.format = (DShow::VideoFormat) config.desire_fmt;

    videoConfig.name = WinUtils::stringToWString(config.name);
    videoConfig.path = WinUtils::stringToWString(config.path);
    if (!device.SetVideoConfig(&videoConfig)) {
        _ERROR("DSCaptureMgr::setVideoConfig() failed");
        m_video_cfg = {};
        return false;
    }
    m_video_cfg = config;
    // 这里之所以再赋值，是因为设置完成之后，可能会有一些参数被修改
    m_video_cfg.width = videoConfig.cx;
    m_video_cfg.height = videoConfig.cy_abs;
    m_video_cfg.flip = videoConfig.cy_flip;
    m_video_cfg.frame_interval = videoConfig.frameInterval;
    m_video_cfg.internal_fmt = (DSVideoFmt) videoConfig.internalFormat;
    m_video_cfg.desire_fmt = (DSVideoFmt) videoConfig.format;
    return true;
}

const DSVideoConfig &DSCaptureMgr::getVideoConfig() const {
    return m_video_cfg;
}

bool DSCaptureMgr::setAudioConfig(const DSAudioConfig &config) {
    DShow::Device &device = *(DShow::Device *) m_context;

    DShow::AudioConfig audioConfig;
    audioConfig.callback = [this](const DShow::AudioConfig &cfg, unsigned char *data, size_t size, long long startTime, long long stopTime) {
        if (m_audio_cfg.data_cb) {
            DSAudioSample sample = {
                cfg.sampleRate,
                cfg.channels,
                (DSAudioFmt) cfg.format,
                data,
                size,
                startTime,
                stopTime
            };
            m_audio_cfg.data_cb(sample);
        } else {
            // copy data
            m_audio_sample_queue.pushSample(cfg.sampleRate, cfg.channels, (DSAudioFmt) cfg.format, data, size, startTime, stopTime);
        }
    };
    audioConfig.useVideoDevice = config.use_video_device;
    audioConfig.useSeparateAudioFilter = config.use_separate_audio_filter;
    audioConfig.sampleRate = config.sample_rate;
    audioConfig.channels = config.channels;
    audioConfig.useDefaultConfig = false;
    audioConfig.format = (DShow::AudioFormat) config.format;
    audioConfig.name = WinUtils::stringToWString(config.name);
    audioConfig.path = WinUtils::stringToWString(config.path);
    if (!device.SetAudioConfig(&audioConfig)) {
        _ERROR("DSCaptureMgr::setAudioConfig() failed");
        m_audio_cfg = {};
        return false;
    }
    m_audio_cfg = config;
    m_audio_cfg.sample_rate = audioConfig.sampleRate;
    m_audio_cfg.channels = audioConfig.channels;
    m_audio_cfg.format = (DSAudioFmt) audioConfig.format;
    return true;
}

const DSAudioConfig &DSCaptureMgr::getAudioConfig() const {
    return m_audio_cfg;
}

bool DSCaptureMgr::valid() const {
    return m_context != nullptr && ((DShow::Device *) m_context)->Valid();
}

bool DSCaptureMgr::prepare() {
    DShow::Device &device = *(DShow::Device *) m_context;
    return device.ConnectFilters();
}

bool DSCaptureMgr::start() {
    DShow::Device &device = *(DShow::Device *) m_context;
    if (device.Start() == DShow::Result::Error) {
        return false;
    }
    m_started = true;
    return true;
}

const DSVideoFrame *DSCaptureMgr::obtainVideoFrame(bool requestNewest) {
    return m_video_frame_queue.popFrame(requestNewest);
}


const DSAudioSample *DSCaptureMgr::obtainAudioSample(bool requestNewest) {
    return m_audio_sample_queue.popSample(requestNewest);
}


void DSCaptureMgr::stop() {
    DShow::Device &device = *(DShow::Device *) m_context;
    device.Stop();
    m_started = false;
}

bool DSCaptureMgr::reset() {
    m_started = false;
    DShow::Device &device = *(DShow::Device *) m_context;
    return device.ResetGraph();
}

void DSCaptureMgr::release() {
    m_audio_cfg = {};
    m_video_cfg = {};
    if (m_context) {
        if (m_started) {
            stop();
        }

        delete (DShow::Device *) m_context;
        m_context = nullptr;
    }
}

NAMESPACE_END