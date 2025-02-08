//
// Created on 2024/7/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include <cstdint>
#include <chrono>
#include <thread>

NAMESPACE_DEFAULT

class TimeUtils {
public:
    static int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    }

    static int64_t nowUs() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    }
    
    static void sleepMs(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
};

class FPSCounter {
public:
    explicit FPSCounter(int intervalMs = 1500) : m_interval(intervalMs) {}

    bool count() {
        bool updated = false;
        if (m_count == 0) {
            m_start = TimeUtils::nowMs();
        } else {
            int64_t end = TimeUtils::nowMs();
            int64_t duration = end - m_start;
            if (duration > m_interval) {
                m_fps = (float) ((double)m_count*1000 / (double)duration);
                m_start = end;
                m_count = 0;
                updated = true;
            }
        }
        m_count++;
        return updated;
    }

    inline float fps() const { return m_fps; }

private:
    int64_t m_start = 0;
    int m_count = 0;
    float m_fps = 0.0f;
    int m_interval = 1000;
};

NAMESPACE_END
