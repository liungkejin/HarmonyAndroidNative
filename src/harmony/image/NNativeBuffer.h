//
// Created on 2024/8/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "common/Object.h"
#include <cstdint>
#include <native_buffer/native_buffer.h>

NAMESPACE_DEFAULT

// 封装 OH_NativeBuffer
class NNativeBuffer : Object {
public:
    NNativeBuffer(int width, int height, int format, int usage, int stride) : m_owner(true), m_width(width), m_height(height),
                                                                             m_format(format), m_usage(usage), m_stride(stride) {
        OH_NativeBuffer_Config config = {width, height, format, usage, stride};
        m_buffer = OH_NativeBuffer_Alloc(&config);
        _ERROR_IF(m_buffer == nullptr,
                  "OH_NativeBuffer_Alloc failed! config(%d x %d, format: %d, usage: %d, stride: %d", width, height,
                  format, usage, stride);
    }

    NNativeBuffer(OH_NativeBuffer *buffer, bool owner) : m_owner(owner) {
        OH_NativeBuffer_Config config;
        OH_NativeBuffer_GetConfig(buffer, &config);
        m_width = config.width;
        m_height = config.height;
        m_format = config.format;
        m_usage = config.usage;
        m_stride = config.stride;
        m_buffer = buffer;
    }

    NNativeBuffer(const NNativeBuffer &o) : m_owner(o.m_owner), m_buffer(o.m_buffer), Object(o) {
        m_width = o.m_width;
        m_height = o.m_height;
        m_format = o.m_format;
        m_usage = o.m_usage;
        m_stride = o.m_stride;
    }

    ~NNativeBuffer() {
        if (m_mapped_addr && m_buffer && no_reference()) {
            unmap();
        }
        if (m_owner && m_buffer && no_reference()) {
            OH_NativeBuffer_Unreference(m_buffer);
            m_buffer = nullptr;
        }
    }

public:
    inline bool valid() const { return m_buffer != nullptr; }

    inline int width() const { return m_width; }

    inline int height() const { return m_height; }

    inline int format() const { return m_format; }

    inline int usage() const { return m_usage; }

    inline int stride() const { return m_stride; }

    void *map() {
        if (m_mapped_addr) {
            return m_mapped_addr;
        }
        int error = OH_NativeBuffer_Map(m_buffer, &m_mapped_addr);
        _ERROR_RETURN_IF(error, m_mapped_addr, "OH_NativeBuffer_Map failed! error: %d", error);
        return m_mapped_addr;
    }

    void unmap() {
        if (m_mapped_addr) {
            int error = OH_NativeBuffer_Unmap(m_buffer);
            _ERROR_IF(error, "OH_NativeBuffer_Unmap failed! error: %d", error);
            m_mapped_addr = nullptr;
        }
    }
    
    uint32_t getSeqNum() {
        return OH_NativeBuffer_GetSeqNum(m_buffer);
    }
    
    int setColorSpace(OH_NativeBuffer_ColorSpace space) {
        return OH_NativeBuffer_SetColorSpace(m_buffer, space);
    }
    
    int getColorSpace(OH_NativeBuffer_ColorSpace &space) {
        return OH_NativeBuffer_GetColorSpace(m_buffer, &space);
    }

    OH_NativeBuffer *value() { return m_buffer; }

private:
    const bool m_owner;

    int m_width;
    int m_height;
    int m_format;
    int m_usage;
    int m_stride;
    OH_NativeBuffer *m_buffer;

    void *m_mapped_addr = nullptr;
};

NAMESPACE_END
