/**
* Copy right (c) 2020-2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#pragma once
#ifndef DIRECTSHOW_CAMERA__DIRECTSHOW_CAMERA__DEVICE__DS_CAMERA_DEVICE_H
#define DIRECTSHOW_CAMERA__DIRECTSHOW_CAMERA__DEVICE__DS_CAMERA_DEVICE_H

//************Content************
#include "directshow_camera/video_format/ds_video_format.h"

#include <string>
#include <vector>

namespace DirectShowCamera
{
    /**
     * @brief A Class to store DirectShow Camera Device information
     */
    class DirectShowCameraDevice
    {
    public:

#pragma region Constructor and Destructor
        DirectShowCameraDevice();

        /**
         * @brief Constructor
         *
         * @param[in] friendlyName Friendly Name
         * @param[in] description Description
         * @param[in] devicePath Device Path
         * @param[in] videoFormats Video Formats
         */
        DirectShowCameraDevice
        (
            const std::string friendlyName,
            const std::string description,
            const std::string devicePath,
            const std::vector<DirectShowVideoFormat> videoFormats
        );

#pragma endregion Constructor and Destructor

#pragma region Getter

        /**
         * 必须有一个可用的格式和分辨率
         * @return Return true if the camera device is valid
         */
        bool valid() const;

        /**
         * @brief Get the supported DirectShowVideoFormats
         *
         * @return Return the supported DirectShowVideoFormats
         */
        std::vector<DirectShowVideoFormat> getDirectShowVideoFormats() const;

        /**
         * 剔除掉无法识别的格式
         * @return Get all supported video formats
         */
        std::vector<DirectShowVideoFormat> getAllSupportedUniqueVideoFormats(bool filterInvalid = true) const;

        /**
         * @return 获取某一个格式的所有分辨率
         */
        std::vector<DirectShowVideoFormat> getAllResolutionOfFormat(const GUID& videoFormat) const;

        /**
         * @brief Get the camera friendly name
         *
         * @return Return the camera friendly name
         */
        std::string getFriendlyName() const;

        /**
         * @brief Get the camera description
         *
         * @return Return the camera description
         */
        std::string getDescription() const;

        /**
         * @brief Get the device path
         *
         * @return Return the device path
         */
        std::string getDevicePath() const;

#pragma endregion Getter

#pragma region operator

        bool operator == (const DirectShowCameraDevice& device) const
        {
            if (m_friendlyName != device.m_friendlyName) return false;
            if (m_description != device.m_description) return false;
            if (m_devicePath != device.m_devicePath) return false;
            if (m_videoFormats.size() != device.m_videoFormats.size()) return false;

            for (int i = 0; i < m_videoFormats.size(); i++) {
                if (m_videoFormats[i] != device.m_videoFormats[i])
                {
                    return false;
                }
            }

            return true;
        }

        std::string toString() const
        {
            std::string result = "Friend Name: " + m_friendlyName + "\n";
            result += "Description: " + m_description + "\n";
            result += "Device Path: " + m_devicePath + "\n";


            if (m_videoFormats.size() > 0)
            {
                result += "---Support Video Formats---\n";
                for (int i = 0; i < m_videoFormats.size();i++) {
                    result += (std::string)m_videoFormats[i] + "\n";
                }
            }        

            return result;
        }

        friend std::ostream& operator << (std::ostream& out, const DirectShowCameraDevice& obj) {
            return out << obj.toString();
        }

#pragma endregion operator

    private:
        std::vector<DirectShowVideoFormat> m_videoFormats;
        std::string m_friendlyName;
        std::string m_description;
        std::string m_devicePath;
    };
}

//*******************************

#endif