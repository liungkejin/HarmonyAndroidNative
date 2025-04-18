/**
* Copy right (c) 2020-2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#pragma once
#ifndef DIRECTSHOW_CAMERA__DIRECTSHOW_CAMERA__CAMERA__DIRECTSHOW_CAMERA_ABSTRACT_H
#define DIRECTSHOW_CAMERA__DIRECTSHOW_CAMERA__CAMERA__DIRECTSHOW_CAMERA_ABSTRACT_H

//************Content************
#include "directshow_camera/properties/ds_camera_properties.h"

#include "directshow_camera/video_format/ds_video_format.h"

#include "directshow_camera/device/ds_camera_device.h"

#include <optional>

#include "frame/frame.h"

namespace DirectShowCamera
{
    /**
     * @brief Directshow Camera. This is the core of this project.
     *
     */
    class AbstractDirectShowCamera
    {
    public:
        virtual ~AbstractDirectShowCamera() { }

        virtual void Release() = 0;

        virtual bool Open(
            IBaseFilter** directShowFilter,
            std::optional<const DirectShowVideoFormat> videoFormat = std::nullopt,
            // If true, the output data will be converted to RGB24 if the format support it.
            bool convertOutputDataToRGB24IfSupported = false
        ) = 0;
        virtual void Close() = 0;
        virtual bool isOpening() const = 0;
        virtual bool isDisconnecting() = 0;
        virtual void setDisconnectionProcess(std::function<void()> func) = 0;

        virtual bool Start() = 0;
        virtual bool Stop() = 0;
        virtual bool isCapturing() const = 0;

        virtual bool getFrame(Frame &frame, bool onlyGetNewFrame, int lastFrameIndex) = 0;

        virtual unsigned long getLastFrameIndex() const = 0;
        virtual void setMinimumFPS(const double minimumFPS) = 0;
        virtual double getFPS() const = 0;
        virtual long getFrameTotalSize() const = 0;
        virtual GUID getFrameType() const = 0;

        // Video Format
        virtual std::vector<DirectShowVideoFormat> getVideoFormatList() const = 0;
        virtual int getCurrentVideoFormatIndex() const = 0;
        virtual DirectShowVideoFormat getCurrentVideoFormat() const = 0;
        virtual DirectShowVideoFormat getCurrentGrabberFormat() const = 0;

        virtual bool setVideoFormat(const DirectShowVideoFormat videoFormat) = 0;
        virtual bool setVideoFormat(const int videoFormatIndex) = 0;

        // Property
        virtual void RefreshProperties() = 0;
        virtual std::shared_ptr<DirectShowCameraProperties> getProperties() const = 0;

        virtual void ResetPropertiesToDefault(const bool asAuto = true) = 0;
        virtual bool setPropertyValue(
            std::shared_ptr<DirectShowCameraProperty>& property,
            const long value,
            const bool isAuto
        ) = 0;

        // Get camera
        virtual bool getCameras(std::vector<DirectShowCameraDevice>& cameraDevices) = 0;
        virtual bool getCamera(const int cameraIndex, IBaseFilter** directShowFilter) = 0;
        virtual bool getCamera(const std::string devicePath, IBaseFilter** directShowFilter) = 0;
        virtual bool getCamera(const DirectShowCameraDevice device, IBaseFilter** directShowFilter) = 0;

        virtual void ResetLastError() = 0;
        virtual std::string getLastError() const = 0;
    };
}


//*******************************

#endif