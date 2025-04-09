//
// Created on 2024/7/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "EGLCtx.h"

#ifdef EGL_VERSION_1_0

#include "common/utils/EventThread.h"
#include <cstdint>

NAMESPACE_DEFAULT

typedef std::function<bool(int width, int height)> RenderRunnable;
typedef std::function<void(void *surface, int width, int height)> SurfaceChangeListener;

class GLEngine {
public:
    explicit GLEngine(const char *name, int glVersion = 3);

    GLEngine(const char *name, GLEngine &sharedCtx);
public:
    inline std::string name() const { return m_name; }

    void removeSurface() {
        updateSurface(nullptr, 0, 0);
    }

    void updateSurface(void *surface, int width, int height);
    
    void setSurfaceChangeListener(SurfaceChangeListener listener) {
        m_surface_change_listener = listener;
    }

    void sync(const Runnable &runnable, int timeoutMs = -1) { m_event_thread.sync(runnable, timeoutMs); }

    bool post(const Runnable &runnable) { return m_event_thread.post(runnable); }

    inline void swapBuffers() { m_ctx.swapBuffers(); }

    void syncRender(const RenderRunnable &runnable, int timeoutMs = -1);

    bool postRender(const RenderRunnable &runnable);

    void destroy();

protected:
    virtual void onUpdateSurface(void *surface, int width, int height);

protected:
    std::string m_name;

    void *m_surface = nullptr;
    int m_surf_width = 0, m_surf_height = 0;
    SurfaceChangeListener m_surface_change_listener = nullptr;

    EGLCtx m_ctx;
    EventThread m_event_thread;
};

NAMESPACE_END

#endif // EGL_VERSION_1_0