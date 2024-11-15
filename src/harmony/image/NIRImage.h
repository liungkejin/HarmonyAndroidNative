//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "common/Object.h"
#include "common/utils/BaseUtils.h"
#include "NNativeBuffer.h"
#include <multimedia/image_framework/image/image_native.h>

typedef OH_ImageNative OHImageNative;
typedef Image_Size OHImageSize;
typedef Image_ErrorCode OHImageErrorCode;

NAMESPACE_DEFAULT

class ImageComponent {
public:
    ImageComponent(OHImageNative *native, uint32_t type) : m_native(native), m_type(type) {}

    ~ImageComponent() {
        if (m_buffer) {
            delete m_buffer;
            m_buffer = nullptr;
        }
    }

    inline uint32_t type() const { return m_type; }

    size_t size() {
        if (m_size <= 0) {
            OHImageErrorCode error = OH_ImageNative_GetBufferSize(m_native, m_type, &m_size);
            _ERROR_IF(error, "OH_ImageNative_GetBufferSize,error: %d", error);
        }
        return m_size;
    }

    int32_t rowStride() {
        if (m_row_stride <= 0) {
            OHImageErrorCode error = OH_ImageNative_GetRowStride(m_native, m_type, &m_row_stride);
            _ERROR_IF(error, "OH_ImageNative_GetRowStride,error: %d", error);
        }
        return m_row_stride;
    }

    int32_t pixelStride() {
        if (m_pixel_stride <= 0) {
            OHImageErrorCode error = OH_ImageNative_GetPixelStride(m_native, m_type, &m_pixel_stride);
            _ERROR_IF(error, "OH_ImageNative_GetPixelStride,error: %d", error);
        }
        return m_pixel_stride;
    }

    NNativeBuffer &byteBuffer() {
        if (m_buffer == nullptr) {
            OH_NativeBuffer *buffer = nullptr;
            OHImageErrorCode error = OH_ImageNative_GetByteBuffer(m_native, m_type, &buffer);
            _ERROR_IF(error, "OH_ImageNative_GetByteBuffer,error: %d", error);

            m_buffer = new NNativeBuffer(buffer, false);
        }
        return *m_buffer;
    }

private:
    OHImageNative *m_native;
    uint32_t m_type;

    size_t m_size = 0;
    int32_t m_row_stride = -1;
    int32_t m_pixel_stride = -1;

    NNativeBuffer *m_buffer = nullptr;
};


// 封装 ImageNative
class NIRImage : Object {
public:
    NIRImage() : m_native(nullptr), m_own(false) {}

    explicit NIRImage(OHImageNative *img, bool own = true) : m_native(img), m_own(own) {
        OHImageErrorCode error = OH_ImageNative_GetImageSize(m_native, &m_image_size);
        _ERROR_IF(error, "OH_ImageNative_GetImageSize, error: %d", error);

        error = OH_ImageNative_GetComponentTypes(m_native, &m_comp_types, &m_comp_size);
        _ERROR_IF(error, "OH_ImageNative_GetComponentTypes,error: %d", error);
    }

    NIRImage(const NIRImage &o) : m_native(o.m_native), m_own(o.m_own), Object(o) {
        OHImageErrorCode error = OH_ImageNative_GetImageSize(m_native, &m_image_size);
        _ERROR_IF(error, "OH_ImageNative_GetImageSize, error: %d", error);

        error = OH_ImageNative_GetComponentTypes(m_native, &m_comp_types, &m_comp_size);
        _ERROR_IF(error, "OH_ImageNative_GetComponentTypes,error: %d", error);
    }

    ~NIRImage() {
        if (no_reference() && m_native && m_own) {
            OH_ImageNative_Release(m_native);
            m_native = nullptr;
        }
        DELETE_ARR_TO_NULL(m_comp_types);
        //_INFO("NIRImage reference count: %d", reference_count());
    }

public:
    inline bool valid() const { return m_native != nullptr; }

    inline uint32_t width() const { return m_image_size.width; }

    inline uint32_t height() const { return m_image_size.height; }

    size_t componentSize() const { return m_comp_size; }

    ImageComponent component(int i) {
        _FATAL_IF(i < 0 || i >= m_comp_size, "component index(%d) out of range(%d)", i, m_comp_size)
        return {m_native, m_comp_types[i]};
    }

private:
    bool m_own;
    OHImageNative *m_native;
    OHImageSize m_image_size = {};

    uint32_t *m_comp_types = new uint32_t[8];
    size_t m_comp_size = 0;
};

NAMESPACE_END
