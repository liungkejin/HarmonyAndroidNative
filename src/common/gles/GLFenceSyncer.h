//
// Created on 2024/8/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "GLUtil.h"

NAMESPACE_DEFAULT

class GLFenceSyncer {
public:
    GLFenceSyncer() : m_gl_sync(nullptr) {}
    
    GLFenceSyncer(const GLFenceSyncer& other) : m_gl_sync(other.m_gl_sync) {}
    
public:
    void create(bool flush = true) {
        if (m_gl_sync) {
            _WARN("FenceSync didn't trigger, delete it!");
            glDeleteSync(m_gl_sync);
        }
        m_gl_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        if (flush) {
            glFlush();
        }
    }
    
    bool valid() const { return m_gl_sync != nullptr; }
    
    bool wait(GLbitfield flags = GL_SYNC_FLUSH_COMMANDS_BIT) {
        _WARN_RETURN_IF(!m_gl_sync, false, "fence sync == nullptr");
        
        GLenum result = glClientWaitSync(m_gl_sync, flags, GL_TIMEOUT_IGNORED);
        if (result == GL_WAIT_FAILED) {
            _ERROR("GLUtil::waitFenceSync failed");
            return false;
        } else if (result == GL_TIMEOUT_EXPIRED) {
            _ERROR("GLUtil::waitFenceSync timeout");
            return false;
        }
        return true;
    }
    
    void release() {
        if (m_gl_sync) {
            glDeleteSync(m_gl_sync);
            m_gl_sync = nullptr;
        }
    }
    
private:
    GLsync m_gl_sync;
};

NAMESPACE_END
