//
// Created on 2024/7/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "BaseFilter.h"
#include "../GLUtil.h"
#include <cstdint>
#include <mutex>

NAMESPACE_DEFAULT

class NV21Filter : public BaseFilter {
public:
    NV21Filter() : BaseFilter("yuv") {
        defAttribute("position", DataType::FLOAT_POINTER)->bind(vertexCoord());
        defAttribute("inputTextureCoordinate", DataType::FLOAT_POINTER)->bind(textureCoord());
        defUniform("yTexture", DataType::SAMPLER_2D);
        defUniform("uTexture", DataType::SAMPLER_2D);
        defUniform("vTexture", DataType::SAMPLER_2D);
    }

    std::string vertexShader() override {
        std::string vs = R"(
attribute vec4 position;
attribute vec2 inputTextureCoordinate;
varying highp vec2 textureCoordinate;

void main() {
    gl_Position = position;
    textureCoordinate = inputTextureCoordinate;
}
        )";
        return CORRECT_VERTEX_SHADER(vs);
    }

    std::string fragmentShader() override {
        std::string fs = R"(
precision highp float;
varying highp vec2 textureCoordinate;
uniform sampler2D yTexture;
uniform sampler2D uTexture;
uniform sampler2D vTexture;

void main() {
    float y = texture2D(yTexture, textureCoordinate).r;
    float u = texture2D(uTexture, textureCoordinate).r - 0.5;
    float v = texture2D(vTexture, textureCoordinate).r - 0.5;
)";
        if (m_standard == BT601) {
            fs += R"(
    float r = y + 1.402 * v;
    float g = y - 0.344136 * u - 0.714136 * v;
    float b = y + 1.772 * u;
}
)";
        } else if (m_standard == BT2020) {
            fs += R"(
    float r = y + 1.4746 * v;
    float g = y - 0.164553 * u - 0.571353 * v;
    float b = y + 1.8814 * u;
)";
        } else {
            fs += R"(
    float r = y + 1.402 * v;
    float g = y - 0.344136 * u - 0.714136 * v;
    float b = y + 1.772 * u;
)";
        }
        fs += R"(
    gl_FragColor = vec4(r, g, b, 1.0);
}
)";
        return CORRECT_FRAGMENT_SHADER(fs);
    }

    void setOrientation(int orientation, bool mirror) {
        bool flipH = !mirror;
        bool flipV = false;
        if (orientation == 90 || orientation == 270) {
            flipH = false;
            flipV = !mirror;
        }
        setTextureCoord(orientation, flipH, flipV);
    }

    void putData(const uint8_t *y, const uint8_t *u, const uint8_t *v,
                 int width, int height, YuvStandard standard = YuvStandard::BT709) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_width = width;
        m_height = height;
        int dstSize = width * height * 3 / 2;
        uint8_t *dst = m_yuv_buffer.obtain<uint8_t>(dstSize);
        memcpy(dst, y, width * height);
        int usize = width * height / 4;
        memcpy(dst + width * height, u, usize);
        int vsize = width * height / 4;
        memcpy(dst + width * height + usize, v, vsize);

        m_next_std = standard;
    }

    void onPreRender(Framebuffer *output) override {
        if (m_standard != m_next_std) {
            m_standard = m_next_std;
            m_program.release();
        }
    }

    void onRender(Framebuffer *output) override {
        const uint8_t *yuv = m_yuv_buffer.bytes();
        if (yuv == nullptr) {
            return;
        }
        int width = m_width, height = m_height;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_y_texture == nullptr || m_u_texture == nullptr || m_v_texture == nullptr
                || m_y_texture->width() != width || m_y_texture->height() != height) {

                if (m_y_texture != nullptr) {
                    m_y_texture->release();
                }
                DELETE_TO_NULL(m_y_texture);
                if (m_u_texture != nullptr) {
                    m_u_texture->release();
                }
                DELETE_TO_NULL(m_u_texture);
                if (m_v_texture != nullptr) {
                    m_v_texture->release();
                }
                DELETE_TO_NULL(m_v_texture);

                TexParams params;
                params.internalFormat = GL_LUMINANCE;
                params.format = GL_LUMINANCE;
                m_y_texture = new Texture2D(width, height, params);

                params.internalFormat = GL_LUMINANCE;
                params.format = GL_LUMINANCE;
                m_u_texture = new Texture2D(width / 2, height / 2, params);
                m_v_texture = new Texture2D(width / 2, height / 2, params);
            }
            m_y_texture->update((void *)yuv);
            m_u_texture->update((void *)(yuv + width * height));
            m_v_texture->update((void *)(yuv + width * height + width * height / 4));
        }

        uniform("yTexture")->set((int)m_y_texture->id());
        uniform("uTexture")->set((int)m_u_texture->id());
        uniform("vTexture")->set((int)m_v_texture->id());

        BaseFilter::onRender(output);
    }

    void release() override {
        BaseFilter::release();
        if (m_y_texture != nullptr) {
            m_y_texture->release();
        }
        DELETE_TO_NULL(m_y_texture);
        if (m_u_texture != nullptr) {
            m_u_texture->release();
        }
        DELETE_TO_NULL(m_u_texture);
        if (m_v_texture != nullptr) {
            m_v_texture->release();
        }
        DELETE_TO_NULL(m_v_texture);
    }

private:
    Array m_yuv_buffer;
    int m_width = 0;
    int m_height = 0;
    YuvStandard m_next_std = YuvStandard::BT709;
    std::mutex m_mutex;

    YuvStandard m_standard = YuvStandard::BT709;
    Texture2D *m_y_texture = nullptr;
    Texture2D *m_u_texture = nullptr;
    Texture2D *m_v_texture = nullptr;
};
NAMESPACE_END
