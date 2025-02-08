//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#if defined(__ANDROID__) || defined(__HARMONYOS__)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#ifdef EGL_VERSION_1_0

#include "common/Object.h"
#include "common/utils/Base.h"
#include "common/Log.h"

#include <cstdint>

NAMESPACE_DEFAULT

class EGLCtx {
public:
    explicit EGLCtx(const char *name, int version = 3);

    explicit EGLCtx(const char *name, EGLCtx &shared);
    
    ~EGLCtx();

public:
    bool makeCurrent(void *window);
    
    inline int version() const { return m_gl_version;}
    
    inline bool isRenderable() const { return m_surface != EGL_NO_SURFACE; }
    
    bool swapBuffers();
    
    void setPtNs(int64_t ptNs);

    void destroy();

private:
    void initialize(int version, EGLContext sharedCtx);

private:
    const std::string m_name;
    const int m_gl_version;

    EGLDisplay m_display = EGL_NO_DISPLAY;
    int m_major = 0, m_minor = 0;

    EGLConfig m_configs = nullptr;
    int m_configs_size = 0;

    EGLContext m_context = EGL_NO_CONTEXT;
    
    void *m_surface_window = nullptr;
    EGLSurface m_surface = EGL_NO_SURFACE;
};

NAMESPACE_END

#endif