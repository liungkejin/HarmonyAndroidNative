//
// Created on 2024/8/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "eventpp/callbacklist.h"
#include "Namespace.h"

NAMESPACE_DEFAULT

template <typename T> class SimpleCallbackList {
public:
    int add(T cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        int id = m_id;
        m_handle_map[id] = m_callbacks.append(cb);
        m_id++;
        return id;
    }

    bool remove(int id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_handle_map.find(id) == m_handle_map.end()) {
            return false;
        }
        auto &handle = m_handle_map[id];
        m_callbacks.remove(handle);
        m_handle_map.erase(id);
        return true;
    }

    inline eventpp::CallbackList<T> callback() { return m_callbacks; }

private:
    std::mutex m_mutex;
    int m_id = 0;
    eventpp::CallbackList<T> m_callbacks;
    std::unordered_map<int, typename eventpp::CallbackList<T>::Handle> m_handle_map;
};

NAMESPACE_END
