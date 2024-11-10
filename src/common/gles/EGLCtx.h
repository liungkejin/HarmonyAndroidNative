//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "GLUtil.h"
#include <cstdint>

NAMESPACE_DEFAULT

class EGLCtx {
public:
    EGLCtx(int version = 3, EGLCtx *shared = nullptr);
    
    ~EGLCtx();

public:
    bool makeCurrent(void *window);
    
    inline int version() const { return m_gl_version;}
    
    inline bool isRenderable() const { return m_surface != EGL_NO_SURFACE; }
    
    bool swapBuffers();
    
    void setPtNs(int64_t ptNs);

private:
    const int m_gl_version;
    
    EGLDisplay m_display;
    int m_major, m_minor;

    EGLConfig m_configs;
    int m_configs_size = 0;

    EGLContext m_context;
    
    void *m_surface_window = nullptr;
    EGLSurface m_surface = EGL_NO_SURFACE;

    EGLCtx *m_shared_ctx = nullptr;
};

NAMESPACE_END