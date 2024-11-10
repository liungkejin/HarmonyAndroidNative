//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "EGLCtx.h"

NAMESPACE_DEFAULT

EGLCtx::EGLCtx(int version, EGLCtx *shared) : m_gl_version(version), m_shared_ctx(shared) {
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    _FATAL_IF(m_display == EGL_NO_DISPLAY, "EGLCtx: Unable to get egl display")

    EGLBoolean result = eglInitialize(m_display, &m_major, &m_minor);
    _FATAL_IF(result != EGL_TRUE, "EGLCtx: Unable to initialize egl display")

    int renderType = version == 2 ? EGL_OPENGL_ES2_BIT : EGL_OPENGL_ES3_BIT;
    int attribList[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, 
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, renderType,
        EGL_NONE
    };

    result = eglChooseConfig(m_display, attribList, &m_configs, 1, &m_configs_size);
    _FATAL_IF(result != EGL_TRUE, "EGLCtx: Unable to choose config, version: %d", version)

    /* Create EGLContext from */
    int attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, version, EGL_NONE};

    EGLContext sharedContext = shared != nullptr ? shared->m_context : nullptr;
    m_context = eglCreateContext(m_display, m_configs, sharedContext, attrib_list);
    _FATAL_IF(m_context == EGL_NO_CONTEXT, "EGLCtx: Unable to create egl context, version: %d", version)

    _INFO("create egl context success! context: %p, version: %d, shared ctx: %p", m_context, version, shared);
}

EGLCtx::~EGLCtx() {
    if (m_surface != EGL_NO_SURFACE) {
        if (!eglDestroySurface(m_display, m_surface)) {
            _WARN("~EGLCtx: destroy egl surface failed: %d", eglGetError());
        }
        m_surface = EGL_NO_SURFACE;
    }
    EGLContext ctx = m_context;
    if (!eglDestroyContext(m_display, m_context)) {
        _WARN("~EGLCtx: destroy egl context failed: %d", eglGetError());
    }
    if (!eglTerminate(m_display)) {
        _WARN("~EGLCtx: destroy display failed = %d", eglGetError());
    }    
    _INFO("~EGLCtx: destroyed: %p", ctx);
}

bool EGLCtx::makeCurrent(void *window) {
    if (m_surface != EGL_NO_SURFACE && m_surface_window == window) {
        return true;
    }
    if (m_surface != EGL_NO_SURFACE) {
        _INFO("destroy egl surface: %p", window);
        if (!eglDestroySurface(m_display, m_surface)) {
            _WARN("EGLCtx: destroy egl surface failed: %d", eglGetError());
        }
        m_surface = EGL_NO_SURFACE;
    }
    if (window == nullptr) {
        m_surface_window = nullptr;
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context);
        _INFO("EGLCtx: makeCurrent on NO SURFACE");
        return true;
    }
    
    EGLNativeWindowType eglWindow = reinterpret_cast<EGLNativeWindowType>(window);
    
    // 此处启用SRGB模式会导致颜色发白
    // EGLint winAttribs[] = {EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
    EGLint winAttribs[] = {EGL_NONE};
    m_surface = eglCreateWindowSurface(m_display, m_configs, eglWindow, winAttribs);
    _ERROR_RETURN_IF(m_surface == EGL_NO_SURFACE, false, "EGLCtx: Unable to create egl surface: %d", eglGetError())

    if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
        _ERROR("EGLCtx: eglMakeCurrent error: %d", eglGetError());
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
        return false;
    }
    m_surface_window = window;
    _INFO("EGLCtx: makeCurrent on surface: %p", window);
    return true;
}

bool EGLCtx::swapBuffers() {
    _ERROR_RETURN_IF(m_surface == EGL_NO_SURFACE, false, "EGLCtx: eglSwapBuffers error: NO_SURFACE")
    
    if (!eglSwapBuffers(m_display, m_surface)) {
        _WARN("EGLCtx: eglSwapBuffers error: %d", eglGetError());
        return false;
    }
    return true;
}

void EGLCtx::setPtNs(int64_t ptNs) {
    //PFNEGLPRESENTATIONTIMEANDROIDPROC(m_display, m_surface, ptNs);
}

NAMESPACE_END