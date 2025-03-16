//
// Created by wutacam on 2025/1/13.
//

#pragma once

#include <strmif.h>
#include <common/Common.h>
#include <vector>
#include "CamUtils.h"

NAMESPACE_DEFAULT
class CamDevice;
/**
 * 表示一条相机流信息
 */
class CamStreamInfo {
    friend class CamDevice;

public:
    CamStreamInfo() = default;

    CamStreamInfo(const CamStreamInfo &o) : m_fmt(o.m_fmt), m_planes(o.m_planes), m_resolutions(o.m_resolutions) {
    }

    ~CamStreamInfo() = default;

    CamStreamInfo &operator=(const CamStreamInfo &o) {
        m_fmt = o.m_fmt;
        m_resolutions = o.m_resolutions;
        return *this;
    }

public:
    CamFmt format() const {
        return m_fmt;
    }

    int planes() const {
        return m_planes;
    }

    int sizeCount() const {
        return m_resolutions.size();
    }

    const CamSize &sizeAt(int i) {
        return m_resolutions[i];
    }

    std::string toString() {
        std::stringstream ss;
        ss << "CamStream[fmt: " << CamUtils::camFmtString(m_fmt) << ", planes: " << m_planes << ", sizes(" << m_resolutions.size() << "): ";
        for (auto &res : m_resolutions) {
            ss << res.width << "x" << res.height << ",";
        }
        ss << "]";
        return ss.str();
    }

private:
    void addCamSize(int width, int height) {
        m_resolutions.emplace_back(width, height);
    }

private:
    // stream format
    CamFmt m_fmt = CAM_FMT_UNSUPPORTED;
    // data number of plane
    int m_planes = 1;
    // all resolutions supported
    std::vector<CamSize> m_resolutions;
};

/**
 * 相机设备的几个基本属性
 * 1. 相机名称
 * 2. 所支持的所有的格式和分辨率
 */
class CamDevice {
public:
    static std::vector<CamDevice> enumAllDevices();

    static CamDevice getDevice(const std::string &id, const std::string &name, IBaseFilter *filter);

public:
    explicit CamDevice(const std::string &id, const std::string &name) : m_id(id), m_name(name) {
    }

    CamDevice(const CamDevice &o) : m_name(o.m_name), m_streams(o.m_streams) {
    }

    CamDevice &operator=(const CamDevice &device) {
        m_name = device.m_name;
        m_streams.clear();
        m_streams.assign(device.m_streams.begin(), device.m_streams.end());
        return *this;
    }

    const std::string &id() const {
        return m_id;
    }

    const std::string &name() const {
        return m_name;
    }

    int streamSize() const {
        return m_streams.size();
    }

    const CamStreamInfo &stream(int i) const {
        return m_streams[i];
    }

    CamStreamInfo *findStream(const CamFmt fmt) {
        for (auto &stream : m_streams) {
            if (stream.format() == fmt) {
                return &stream;
            }
        }
        return nullptr;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "CamDevice(" << m_name << ") streams(" << m_streams.size() << ") {\n";
        for (auto &stream : m_streams) {
            ss << "\t" << stream.toString() << "\n";
        }
        ss << "}";
        return ss.str();
    }

private:
    void addStreamInfo(const CamStreamInfo &streamInfo) {
        m_streams.emplace_back(streamInfo);
    }

private:
    std::string m_id;
    std::string m_name;
    std::vector<CamStreamInfo> m_streams;
};

NAMESPACE_END
