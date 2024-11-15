//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "harmony/media/AVFormat.h"

NAMESPACE_DEFAULT

class AVBuffer : Object {
public:
    explicit AVBuffer(int capacity) : m_owner(true) {
        m_buffer = OH_AVBuffer_Create(capacity);
        _FATAL_IF(m_buffer == nullptr, "OH_AVBuffer_Create failed, capacity: %d", capacity);
    }

    AVBuffer(OH_AVBuffer *buffer, bool own) : m_buffer(buffer), m_owner(own) {
        OH_AVCodecBufferAttr attr;
        OH_AVErrCode error = OH_AVBuffer_GetBufferAttr(m_buffer, &attr);
        if (!error) {
            m_pts = attr.pts;
            m_size = attr.size;
            m_offset = attr.offset;
            m_flags = attr.flags;
        }
    }

    AVBuffer(const AVBuffer &buffer) : m_buffer(buffer.m_buffer), m_owner(buffer.m_owner), Object(buffer) {
        OH_AVCodecBufferAttr attr;
        OH_AVErrCode error = OH_AVBuffer_GetBufferAttr(buffer.m_buffer, &attr);
        if (!error) {
            m_pts = attr.pts;
            m_size = attr.size;
            m_offset = attr.offset;
            m_flags = attr.flags;
        }
    }

    ~AVBuffer() {
        if (m_owner && m_buffer && no_reference()) {
            OH_AVBuffer_Destroy(m_buffer);
            m_buffer = nullptr;
        }
    }

public:
    inline bool valid() const { return m_buffer != nullptr; }

    inline OH_AVBuffer *value() const { return m_buffer; }
    
    void updateAttr() {
        OH_AVCodecBufferAttr attr;
        OH_AVErrCode error = OH_AVBuffer_GetBufferAttr(m_buffer, &attr);
        if (!error) {
            m_pts = attr.pts;
            m_size = attr.size;
            m_offset = attr.offset;
            m_flags = attr.flags;
        }
    }
    
    inline int capacity() const { return OH_AVBuffer_GetCapacity(m_buffer); }

    inline uint8_t *addr() const { return OH_AVBuffer_GetAddr(m_buffer); }

    // microseconds
    inline int64_t ptUs() const { return m_pts; }

    inline int32_t size() const { return m_size; }

    inline int32_t offset() const { return m_offset; }

    inline uint32_t flags() const { return m_flags; }
    
    inline bool isEOS() const { return m_flags & AVCODEC_BUFFER_FLAGS_EOS; }
    
    inline bool isSyncFrame() const { return m_flags & AVCODEC_BUFFER_FLAGS_SYNC_FRAME; }

    OH_AVErrCode setAttr(int64_t pts, int32_t size, int32_t offset, uint32_t flags) {
        OH_AVCodecBufferAttr attr = {pts, size, offset, flags};
        return setAttr(attr);
    }

    OH_AVErrCode setAttr(const OH_AVCodecBufferAttr &attr) {
        OH_AVErrCode error = OH_AVBuffer_SetBufferAttr(m_buffer, &attr);
        if (!error) {
            m_pts = attr.pts;
            m_size = attr.size;
            m_offset = attr.offset;
            m_flags = attr.flags;
        }
        return error;
    }

    AVFormat getParameter() {
        OH_AVFormat *format = OH_AVBuffer_GetParameter(m_buffer);
        return {format, true};
    }

    OH_AVErrCode setParameter(const AVFormat &format) { return setParameter(format.value()); }

    OH_AVErrCode setParameter(const OH_AVFormat *format) { return OH_AVBuffer_SetParameter(m_buffer, format); }
    
    void copyFrom(const AVBuffer &src) {
        setAttr(src.ptUs(), src.size(), src.offset(), src.flags());
        memcpy(addr(), src.addr(), src.size());
    }
    
    std::string toString() const {
        char buffer[128] = {0};
        std::snprintf(buffer, 127, "[buffer: %p, pts: %ld, size: %d, offset: %d, flags: %u]", m_buffer, m_pts, m_size, m_offset, m_flags);
        return {buffer};
    }

private:
    int64_t m_pts = 0;
    int32_t m_size = 0;
    int32_t m_offset = 0;
    uint32_t m_flags = 0;

    const bool m_owner;
    OH_AVBuffer *m_buffer;
};
NAMESPACE_END
