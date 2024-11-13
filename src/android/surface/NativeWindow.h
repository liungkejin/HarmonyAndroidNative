//
// Created by LiangKeJin on 2024/11/13.
//

#pragma once

#include <android/native_window_jni.h>
#include "Namespace.h"
#include "common/Object.h"
#include "common/gles/GLEngine.h"

NAMESPACE_DEFAULT

/**
 * 封装 ANativeWindow
 */
class NativeWindow : Object {
public:
    static NativeWindow fromSurface(JNIEnv *env, jobject surface) {
        ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
        _FATAL_IF(window == nullptr, "ANativeWindow_fromSurface failed")
        return NativeWindow(window, false);
    }

public:
    explicit NativeWindow(ANativeWindow *window, bool acquireFirst = false) : m_window(window) {
        if (acquireFirst) {
            ANativeWindow_acquire(m_window);
        }
    }

    NativeWindow(const NativeWindow &o) : m_window(o.m_window), Object(o) {}

    ~NativeWindow() {
        if (no_reference() && m_window) {
            ANativeWindow_release(m_window);
            m_window = nullptr;
        }
    }

public:
    ANativeWindow *value() {
        return m_window;
    }

    int32_t width() {
        return ANativeWindow_getWidth(m_window);
    }

    int32_t height() {
        return ANativeWindow_getHeight(m_window);
    }

    int32_t format() {
        return ANativeWindow_getFormat(m_window);
    }

    bool setBuffersGeometry(int32_t width, int32_t height, int32_t format) {
        auto ret = ANativeWindow_setBuffersGeometry(m_window, width, height, format);
        _ERROR_RETURN_IF(ret != 0, false, "ANativeWindow_setBuffersGeometry(%d, %d, %d) failed: %d", width, height, format, ret)
        return true;
    }

    bool lock(ANativeWindow_Buffer *outBuffer, ARect *inOutDirtyBounds) {
        auto ret = ANativeWindow_lock(m_window, outBuffer, inOutDirtyBounds);
        _ERROR_RETURN_IF(ret != 0, false, "ANativeWindow_lock failed: %d", ret)
        return true;
    }

    bool unlock() {
        auto ret = ANativeWindow_unlockAndPost(m_window);
        _ERROR_RETURN_IF(ret != 0, false, "ANativeWindow_unlockAndPost failed: %d", ret)
        return true;
    }

public:
    bool bindToGLEngine(GLEngine &engine) {
        int w = width();
        int h = height();
        if (w > 0 && h > 0) {
            engine.updateSurface(m_window, w, h);
            return true;
        } else {
            _WARN("NativeWindow::bindToGLEngine failed, invalid size: %d x %d", w, h);
            return false;
        }
    }

public:
    std::string toString() {
        std::stringstream ss;
        ss << "(size: " << width() << " x " << height() << ", format: " << format() << ")";
        return ss.str();
    }

private:
    ANativeWindow *m_window = nullptr;
};

NAMESPACE_END
