//
// Created by LiangKeJin on 2024/12/10.
//

#pragma once

#include <media/NdkImage.h>
#include "Namespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include "common/utils/RawData.h"

NAMESPACE_DEFAULT

class IRImage : Object {
public:
    explicit IRImage(AImage *image) : m_image(image) {}

    IRImage(const IRImage &o) : m_image(o.m_image), Object(o) {}

    ~IRImage() {
        if (no_reference() && m_image) {
            AImage_delete(m_image);
            m_image = nullptr;
        }
    }

public:
    AImage *value() {
        return m_image;
    }

    int32_t width() {
        int32_t width = 0;
        auto ret = AImage_getWidth(m_image, &width);
        _ERROR_RETURN_IF(ret, 0, "AImage_getWidth failed: %d", ret);
        return width;
    }

    int32_t height() {
        int32_t height = 0;
        auto ret = AImage_getHeight(m_image, &height);
        _ERROR_RETURN_IF(ret, 0, "AImage_getHeight failed: %d", ret);
        return height;
    }

    int32_t format() {
        int32_t format = 0;
        auto ret = AImage_getFormat(m_image, &format);
        _ERROR_RETURN_IF(ret, 0, "AImage_getFormat failed: %d", ret);
        return format;
    }

    AImageCropRect cropRect() {
        AImageCropRect rect;
        auto ret = AImage_getCropRect(m_image, &rect);
        if (ret) {
            _ERROR("AImage_getCropRect failed: %d", ret);
            return {0, 0, 0, 0};
        }
        return {rect.left, rect.top, rect.right, rect.bottom};
    }

    int64_t timestamp() {
        int64_t timestamp = 0;
        auto ret = AImage_getTimestamp(m_image, &timestamp);
        _ERROR_RETURN_IF(ret, 0, "AImage_getTimestamp failed: %d", ret);
        return timestamp;
    }

    int32_t numberOfPlanes() {
        int32_t num = 0;
        auto ret = AImage_getNumberOfPlanes(m_image, &num);
        _ERROR_RETURN_IF(ret, 0, "AImage_getNumberOfPlanes failed: %d", ret);
        return num;
    }

    int32_t planePixelStride(int32_t planeIdx) {
        int32_t stride = 0;
        auto ret = AImage_getPlanePixelStride(m_image, planeIdx, &stride);
        _ERROR_RETURN_IF(ret, 0, "AImage_getPlanePixelStride failed: %d", ret);
        return stride;
    }

    int32_t planeRowStride(int32_t planeIdx) {
        int32_t stride = 0;
        auto ret = AImage_getPlaneRowStride(m_image, planeIdx, &stride);
        _ERROR_RETURN_IF(ret, 0, "AImage_getPlaneRowStride failed: %d", ret);
        return stride;
    }

    RawData planeData(int32_t planeIdx) {
        uint8_t *data = nullptr;
        int dataLength = 0;
        auto ret = AImage_getPlaneData(m_image, planeIdx, &data, &dataLength);
        _ERROR_RETURN_IF(ret, RawData(), "AImage_getPlaneData failed: %d", ret);
        return {data, (size_t) dataLength};
    }

private:
    AImage *m_image;
};

NAMESPACE_END
