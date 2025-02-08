//
// Created on 2024/8/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "harmony/image/NNativeWindow.h"
#include "harmony/media/video/VideoEncoder.h"
#include <cstdint>
#include <queue>

NAMESPACE_DEFAULT

class VSurfaceEncoder : public VideoEncoder {
public:
    static int32_t toWindowPixelFormat(int32_t avPixelFormat, bool isHDRVivid = false) {
        if (isHDRVivid) {
            return NATIVEBUFFER_PIXEL_FMT_YCBCR_P010;
        }
        switch (avPixelFormat) {
        case AV_PIXEL_FORMAT_SURFACE_FORMAT :
        case AV_PIXEL_FORMAT_RGBA :
            return NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
        case AV_PIXEL_FORMAT_YUVI420 :
            return NATIVEBUFFER_PIXEL_FMT_YCBCR_420_P;
        case AV_PIXEL_FORMAT_NV21 :
            return NATIVEBUFFER_PIXEL_FMT_YCRCB_420_SP;
        default : // NV12 and others
            return NATIVEBUFFER_PIXEL_FMT_YCRCB_420_SP;
        }
    }

public:
    VSurfaceEncoder(const char *name) : VideoEncoder(name) {}

    ~VSurfaceEncoder() { release(); }

    OH_AVErrCode start() {
        OH_AVErrCode error = VideoEncoder::start();
        
        std::lock_guard<std::mutex> lock(m_pts_mutex);
        while (!m_push_timestamps.empty()) {
            m_push_timestamps.pop();
        }
        m_use_own_timestamp = false;
        return error;
    }
    
    /**
     * 必须在 configure 和 prepare 之间调用
     */
    NNativeWindow *getSurface() {
        if (m_surface) {
            return m_surface;
        }
        _ERROR_RETURN_IF(m_state != VE_STATE_CONFIGURED, nullptr, "VEncoder(%s): Illegal state: %d", m_name, m_state);
        OHNativeWindow *window = nullptr;
        OH_AVErrCode code = OH_VideoEncoder_GetSurface(m_codec, &window);
        _ERROR_RETURN_IF(code, nullptr, "Encoder(%s): OH_VideoEncoder_GetSurface failed: %s", m_name,
                         AVUtils::errString(code));

        m_surface = new NNativeWindow(window, true);
        m_surface->setSize(m_width, m_height);

        _INFO("encode surface: %s", m_surface->dump());
        return m_surface;
    }

    void onNewOutputBuffer(uint32_t index, AVBuffer &buffer) override {
        int64_t ptUs = buffer.ptUs();
        uint32_t flags = buffer.flags();

        if (flags == AVCODEC_BUFFER_FLAGS_NONE || (flags & AVCODEC_BUFFER_FLAGS_SYNC_FRAME)) {
            std::lock_guard<std::mutex> lock(m_pts_mutex);
            if (m_use_own_timestamp) {
                if (m_push_timestamps.empty()) {
                    _ERROR("timestamp is missed! something wrong");
                }
                ptUs = m_push_timestamps.front();
                m_push_timestamps.pop();
            }
//            _INFO("buffer: %s", buffer.toString());
            buffer.setAttr(ptUs, buffer.size(), buffer.offset(), flags);
        }
        
        VideoEncoder::onNewOutputBuffer(index, buffer);
    }
    
    /**
     * 因为没有找到如果在 opengl 中传递时间戳给编码器，导致编码器出来的帧编码数据的时间戳都是0
     * 所以需要我们主动记录时间戳，但是这样是不稳的，因如果编码器带有B帧，那时间戳将不是顺序的
     * @param ptUs 微秒
     */
    void pushTimestampUs(int64_t ptUs) {
        std::lock_guard<std::mutex> lock(m_pts_mutex);
        if (m_input_start_ptUs < 0) {
            m_input_start_ptUs = ptUs;
        }
        m_input_duration_us = ptUs - m_input_start_ptUs;
        if (m_surface) {
            m_surface->setUITimestamp(m_input_duration_us);
        } else {
            m_use_own_timestamp = true;
            m_push_timestamps.push(m_input_duration_us);
        }
    }

    OH_AVErrCode reset() override {
        OH_AVErrCode code = VideoEncoder::reset();
        DELETE_TO_NULL(m_surface);
        return code;
    }

    void release() override {
        VideoEncoder::release();
        DELETE_TO_NULL(m_surface);
    }

private:
    NNativeWindow *m_surface = nullptr;
    
    std::mutex m_pts_mutex;
    std::queue<int64_t> m_push_timestamps;
    volatile bool m_use_own_timestamp = false;
};

NAMESPACE_END
