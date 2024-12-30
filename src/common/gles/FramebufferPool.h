//
// Created on 2024/6/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "GLUtil.h"
#include "Framebuffer.h"
#include <vector>
#include <map>

NAMESPACE_DEFAULT

class FramebufferRef : Object {
public:
    explicit FramebufferRef(Framebuffer *fb = nullptr) : m_fb(fb) {
        if (fb) {
            fb->ref();
        }
    }

    FramebufferRef(const FramebufferRef &o) : m_fb(o.m_fb), Object(o) {
        if (m_fb) {
            m_fb->ref();
        }
    }

    ~FramebufferRef() {
        if (m_fb) {
            m_fb->unref();
        }
    }

public:
    // override operator =
    FramebufferRef &operator=(const FramebufferRef &o) {
        if (this == &o) {
            return *this;
        }
        if (m_fb) {
            m_fb->unref();
        }
        m_fb = o.m_fb;
        if (m_fb) {
            m_fb->ref();
        }
        return *this;
    }

    inline Framebuffer *get() const { return m_fb; }

    inline Framebuffer *operator->() const { return m_fb; }

    void free() {
        if (m_fb) {
            m_fb->unref();
            m_fb = nullptr;
        }
    }
private:
    Framebuffer *m_fb;
};

class FbArrayList {
public:
    FbArrayList(int w, int h) : m_width(w), m_height(h) {}

    FramebufferRef obtain() {
        for (auto &it : m_fb_list) {
            if (it->available()) {
                return FramebufferRef(it);
            }
        }
        auto *fb = new Framebuffer();
        fb->create(m_width, m_height);

        m_fb_list.push_back(fb);
        return FramebufferRef(fb);
    }

    int allSize() const { return (int)m_fb_list.size(); }

    int avSize() {
        int size = 0;
        for (auto &it : m_fb_list) {
            if (it->available()) {
                size++;
            }
        }
        return size;
    }

    bool trimMem(bool once=true) {
        bool trimmed = false;
        for (auto it = m_fb_list.begin(); it != m_fb_list.end();) {
            if ((*it)->available()) {
                (*it)->release();
                delete (*it);
                it = m_fb_list.erase(it);
                
                trimmed = true;
                if (once) {
                    return true;
                }
            } else {
                it++;
            }
        }
        return trimmed;
    }

    int memSizeMb() { return (int) ((uint64_t)m_width * m_height * 4 * m_fb_list.size() / 1024 / 1024); }

    void release() {
        for (auto &it : m_fb_list) {
            it->release();
            delete it;
        }
        m_fb_list.clear();
    }

private:
    std::vector<Framebuffer *> m_fb_list;
    const int m_width, m_height;
};

class FramebufferPool {
public:
    explicit FramebufferPool(int maxCacheMb = 50) : m_max_mem_cache_mb(maxCacheMb) {}
    
    FramebufferRef obtain(int w, int h) {
        std::string key = std::to_string(w) + "x" + std::to_string(h);
        
        FramebufferRef fb(nullptr);
        auto it = m_fb_map.find(key);
        if (it != m_fb_map.end()) {
            fb = it->second->obtain();
        } else {
            auto *list = new FbArrayList(w, h);
            m_fb_map[key] = list;
            fb = list->obtain();
        }
        trimMem();
        return fb;
    }
    
    int allSize() {
        int size = 0;
        for (auto &it : m_fb_map) {
            size += it.second->allSize();
        }
        return size;
    }
    
    int avSize() {
        int size = 0;
        for (auto &it : m_fb_map) {
            size += it.second->avSize();
        }
        return size;
    }
    
    int memSizeMb() {
        int size = 0;
        for (auto &it : m_fb_map) {
            size += it.second->memSizeMb();
        }
        return size;
    }
    
    void trimMem(bool force = false) {
        int curMemSizeMb = memSizeMb();
        if (curMemSizeMb <= m_max_mem_cache_mb && !force) {
            return;
        }
        do {
            for (auto& list : m_fb_map) {
                if (list.second->trimMem() && !force) {
                    break;
                }
            }
        } while (memSizeMb() > m_max_mem_cache_mb && avSize() > 0);
        
        int memSize = memSizeMb();
        _INFO("FramebufferPool::trimMem, force=%d: %d mb -> %d mb", force, curMemSizeMb, memSize);
    }
    
    void release() {
        for (auto &it : m_fb_map) {
            it.second->release();
            delete it.second;
        }
        m_fb_map.clear();
    }

private:
    std::map<std::string, FbArrayList *> m_fb_map;
    const int m_max_mem_cache_mb;
};


NAMESPACE_END
