//
// Created on 2024/8/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Object.h"
#include <cstdint>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>

NAMESPACE_DEFAULT

// 封装 OHNativeWindow
class NNativeWindow : Object {
public:
    static NNativeWindow *createFromSurfaceId(uint64_t surfaceId) {
        OHNativeWindow *window = nullptr;
        int error = OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &window);
        if (error) {
            _ERROR("CreateNativeWindowFromSurfaceId() failed: %d", error);
            return nullptr;
        }
        return new NNativeWindow(window, true);
    }

public:
    NNativeWindow(OHNativeWindow *window, bool owner) : m_window(window), m_owner(owner) {}

    NNativeWindow(const NNativeWindow &other) : m_window(other.m_window), m_owner(other.m_owner), Object(other) {}

    ~NNativeWindow() {
        if (m_owner && m_window && no_reference()) {
            OH_NativeWindow_DestroyNativeWindow(m_window);
            m_window = nullptr;
        }
    }

public:
    inline bool valid() const { return m_window != nullptr; }

    std::string dump() {
        int width = 0, height = 0;
        getSize(width, height);
        std::string msg = "Window(";
        msg += "surface id=" + std::to_string(getSurfaceId()) + ", size=" + std::to_string(width) + "x" +
               std::to_string(height) + ", format=" + std::to_string(format()) + ", usage=" + std::to_string(usage()) +
        ", stride=" + std::to_string(stride()) + ", transform=" + std::to_string(transform()) +
        ", swapInterval=" + std::to_string(swapInterval()) + ")";
        return msg;
    }

    uint64_t getSurfaceId() const {
        _FATAL_IF(!m_window, "Invalid surface")

        uint64_t id = 0;
        int error = OH_NativeWindow_GetSurfaceId(m_window, &id);
        _ERROR_RETURN_IF(error, 0, "GetSurfaceId() failed: %d", error);
        return id;
    }

    bool setSize(int width, int height) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_BUFFER_GEOMETRY, width, height);
        _ERROR_RETURN_IF(error, false, "SetSize(%d, %d) failed: %d", width, height, error);
        return true;
    }

    bool getSize(int &width, int &height) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_BUFFER_GEOMETRY, &height, &width);
        _ERROR_RETURN_IF(error, false, "GetSize() failed: %d", error);
        return true;
    }

    bool setFormat(int f) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_FORMAT, f);
        _ERROR_RETURN_IF(error, false, "SetFormat() failed: %d", error);
        return true;
    }

    int format() {
        int f = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_FORMAT, &f);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_FORMAT) failed: %d", error);
        return f;
    }

    bool setUsage(uint64_t usage) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_USAGE, usage);
        _ERROR_RETURN_IF(error, false, "SetUsage() failed: %d", error);
        return true;
    }

    uint64_t usage() {
        uint64_t usage = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_USAGE, &usage);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_USAGE) failed: %d", error);
        return usage;
    }

    bool setStride(int stride) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_STRIDE, stride);
        _ERROR_RETURN_IF(error, false, "SetStride() failed: %d", error);
        return true;
    }

    int stride() {
        int stride = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_STRIDE, &stride);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_STRIDE) failed: %d", error);
        return stride;
    }

    bool setSwapInterval(int interval) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_SWAP_INTERVAL, interval);
        _ERROR_RETURN_IF(error, false, "SetSwapInterval() failed: %d", error);
        return true;
    }

    int swapInterval() {
        int interval = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_SWAP_INTERVAL, &interval);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_SWAP_INTERVAL) failed: %d", error);
        return interval;
    }

    bool setTimeout(int timeout) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_TIMEOUT, timeout);
        _ERROR_RETURN_IF(error, false, "setTimeout() failed: %d", error);
        return true;
    }

    int timeout() {
        int timeout = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_TIMEOUT, &timeout);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_TIMEOUT) failed: %d", error);
        return timeout;
    }

    bool setColorGamut(int colorGamut) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_COLOR_GAMUT, colorGamut);
        _ERROR_RETURN_IF(error, false, "SetColorGamut() failed: %d", error);
        return true;
    }

    int colorGamut() {
        int colorGamut = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_COLOR_GAMUT, &colorGamut);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_COLOR_GAMUT) failed: %d", error);
        return colorGamut;
    }

    bool setTransform(int transform) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_TRANSFORM, transform);
        _ERROR_RETURN_IF(error, false, "SetTransform() failed: %d", error);
        return true;
    }

    int transform() {
        int transform = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_TRANSFORM, &transform);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_TRANSFORM) failed: %d", error);
        return transform;
    }

    bool setUITimestamp(uint64_t timestamp) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_UI_TIMESTAMP, timestamp);
        _ERROR_RETURN_IF(error, false, "SetUITimestamp() failed: %d", error);
        return true;
    }

    int bufferQueueSize() {
        int size = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_BUFFERQUEUE_SIZE, &size);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_BUFFER_QUEUE_SIZE) failed: %d", error);
        return size;
    }

    bool setSourceType() {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_SOURCE_TYPE, 0);
        _ERROR_RETURN_IF(error, false, "SetSourceType() failed: %d", error);
        return true;
    }

    int sourceType() {
        int type = 0;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_SOURCE_TYPE, &type);
        _ERROR_RETURN_IF(error, -1, "NativeWindowHandleOpt(GET_SOURCE_TYPE) failed: %d", error);
        return type;
    }

    const char *appFrameworkType() {
        char *type = nullptr;
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, GET_APP_FRAMEWORK_TYPE, &type);
        _ERROR_RETURN_IF(error, nullptr, "NativeWindowHandleOpt(GET_APP_FRAMEWORK_TYPE) failed: %d", error);
        return type;
    }

    bool setHDRWhitePointBrightness(float brightness) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_HDR_WHITE_POINT_BRIGHTNESS, brightness);
        _ERROR_RETURN_IF(error, false, "SetHDRWhitePointBrights() failed: %d", error);
        return true;
    }

    bool setSDRWhitePointBrightness(float brightness) {
        int error = OH_NativeWindow_NativeWindowHandleOpt(m_window, SET_SDR_WHITE_POINT_BRIGHTNESS, brightness);
        _ERROR_RETURN_IF(error, false, "SetSDRWhitePointBrights() failed: %d", error);
        return true;
    }

    OHNativeWindow *value() { return m_window; }

private:
    const bool m_owner;
    OHNativeWindow *m_window;
};

NAMESPACE_END
