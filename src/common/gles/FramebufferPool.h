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

class FbArrayList {
public:
    FbArrayList(int w, int h) : m_width(w), m_height(h) {}

    Framebuffer *obtain() {
        for (auto &it : m_fb_list) {
            if (it->available()) {
                it->ref();
                return it;
            }
        }
        Framebuffer *fb = new Framebuffer();
        fb->create(m_width, m_height);
        fb->ref();

        m_fb_list.push_back(fb);
        return fb;
    }

    int allSize() const { return m_fb_list.size(); }

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

    int memSizeMb() { return m_width * m_height * 4 * m_fb_list.size() / 1024 / 1024; }

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
    
    Framebuffer *obtain(int w, int h) {
        std::string key = std::to_string(w) + "x" + std::to_string(h);
        
        Framebuffer * fb;
        auto it = m_fb_map.find(key);
        if (it != m_fb_map.end()) {
            fb = it->second->obtain();
        } else {
            FbArrayList *list = new FbArrayList(w, h);
            m_fb_map[key] = list;
            fb = list->obtain();
        }
        trimMemIfNeed();
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
    
    void trimMemIfNeed() {
        int curMemSizeMb = memSizeMb();
        if (curMemSizeMb <= m_max_mem_cache_mb) {
            return;
        }
        do {
            for (auto& list : m_fb_map) {
                if (list.second->trimMem()) {
                    break;
                }
            }
        } while (memSizeMb() > m_max_mem_cache_mb && avSize() > 0);
        
        int memSize = memSizeMb();
        _INFO("FramebufferPool::trimMem: %d mb -> %d mb", curMemSizeMb, memSize);
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
