/**
* Copy right (c) 2020-2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#include "frame/frame.h"

#include "directshow_camera/video_format/ds_guid.h"
#include "directshow_camera/utils/ds_video_format_utils.h"

#include "utils/path_utils.h"

namespace DirectShowCamera
{
#pragma region Constructor and Destructor

    Frame::Frame()
    {
        Clear();
    }

    Frame::~Frame()
    {
    }

    Frame::Frame(const Frame& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_dataCapacity = other.m_dataCapacity;
        m_dataSize = other.m_dataSize;
        m_frameIndex = other.m_frameIndex;
        m_frameType = other.m_frameType;
        m_data = std::make_unique<unsigned char[]>(m_dataCapacity);
        memcpy(m_data.get(), other.m_data.get(), m_dataSize);
    }

    Frame::Frame(Frame&& other) noexcept
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_dataCapacity = other.m_dataCapacity;
        m_dataSize = other.m_dataSize;
        m_frameIndex = other.m_frameIndex;
        m_frameType = other.m_frameType;
        m_data = std::move(other.m_data);

        // Reset other after move
        other.Clear();
    }

    void Frame::Clear()
    {
        m_width = -1;
        m_height = -1;
        m_dataCapacity = 0;
        m_dataSize = 0;
        m_frameIndex = 0;
        if (m_data != nullptr) m_data.reset();
    }

#pragma endregion Constructor and Destructor

#pragma region Frame
    void Frame::setInfo(int width, int height, const GUID &frameType) {
        // Set
        m_width = width;
        m_height = height;
        m_frameType = frameType;
    }

    void Frame::putData(int frameIndex, const unsigned char *data, int dataSize) {
        if (m_dataCapacity < dataSize) {
            m_data = std::make_unique<unsigned char[]>(dataSize);
            m_dataCapacity = dataSize;
        }
        memcpy(m_data.get(), data, dataSize);
        m_frameIndex = frameIndex;
        m_dataSize = dataSize;
    }

    unsigned char* Frame::getFrameDataPtr(int& numOfBytes)
    {
        numOfBytes = m_dataSize;
        return m_data.get();
    }

    std::shared_ptr<unsigned char[]> Frame::getFrameData(int& numOfBytes, bool flipVertical, bool rgb2bgr)
    {
        // Check
        if (!FrameDecoder::isMonochromeFrameType(m_frameType) &&
            !FrameDecoder::isRGBFrameType(m_frameType)
        )
        {
            throw std::runtime_error("Frame type(" + DirectShowVideoFormatUtils::ToString(m_frameType) + ") is not 8 bit.");
        }

        numOfBytes = m_dataSize;
        // Convert
        if (FrameDecoder::isMonochromeFrameType(m_frameType))
        {
            // Monochrome 这里并没有做任何解码，只是拷贝了一下数据
            return FrameDecoder::DecodeMonochromeFrame(
                m_data.get(),
                m_frameType,
                m_width,
                m_height,
                flipVertical
            );
        }
        else
        {
            if (m_frameType != MEDIASUBTYPE_RGB24) {
                auto result = std::make_unique<unsigned char[]>(m_dataSize);
                memcpy(result.get(), m_data.get(), m_dataSize);
                return result;
            }

            // RGB
            return FrameDecoder::DecodeRGBFrame(
                m_data.get(),
                m_frameType,
                m_width,
                m_height,
                flipVertical,
                !rgb2bgr
            );
        }
    }

    std::shared_ptr<unsigned short[]> Frame::getFrame16bitData(int& numOfBytes, bool flipVertical)
    {
        // Check
        FrameDecoder::Check16BitMonochromeFrameType(m_frameType);

        numOfBytes = m_dataSize;
        // Convert
        return FrameDecoder::Decode16BitMonochromeFrame(
            m_data.get(),
            m_frameType,
            m_width,
            m_height,
            flipVertical
        );
    }

#pragma endregion Frame

#pragma region Getter

    bool Frame::isEmpty() const
    {
        return m_data == nullptr || m_dataSize == 0;
    }

    unsigned long Frame::getFrameIndex() const
    {
        return m_frameIndex;
    }

    int Frame::getWidth() const
    {
        return m_width;
    }

    int Frame::getHeight() const
    {
        return m_height;
    }

    int Frame::getFrameSize() const
    {
        return m_dataSize;
    }

    const GUID & Frame::getRawFrameType() const
    {
        return m_frameType;
    }

    Frame::FrameType Frame::getFrameType() const
    {
        if (m_frameType == MEDIASUBTYPE_None)
        {
            return FrameType::None;
        }
        else if (FrameDecoder::isMonochromeFrameType(m_frameType))
        {
            return FrameType::Monochrome8bit;
        }
        else if (FrameDecoder::is16BitMonochromeFrameType(m_frameType))
        {
            return FrameType::Monochrome16bit;
        }
        else if (FrameDecoder::isRGBFrameType(m_frameType))
        {
            return FrameType::ColorRGB24bit;
        }
        else
        {
            return FrameType::Unknown;
        }
    }

#pragma endregion Getter

#ifdef WITH_OPENCV2
#pragma region OpenCV

    cv::Mat Frame::getMat(bool flipVertical, bool rgb2bgr)
    {
        // Check
        FrameDecoder::CheckSupportVideoType(m_frameType);

        // Convert
        return FrameDecoder::DecodeFrameToCVMat(
            m_data.get(),
            m_frameType,
            m_width, 
            m_height,
            flipVertical,
            rgb2bgr
        );
    }

#pragma endregion OpenCV
#endif

    void Frame::Save(
        const std::filesystem::path path,
        const Gdiplus::EncoderParameters* encoderParams
    )
    {
        // Check video type
        FrameDecoder::CheckSupportVideoType(m_frameType);

        // Get file Extension
        const auto fileExtension = Utils::PathUtils::getExtension(path);

        // Get encoder
        CLSID pngClsid;
        bool success = false;
        if (fileExtension == "png")
        {
            success = Utils::GDIPLUSUtils::GetPngEncoderClsid(pngClsid);
        }
        else if (fileExtension == "jpg" || fileExtension == "jpeg")
        {
            success = Utils::GDIPLUSUtils::GetJpegEncoderClsid(pngClsid);
        }
        else if (fileExtension == "bmp")
        {
            success = Utils::GDIPLUSUtils::GetBmpEncoderClsid(pngClsid);
        }
        else if (fileExtension == "tiff")
        {
            success = Utils::GDIPLUSUtils::GetTiffEncoderClsid(pngClsid);
        }
        else
        {
            throw std::invalid_argument("File type(" + fileExtension + ") is not supported.");
        }
        if (!success) throw std::runtime_error("Can't get encoder for file type(" + fileExtension + ").");

        // Create bitmap
        Gdiplus::PixelFormat pixelFormat;
        if (FrameDecoder::isMonochromeFrameType(m_frameType))
        {
            pixelFormat = PixelFormat8bppIndexed;
        }
        else if (FrameDecoder::is16BitMonochromeFrameType(m_frameType))
        {
            pixelFormat = PixelFormat16bppGrayScale;
        }
        else
        {
            // TODO convert raw frame type to RGB24
            pixelFormat = PixelFormat24bppRGB;
        }
        Gdiplus::Bitmap bitmap(m_width, m_height, pixelFormat);

        // Draw
        if (false)
        {
            // Draw image which is vertical flip

            // Note: The image is already vertical flip in m_data

            Utils::GDIPLUSUtils::DrawBitmap(bitmap, m_data.get(), m_dataSize);
        }
        else
        {
            // Draw image which is not vertical flip
            // As image is already vertical flip in m_data, we need to flip it

            // Create a image buffer
            auto data = new unsigned char[m_dataSize];

            try {
                // Flip the image into the buffer
                FrameDecoder::DecodeFrame(
                    m_data.get(),
                    data,
                    m_frameType,
                    m_width,
                    m_height,
                    false,
                    false
                );

                // Draw
                Utils::GDIPLUSUtils::DrawBitmap(bitmap, data, m_dataSize);

                // Delete the buffer
                delete[] data;
            }
            catch (...)
            {
                delete[] data;
                throw;
            }
        }

        // Save
        bitmap.Save(path.wstring().c_str(), &pngClsid, encoderParams);
    }
}