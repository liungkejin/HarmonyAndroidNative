//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CamDevice.h"
#include "CamManager.h"
#include "CamUtils.h"
#include <vector>

NAMESPACE_DEFAULT

static const CamProfile *findExpectProfile(const std::vector<CamProfile> &profiles,
                                           int expectWidth, int expectHeight, int minSize = -1, int maxSize = -1, int expectFormat = -1) {
    const CamProfile *result = nullptr;
    long delta = LONG_MAX;
    for (auto &p : profiles) {
        int width = (int) p.width;
        int height = (int) p.height;
        if (minSize > 0 && std::min(width, height) < minSize) {
            continue;
        }
            
        if (maxSize > 0 && std::max(width, height) > maxSize) {
            continue;
        }
        
        // 目前format没有用，所有的都是 JPEG 格式，但是内部又都是 NV21 格式
        if (width * expectHeight != height * expectWidth) {
            continue;
        }
        if (expectFormat != -1 && p.format != expectFormat) {
            continue;
        }
        if (result == nullptr) {
            result = &p;
            continue;
        }
        long d = std::abs(width - expectWidth);
        if (d < delta) {
            result = &p;
            delta = d;
        }
    }
    return result;
}


CamOutputCapability::CamOutputCapability(const Camera_OutputCapability *cap) {
    if (cap == nullptr) {
        return;
    }
    for (int i = 0; i < cap->previewProfilesSize; ++i) {
        Camera_Profile *profile = cap->previewProfiles[i];
        //_INFO("preview profile %d %dx%d", profile->format, profile->size.width, profile->size.height);
        m_preview_profiles.emplace_back(*profile);
    }
    for (int i = 0; i < cap->photoProfilesSize; ++i) {
        Camera_Profile *profile = cap->photoProfiles[i];
        //_INFO("photo profile %d %dx%d", profile->format, profile->size.width, profile->size.height);
        m_photo_profiles.emplace_back(*profile);
    }
    for (int i = 0; i < cap->videoProfilesSize; ++i) {
        Camera_VideoProfile *profile = cap->videoProfiles[i];
        //_INFO("video profile %d %dx%d", profile->format, profile->size.width, profile->size.height);
        m_video_profiles.emplace_back(*profile);
    }
    if (cap->supportedMetadataObjectTypes) {
        for (int i = 0; i < cap->metadataProfilesSize; ++i) {
            m_metadata_types.push_back((*cap->supportedMetadataObjectTypes)[i]);
        }
    } else {
        _WARN_IF(cap->metadataProfilesSize, 
            "Error metadataProfilesSize(%d) supportedMetadataObjectTypes == nullptr", cap->metadataProfilesSize)
    }
}

const CamProfile *CamOutputCapability::findPreviewProfile(int expectWidth, int expectHeight, int minSize, int maxSize, int format) {
    return findExpectProfile(m_preview_profiles, expectWidth, expectHeight, minSize, maxSize, format);
}

const CamProfile *CamOutputCapability::findPhotoProfile(int expectWidth, int expectHeight, int minSize, int maxSize, int format) {
    return findExpectProfile(m_photo_profiles, expectWidth, expectHeight, minSize, maxSize, format);
}

const CamProfile *CamOutputCapability::findVideoProfile(int expectWidth, int expectHeight, int minSize, int maxSize, int format) {
    return findExpectProfile(m_video_profiles, expectWidth, expectHeight, minSize, maxSize, format);
}

bool CamOutputCapability::isMetadataSupported(Camera_MetadataObjectType type) {
    for (auto &t : m_metadata_types) { // NOLINT(*-use-anyofallof)
        if (t == type) {
            return true;
        }
    }
    return false;
}

void CamOutputCapability::dump() const {
    _INFO("preview profile size: %d", m_preview_profiles.size());
    int index = 0;
    for (auto &profile : m_preview_profiles) {
        _INFO("preview profile[%d]: format(%s) size(%dx%d)", index, 
            CamUtils::formatStr(profile.format), profile.width, profile.height);
        index += 1;
    }
    _INFO("photo profile size: %d", m_photo_profiles.size());
    index = 0;
    for (auto &profile : m_photo_profiles) {
        _INFO("photo profile[%d]: format(%s) size(%dx%d)", index,
            CamUtils::formatStr(profile.format), profile.width, profile.height);
        index += 1;
    }
    _INFO("video profile size: %d", m_video_profiles.size());
    index = 0;
    for (auto &profile : m_video_profiles) {
        _INFO("video profile[%d]: format(%s) size(%dx%d) fps-range[%d,%d]", index,
              CamUtils::formatStr(profile.format), profile.width, profile.height, profile.minFps, profile.maxFps);
        index += 1;
    }
    _INFO("metadata profile size: %d", m_metadata_types.size());
    index = 0;
    for (auto &type : m_metadata_types) {
        _INFO("metadata profile[%d]: type(%s)", index, CamUtils::metadataObjectType(type));
        index += 1;
    }
}


////////////////////////////////////////////////////////////////
CamDevice::CamDevice()
    : cam_position(CAMERA_POSITION_BACK), cam_type(CAMERA_TYPE_DEFAULT),
      cam_connection_type(CAMERA_CONNECTION_BUILT_IN) {
    cam_id = new char[1];
    cam_id[0] = 0;
}

CamDevice::CamDevice(const Camera_Device *d)
    : cam_position(d->cameraPosition), cam_type(d->cameraType), cam_connection_type(d->connectionType) {
    auto size = strlen(d->cameraId);
    cam_id = new char[size + 1];
    strcpy(cam_id, d->cameraId);
}

CamDevice::CamDevice(const CamDevice &d)
    : cam_position(d.cam_position), cam_type(d.cam_type), cam_connection_type(d.cam_connection_type) {
    cam_id = new char[strlen(d.cam_id) + 1];
    strcpy(cam_id, d.cam_id);
}

CamDevice::~CamDevice() {
    delete[] cam_id;
    cam_id = nullptr;
}

CamDevice& CamDevice::operator=(const Camera_Device *d) {
    cam_position = d->cameraPosition;
    cam_type = d->cameraType;
    cam_connection_type = d->connectionType;
    delete[] cam_id;
    auto size = strlen(d->cameraId);
    cam_id = new char[size + 1];
    strcpy(cam_id, d->cameraId);
    return *this;
}

CamDevice& CamDevice::operator=(const CamDevice &d) {
    if (this == &d) {
        return *this;
    }
    cam_position = d.cam_position;
    cam_type = d.cam_type;
    cam_connection_type = d.cam_connection_type;
    delete[] cam_id;
    auto size = strlen(d.cam_id);
    cam_id = new char[size + 1];
    strcpy(cam_id, d.cam_id);
    return *this;
}

CamOutputCapability *CamDevice::getOutputCapability() const { return CamManager::getCameraOutputCapability(*this); }

NAMESPACE_END