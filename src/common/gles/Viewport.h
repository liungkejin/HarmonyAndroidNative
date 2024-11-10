//
// Created on 2024/6/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "GLUtil.h"

NAMESPACE_DEFAULT

class Viewport {
public:
    static void viewport(int x, int y, int width, int height, bool scissor = false) {
        if (scissor) {
            glScissor(x, y, width, height);
        } else {
            glViewport(x, y, width, height);
        }
    }

public:
    Viewport(int width, int height, bool scissor = false)
        : m_x(0), m_y(0), m_width(width), m_height(height), m_scissor(scissor) {}
    Viewport(int x, int y, int width, int height, bool scissor = false)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_scissor(scissor) {}

    Viewport& set(int width, int height, bool scissor = false) {
        set(0, 0, width, height, scissor);
        return *this;
    }

    Viewport& set(int x, int y, int width, int height, bool scissor = false) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
        m_scissor = scissor;
        return *this;
    }

    Viewport& enableClearColor(float red, float green, float blue, float alpha) {
        m_clear_color[0] = red;
        m_clear_color[1] = green;
        m_clear_color[2] = blue;
        m_clear_color[3] = alpha;
        return *this;
    }

    Viewport& disableClearColor() {
        m_clear_color[0] = -1000;
        return *this;
    }
    
    Viewport& set(const Viewport& v) {
        m_x = v.m_x;
        m_y = v.m_y;
        m_width = v.m_width;
        m_height = v.m_height;
        m_scissor = v.m_scissor;
        m_clear_color[0] = v.m_clear_color[0];
        m_clear_color[1] = v.m_clear_color[1];
        m_clear_color[2] = v.m_clear_color[2];
        m_clear_color[3] = v.m_clear_color[3];
        return *this;
    }

    void apply() {
        //_INFO("viewport(%.2f, %.2f, %.2f, %.2f)", m_x, m_y, m_width, m_height);
        if (m_scissor && m_width > 0 && m_height > 0) {
            glScissor(m_x, m_y, m_width, m_height);
        } else if (m_width > 0 && m_height > 0) {
            glViewport(m_x, m_y, m_width, m_height);
        }
        
        if (m_clear_color[0] >= 0 && m_clear_color[0] <= 1) {
            glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }

private:
    int m_x, m_y, m_width, m_height;
    bool m_scissor = false;
    
    float m_clear_color[4] = {-1000, 0, 0, 0};
};

NAMESPACE_END
