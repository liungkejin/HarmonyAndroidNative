//
// Created by LiangKeJin on 2024/12/10.
//

#pragma once

#include <media/NdkImageReader.h>
#include "ZNamespace.h"
#include "common/Object.h"
#include "IRImage.h"

NAMESPACE_DEFAULT

class ImageReader;
typedef std::function<void(ImageReader& ir)> ImageCallback;

class ImageReader : Object {
public:
    explicit ImageReader(AImageReader *reader, bool owned = true) : m_reader(reader), m_owned(owned) {}

    ImageReader(int32_t width, int32_t height, int32_t format, int32_t maxImages): m_owned(true) {
        auto ret = AImageReader_new(width, height, format, maxImages, &m_reader);
        _FATAL_IF(ret, "AImageReader_new failed: %d", ret);
    }

    ImageReader(const ImageReader &o) : m_reader(o.m_reader), Object(o) {}

    ~ImageReader() {
        if (no_reference() && m_reader) {
            AImageReader_delete(m_reader);
            m_reader = nullptr;
        }
    }

public:
    inline bool valid() const {
        return m_reader != nullptr;
    }

    AImageReader *value() {
        return m_reader;
    }

    ANativeWindow *window() {
        ANativeWindow *window = nullptr;
        auto ret = AImageReader_getWindow(m_reader, &window);
        _ERROR_IF(ret, "AImageReader_getWindow failed: %d", ret);
        return window;
    }

    int32_t width() {
        int32_t width = 0;
        auto ret = AImageReader_getWidth(m_reader, &width);
        _ERROR_RETURN_IF(ret, 0, "AImageReader_getWidth failed: %d", ret);
        return width;
    }

    int32_t height() {
        int32_t height = 0;
        auto ret = AImageReader_getHeight(m_reader, &height);
        _ERROR_RETURN_IF(ret, 0, "AImageReader_getHeight failed: %d", ret);
        return height;
    }

    int32_t format() {
        int32_t format = 0;
        auto ret = AImageReader_getFormat(m_reader, &format);
        _ERROR_RETURN_IF(ret, 0, "AImageReader_getFormat failed: %d", ret);
        return format;
    }

    int32_t maxImages() {
        int32_t maxImages = 0;
        auto ret = AImageReader_getMaxImages(m_reader, &maxImages);
        _ERROR_RETURN_IF(ret, 0, "AImageReader_getMaxImages failed: %d", ret);
        return maxImages;
    }

    IRImage acquireNextImage() {
        AImage *image = nullptr;
        auto ret = AImageReader_acquireNextImage(m_reader, &image);
        _FATAL_IF(ret, "AImageReader_acquireNextImage failed: %d", ret);
        return IRImage(image);
    }

    IRImage acquireLatestImage() {
        AImage *image = nullptr;
        auto ret = AImageReader_acquireLatestImage(m_reader, &image);
        _FATAL_IF(ret, "AImageReader_acquireLatestImage failed: %d", ret);
        return IRImage(image);
    }

    void setImageAvailableCallback(ImageCallback callback);

private:
    AImageReader *m_reader = nullptr;
    bool m_owned = true;

    ImageCallback m_callback = nullptr;
};

NAMESPACE_END
