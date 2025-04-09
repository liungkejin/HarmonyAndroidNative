//
// Created on 2024/7/5.
//

#include "GLEngine.h"

#ifdef EGL_VERSION_1_0

NAMESPACE_DEFAULT

GLEngine::GLEngine(const char *name, int glVersion)
        : m_name(name), m_ctx(name, glVersion), m_event_thread(name) {
    post([this]() { onUpdateSurface(nullptr, 0, 0); });
}

GLEngine::GLEngine(const char *name, GLEngine &sharedCtx)
        : m_name(name), m_ctx(name, sharedCtx.m_ctx), m_event_thread(name) {
    post([this]() { onUpdateSurface(nullptr, 0, 0); });
}

void GLEngine::updateSurface(void *surface, int width, int height) {
    sync([this, surface, width, height]() { onUpdateSurface(surface, width, height); });
}

void GLEngine::onUpdateSurface(void *surface, int width, int height) {
    m_surface = surface;
    m_surf_width = width;
    m_surf_height = height;

    bool result = m_ctx.makeCurrent(surface);
    if (m_surface != nullptr) {
        m_surface = surface;
        m_surf_width = width;
        m_surf_height = height;
    } else {
        m_surface = nullptr;
        m_surf_width = 0;
        m_surf_height = 0;
    }
    if (m_surface_change_listener != nullptr) {
        m_surface_change_listener(m_surface, m_surf_width, m_surf_height);
    }

    _INFO("GLEngine(%s) update surface(%p) result: %s, surface size(%d x %d)", m_name.c_str(), surface,
          (result ? "success" : "fail"), m_surf_width, m_surf_height);
}

void GLEngine::syncRender(const RenderRunnable &runnable, int timeoutMs) {
    m_event_thread.sync(
            [this, runnable]() {
                bool swap = runnable(m_surf_width, m_surf_height);
                if (swap) {
                    m_ctx.swapBuffers();
                }
            },
            timeoutMs);
}

bool GLEngine::postRender(const RenderRunnable &runnable) {
    return m_event_thread.post([this, runnable]() {
        bool swap = runnable(m_surf_width, m_surf_height);
        if (swap) {
            m_ctx.swapBuffers();
        }
    });
}

void GLEngine::destroy() {
    if (m_event_thread.isRunning()) {
        m_event_thread.sync([this]() {
            m_ctx.destroy();
        });
        m_event_thread.quit();
        _INFO("GLEngine(%s) destroyed", m_name.c_str());
    }
}

NAMESPACE_END

#endif // EGL_VERSION_1_0