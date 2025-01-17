//
// Created by wutacam on 25-1-17.
//

#ifndef GLREADER_H
#define GLREADER_H

#include <common/Common.h>
#include <functional>
#include "GLUtil.h"

NAMESPACE_DEFAULT

// 双buffer实现快速 glReadPixels
class GLReader {
public:
    void readPixels(const int width, const int height, const std::function<void(GLubyte *pixels, int width, int height)>& callback) {
        initialize(width, height);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbos[m_cur_pbo_index]);
        // 开始读取像素数据到 PBO
        glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        int nextPBO = (m_cur_pbo_index + 1) % 2;
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbos[nextPBO]);
        // 映射 PBO 到系统内存
        GLubyte* pixels = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_width * m_height * 4 * sizeof(GLubyte), GL_MAP_READ_BIT);
        if (pixels) {
            // 在这里处理像素数据
            callback(pixels, width, height);
        }
        // 解除映射
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        m_cur_pbo_index = nextPBO;
    }

    void release() {
        if (m_width > 0) {
            glDeleteBuffers(2, m_pbos);
        }
        m_width = 0;
        m_height = 0;
        m_cur_pbo_index = 0;
    }

private:
    void initialize(const int width, const int height) {
        if (width != m_width || m_height != height) {
            release();

            m_width = width;
            m_height = height;
            glGenBuffers(2, m_pbos);
            for (int i = 0; i < 2; ++i) {
                glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbos[i]);
                // 分配足够的空间存储像素数据
                glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4 * sizeof(GLubyte), nullptr, GL_DYNAMIC_COPY);
            }
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }
    }

private:
    int m_width = 0;
    int m_height = 0;
    // 两个 PBO
    GLuint m_pbos[2] = {0};
    int m_cur_pbo_index = 0;
};

NAMESPACE_END

#endif //GLREADER_H
