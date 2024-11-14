//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <map>
#include <mutex>
#include "Namespace.h"

NAMESPACE_DEFAULT

#define CALLBACKS_VECTOR std::vector<std::pair<HOST&, CALLBACK*>> 

template <typename HOST, typename CALLBACK> class CallbackMgr {
public:
    bool hasAnyCallback(const void *key) {
        return callbacks_map.find(key) != callbacks_map.end();
    }
    
    void addCallback(const void *key, HOST &host, CALLBACK *callback) {
        if (callback == nullptr) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mtx);
        auto wrap = std::pair<HOST&, CALLBACK*>(host, callback);
        
        auto it = callbacks_map.find(key);
        if (it == callbacks_map.end()) {
            auto vect = CALLBACKS_VECTOR();
            vect.push_back(wrap);
            callbacks_map.insert(std::make_pair(key, vect));
        } else {
            auto &vect = it->second;
            for (auto &item : vect) {
                if (item.second == callback) {
                    return;
                }
            }
            vect.push_back(wrap);
        }
    }

    void removeCallback(const void *key, CALLBACK *callback) {
        if (callback == nullptr) {
            return;
        }
        std::lock_guard<std::mutex> lock(mtx);
        auto it = callbacks_map.find(key);
        if (it == callbacks_map.end()) {
            return;
        }
        auto &vect = it->second;
        for (auto i = vect.begin(); i != vect.end(); ++i) {
            if (i->second == callback) {
                vect.erase(i);
                break;
            }
        }
        if (vect.empty()) {
            callbacks_map.erase(key);
        }
    }
    
    CALLBACKS_VECTOR* findCallback(const void *key) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = callbacks_map.find(key);
        if (it == callbacks_map.end()) {
            return nullptr;
        }
        return &it->second;
    }
    
    void clearCallback(const void *key) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = callbacks_map.find(key);
        if (it == callbacks_map.end()) {
            return;
        }
        callbacks_map.erase(key);
    }
    
private:
    std::map<const void *, CALLBACKS_VECTOR> callbacks_map;
    std::mutex mtx;
};

NAMESPACE_END
