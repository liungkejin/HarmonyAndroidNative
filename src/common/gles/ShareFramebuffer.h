//
// Created on 2024/8/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Object.h"
#include "Framebuffer.h"
#include "GLFenceSyncer.h"

NAMESPACE_DEFAULT

class ShareFramebuffer {
public:
    /**
     * 生产者线程在更新之前先 lock 住
     */
    Framebuffer& updateLock() {
        m_update_locker.lock();
        return m_framebuffer;
    }
    
    /**
     * 生产者线程更新完之后，unlock
     */
    void updateUnlock(bool fenceSync=false) {
        if (fenceSync) {
            m_fence_syncer.create();
        } else if (m_fence_syncer.valid()) {
            m_fence_syncer.release();
        }
        m_update_locker.unlock();
    }
    
    /**
     * 消费者线程在获取之前先lock
     * @return 在 waiting 失败时返回 nullptr
     */
    Framebuffer* obtainLock(bool fenceSync=true) {
        m_update_locker.lock();
        if (fenceSync) {
            if (m_fence_syncer.valid()) {
                bool success = m_fence_syncer.wait();
                m_fence_syncer.release();
                if (!success) {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }
        return &m_framebuffer;
    }
    
    /**
     * 消费者线程使用完成之后 unlock
     */
    void obtainUnlock() {
        m_update_locker.unlock();
    }
    
    void release() {
        m_update_locker.lock();
        m_framebuffer.release();
        m_fence_syncer.release();
        m_update_locker.unlock();
    }
    
private:
    std::mutex m_update_locker;
    Framebuffer m_framebuffer;
    GLFenceSyncer m_fence_syncer;
};
NAMESPACE_END
