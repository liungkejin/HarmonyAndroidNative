/**
* Copy right (c) 2020-2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#include "directshow_camera/device/ds_camera_device.h"

#include <algorithm>

namespace DirectShowCamera
{

#pragma region Constructor and Destructor

    DirectShowCameraDevice::DirectShowCameraDevice()
    {
    }

    DirectShowCameraDevice::DirectShowCameraDevice(
        const std::string friendlyName,
        const std::string description,
        const std::string devicePath,
        const std::vector<DirectShowVideoFormat> videoFormats
    )
    {
        m_videoFormats = videoFormats;
        m_friendlyName = friendlyName;
        m_description = description;
        m_devicePath = devicePath;    
    }

#pragma endregion Constructor and Destructor

#pragma region Getter

    bool DirectShowCameraDevice::valid() const {
        //不能判断path是否为空，因为有的设备没有path
        for (auto &f : m_videoFormats) {
            if (f.valid()) {
                return true;
            }
        }
        return false;
    }


    std::vector<DirectShowVideoFormat> DirectShowCameraDevice::getDirectShowVideoFormats() const
    {
        return m_videoFormats;
    }

    std::vector<DirectShowVideoFormat> DirectShowCameraDevice::getAllSupportedUniqueVideoFormats(bool filterInvalid) const {
        std::vector<DirectShowVideoFormat> result;
        for (auto &f : m_videoFormats) {
            if (filterInvalid && !f.valid()) {
                continue;
            }
            bool added = false;
            for (auto &r : result) {
                if (r.getVideoType() == f.getVideoType()) {
                    added = true;
                    break;
                }
            }
            if (!added) {
                result.push_back(f);
            }
        }
        return result;
    }

    std::vector<DirectShowVideoFormat> DirectShowCameraDevice::getAllResolutionOfFormat(const GUID &videoFormat) const {
        std::vector<DirectShowVideoFormat> result;
        for (auto &f : m_videoFormats) {
            if (f.getVideoType() == videoFormat) {
                result.push_back(f);
            }
        }
        // sort by width big to small
        std::sort(result.begin(), result.end(), [](const DirectShowVideoFormat &a, const DirectShowVideoFormat &b) {
            return a.getTotalSize() > b.getTotalSize();
        });
        return result;
    }



    std::string DirectShowCameraDevice::getFriendlyName() const
    {
        return m_friendlyName;
    }

    std::string DirectShowCameraDevice::getDescription() const
    {
        return m_description;
    }

    std::string DirectShowCameraDevice::getDevicePath() const
    {
        return m_devicePath;
    }

#pragma endregion Getter

}
