//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <cstdint>
#include <ohcamera/camera_manager.h>
#include <string>
#include <vector>
#include "CamUtils.h"

NAMESPACE_DEFAULT

struct CamProfile {
public:
    CamProfile() : format(CAMERA_FORMAT_YUV_420_SP), width(0), height(0), minFps(0), maxFps(0) {}

    explicit CamProfile(const Camera_Profile &p)
            : format(p.format), width(p.size.width), height(p.size.height), minFps(0), maxFps(0) {}

    explicit CamProfile(const Camera_VideoProfile &p)
            : format(p.format), width(p.size.width), height(p.size.height), minFps(p.range.min), maxFps(p.range.max) {}

    CamProfile(const CamProfile &other)
            : format(other.format), width(other.width), height(other.height), minFps(other.minFps),
              maxFps(other.maxFps) {}

    CamProfile &operator=(const CamProfile &other) {
        format = other.format;
        width = other.width;
        height = other.height;
        minFps = other.minFps;
        maxFps = other.maxFps;
        return *this;
    }

    inline std::string toString() const {
        std::string result;
        result.append("{").append(CamUtils::formatStr(format));
        result.append(", ").append(std::to_string(width)).append("x").append(std::to_string(height));
        if (minFps != 0 || maxFps != 0) {
            result.append(", fps[")
                    .append(std::to_string(minFps))
                    .append(",")
                    .append(std::to_string(maxFps))
                    .append("]");
        }
        result.append("}");
        return result;
    }

public:
    Camera_Format format;
    uint32_t width;
    uint32_t height;
    uint32_t minFps;
    uint32_t maxFps;
};

// 封装 Camera_OutputCapability
class CamOutputCapability {
public:
    explicit CamOutputCapability(const Camera_OutputCapability *cap);

    ~CamOutputCapability() = default;

public:
    inline size_t previewProfileSize() const { return m_preview_profiles.size(); }

    inline size_t photoProfileSize() const { return m_photo_profiles.size(); }

    inline size_t videoProfileSize() const { return m_video_profiles.size(); }

    inline size_t metadataTypeSize() const { return m_metadata_types.size(); }

    /**
     * 找到最接近 expectWidth,expectHeight的同比例的预览分辨率
     */
    const CamProfile *findPreviewProfile(int expectWidth, int expectHeight, int minSize, int maxSize, int format = -1);

    /**
     * 找到最接近的拍照分辨率
     */
    const CamProfile *findPhotoProfile(int expectWidth, int expectHeight, int minSize, int maxSize, int format = -1);

    /**
     * 找到最接近的视频录制分辨率
     */
    const CamProfile *findVideoProfile(int expectWidth, int expectHeight, int minSize, int maxSize, int format = -1);

    // 判定是否支持metadata
    bool isMetadataSupported(Camera_MetadataObjectType type);

    // print log
    void dump() const;

private:
    std::vector<CamProfile> m_preview_profiles;
    std::vector<CamProfile> m_photo_profiles;
    std::vector<CamProfile> m_video_profiles;
    std::vector<Camera_MetadataObjectType> m_metadata_types;
};

// 封装 Camera_Device
class CamDevice {
public:
    CamDevice();

    explicit CamDevice(const Camera_Device *d);

    CamDevice(const CamDevice &d);

    ~CamDevice();

public:

    CamDevice &operator=(const Camera_Device *d);

    CamDevice &operator=(const CamDevice &d);

    bool operator==(const CamDevice &other) const {
        return cam_id != nullptr && other.cam_id != nullptr && strcmp(cam_id, other.cam_id) == 0;
    }

    bool operator!=(const CamDevice &other) const { return !(*this == other); }

    bool operator<(const CamDevice &other) const { return strcmp(cam_id, other.cam_id) < 0; }

    bool operator>(const CamDevice &other) const { return strcmp(cam_id, other.cam_id) > 0; }

    inline char *id() const { return cam_id; }

    inline Camera_Position position() const { return cam_position; }

    inline Camera_Type type() const { return cam_type; }

    inline Camera_Connection connectionType() const { return cam_connection_type; }

    CamOutputCapability *getOutputCapability() const;

    inline std::string toString() const {
        std::string str;
        str.append("{id: ").append(id()).append(", ");
        str.append("position: ").append(CamUtils::positionStr(cam_position)).append(", ");
        str.append("type: ").append(CamUtils::typeStr(cam_type)).append(", ");
        str.append("connection: ").append(CamUtils::connectionStr(cam_connection_type)).append("}");
        return str;
    }

private:
    Camera_Position cam_position;
    Camera_Type cam_type;
    Camera_Connection cam_connection_type;

    // 这里之所以不使用 const 是因为 Camera_Device 的 cameraId 是 char *
    char *cam_id;
};

class CamDeviceList {
public:
    CamDeviceList() {}

    CamDeviceList(const CamDeviceList &other) {
        m_list.clear();
        for (const auto &device: other.m_list) {
            m_list.push_back(device);
        }
    }

    void add(const Camera_Device *d) { m_list.emplace_back(d); }

    void add(const CamDevice &d) { m_list.push_back(d); }

    inline size_t size() const { return m_list.size(); }

    const CamDevice *findBackWideAngleCamera() {
        return findCamera(CAMERA_POSITION_BACK, Camera_Type::CAMERA_TYPE_WIDE_ANGLE);
    }

    const CamDevice *findBackCamera(Camera_Type type = Camera_Type::CAMERA_TYPE_DEFAULT) {
        return findCamera(CAMERA_POSITION_BACK, type);
    }

    const CamDevice *findFrontWideAngleCamera() {
        return findCamera(CAMERA_POSITION_FRONT, Camera_Type::CAMERA_TYPE_WIDE_ANGLE);
    }

    const CamDevice *findFrontCamera(Camera_Type type = Camera_Type::CAMERA_TYPE_DEFAULT) {
        return findCamera(CAMERA_POSITION_FRONT, type);
    }

    const CamDevice *findCamera(Camera_Position pos, Camera_Type type) {
        for (const auto &device: m_list) {
            if (device.position() == pos && device.type() == type) {
                return &device;
            }
        }
        return nullptr; // not found
    }

    void dump() const {
        _INFO(">>>Dumping camera device list, size(%d)", m_list.size());
        int index = 0;
        for (const auto &device: m_list) {
            std::string s = device.toString();
            _INFO("camera(%d): %s", index, s.c_str());
            index += 1;
        }
        _INFO(">>>Dumping camera device list done.");
    }

private:
    std::vector<CamDevice> m_list;
};


NAMESPACE_END