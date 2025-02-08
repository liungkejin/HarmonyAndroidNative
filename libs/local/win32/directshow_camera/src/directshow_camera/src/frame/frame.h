/**
* Copy right (c) 2020-2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#pragma once
#ifndef DIRECTSHOW_CAMERA__CAMERA__FRAME__FRAME_H
#define DIRECTSHOW_CAMERA__CAMERA__FRAME__FRAME_H

//************Content************

#include <guiddef.h>

#include "frame/frame_decoder.h"
#include "utils/gdi_plus_utils.h"

#include <memory>
#include <ostream>
#include <string>
#include <functional>
#include <filesystem>

namespace DirectShowCamera
{
    class Frame
    {
    public:
        enum FrameType {
            None,
            Unknown,
            Monochrome8bit,
            Monochrome16bit,
            ColorBGR24bit,
            ColorRGB24bit
        };

    public:


#pragma region Constructor and Destructor
        /**
        * @brief Constructor
        */
        Frame();

        /**
        * @brief Destructor
        */
        ~Frame();

        /**
        * @brief Copy constructor
        */
        Frame(const Frame& other);

        /**
        * @brief Move constructor
        */
        Frame(Frame&& other) noexcept;

        /**
        * @brief Clear the frame
        */
        void Clear();

#pragma endregion Constructor and Destructor

#pragma region Frame
        void setInfo(int width, int height, const GUID &frameType);

        void putData(int frameIndex, const unsigned char *data, int dataSize);

        /**
         * @brief   Get frame data pointer. This is the data pointer in the Frame object which is in the order of pixel by pixel (BGR if color),
         *          row by row and is vertical flipped. Don't release the pointer. The pointer will be released when the frame is destroyed.
         * @param[out] numOfBytes   Number of bytes of the frame.
         * @return Return the frame in bytes (BGR if color) which is vertical flipped.
        */
        unsigned char* getFrameDataPtr(int& numOfBytes);

        /**
        * @brief    Return a cloned frame data. The data is in the order of pixel by pixel, row by row.
        *           You will need to know the width, height and frame type to decode the data.
        * @param[out] numOfBytes   Number of bytes of the frame.
        * @return Return the frame in bytes
        */
        std::shared_ptr<unsigned char[]> getFrameData(int& numOfBytes, bool flipVertical, bool rgb2bgr);

        /**
        * @brief    Return a cloned frame 16 bit data. The data is in the order of pixel by pixel, row by row.
        *           You will need to know the width, height and frame type to decode the data.
        * @param[out] numOfBytes   Number of bytes of the frame.
        * @return Return the frame in bytes
        */
        std::shared_ptr<unsigned short[]> getFrame16bitData(int& numOfBytes, bool flipVertical);

#pragma endregion Frame

#pragma region Getter

        /**
        * @brief Return true if the frame is empty
        * @return Return true if the frame is empty
        */
        bool isEmpty() const;

        /**
         * @brief Return the last frame index. It use to identify whether a new frame. Index will only be updated when you call getFrame() or gatMat();
         * @return Return the last frame index.
        */
        unsigned long getFrameIndex() const;

        /**
         * @brief Get the frame width in pixel
         * @return Return the frame width
        */
        int getWidth() const;

        /**
         * @brief Get the frame height in pixel
         * @return Return the frame height
        */
        int getHeight() const;

        /**
         * @brief Get frame size in bytes
         * @return Return the the frame size in bytes
        */
        int getFrameSize() const;

        const GUID& getRawFrameType() const;

        /**
        * @brief Get the frame type
        * @return Return the frame type
        */
        FrameType getFrameType() const;

        /**
        * @brief Get the frame settings
        * @return Return the frame settings by reference so that you can change the settings.
        */
        FrameSettings& getFrameSettings();

#pragma endregion Getter

#ifdef WITH_OPENCV2
#pragma region OpenCV

        /**
         * @brief Get cv::Mat of the current frame
         * @return Return cv::Mat
        */
        cv::Mat getMat(bool flipVertical, bool rgb2bgr);

#pragma endregion OpenCV
#endif

#pragma region Operator

        /**
        * @brief Copy assignment
        */
        Frame& operator=(const Frame& other)
        {
            if (this != &other)
            {
                m_width = other.m_width;
                m_height = other.m_height;
                m_dataSize = other.m_dataSize;
                m_frameIndex = other.m_frameIndex;
                m_frameType = other.m_frameType;
                if (other.m_data != nullptr)
                {
                    m_data = std::make_unique<unsigned char[]>(m_dataSize);
                    memcpy(m_data.get(), other.m_data.get(), m_dataSize);
                }
            }
            return *this;
        }


        /**
        * @brief Move assignment
        */
        Frame& operator=(Frame&& other) noexcept
        {
            if (this != &other)
            {
                m_width = other.m_width;
                m_height = other.m_height;
                m_dataSize = other.m_dataSize;
                m_frameIndex = other.m_frameIndex;
                m_frameType = other.m_frameType;
                m_data = std::move(other.m_data);
            }
            return *this;
        }

        /**
        * @brief equal operator
        */
        bool operator==(const Frame& other) const
        {
            if (m_width != other.m_width) return false;
            if (m_height != other.m_height) return false;
            if (m_dataSize != other.m_dataSize) return false;
            if (m_frameIndex != other.m_frameIndex) return false;
            if (m_frameType != other.m_frameType) return false;

            if (m_data == nullptr && other.m_data != nullptr) return false;
            if (m_data != nullptr && other.m_data == nullptr) return false;
            if (m_data != nullptr && other.m_data != nullptr)
            {
                for (int i = 0; i < m_dataSize; i++)
                {
                    if (m_data[i] != other.m_data[i]) return false;
                }
            }
            return true;
        }

        /**
        * @brief not equal operator
        */
        bool operator!=(const Frame& other) const
        {
            return !(*this == other);
        }

#pragma endregion Operator

        /**
         * @brief Save the frame to a image file. The Save function require running the GDI+ library which is running when the WinCamrea object exist.
         * @param[in] path Path to save the image. Supported format are png, jpg, jpeg, bmp, tiff
         * @param[in] (Optional) encoderParams Encoder parameters. default is NULL. See https://learn.microsoft.com/en-us/windows/win32/api/gdiplusimaging/nl-gdiplusimaging-encoderparameters
         */
        void Save(
            const std::filesystem::path path,
            const Gdiplus::EncoderParameters* encoderParams = NULL
        );

    private:

        /**
        * If image is color image, Raw data will be stored in BGR format pixel by pixel, row by row and vertical flipped.
        * Let say if the image is
        * 1 2 3
        * 4 5 6
        * 7 8 9
        * The raw data will be [9,8,7,4,5,6,1,2,3] and each number is 3 byte which is BGR.
        */
        std::unique_ptr<unsigned char[]> m_data = nullptr;
        long m_dataCapacity = 0; // data capacity
        long m_dataSize = 0; // each frame data size

        int m_width = -1;
        int m_height = -1;
        GUID m_frameType;

        unsigned long m_frameIndex = 0;

    };

}

//*******************************

#endif