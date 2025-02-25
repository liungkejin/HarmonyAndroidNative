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

class NV12Filter : public BaseFilter {
public:
    NV12Filter() : BaseFilter("nv12") {
        defAttribute("position", DataType::FLOAT_POINTER)->bind(vertexCoord());
        defAttribute("inputTextureCoordinate", DataType::FLOAT_POINTER)->bind(textureCoord());
        defUniform("yTexture", DataType::SAMPLER_2D);
        defUniform("uvTexture", DataType::SAMPLER_2D);
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
uniform sampler2D uvTexture;

void main() {
    float y = texture2D(yTexture, textureCoordinate).r;
    vec4 uv = texture2D(uvTexture, textureCoordinate);
    float v = uv.a - 0.5;
    float u = uv.r - 0.5;
)";
        if (m_standard == BT601) {
            fs = R"(
    float r = y + 1.402 * v;
    float g = y - 0.344136 * u - 0.714136 * v;
    float b = y + 1.772 * u;
)";
        } else if (m_standard == BT2020) {
            fs = R"(
    float r = y + 1.4746 * v;
    float g = y - 0.164553 * u - 0.571353 * v;
    float b = y + 1.8814 * u;
)";
        } else {
            fs = R"(
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

    void putData(const uint8_t *nv12, int width, int height, YuvStandard standard = YuvStandard::BT709) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_width = width;
        m_height = height;
        int dstSize = width * height * 3 / 2;
        uint8_t *dst = m_nv12_buffer.obtain<uint8_t>(dstSize);
        memcpy(dst, nv12, dstSize);
        m_next_std = standard;
    }

    void onPreRender(Framebuffer *output) override {
        if (m_standard != m_next_std) {
            m_standard = m_next_std;
            m_program.release();
        }
    }

    void onRender(Framebuffer *output) override {
        const uint8_t *nv12 = m_nv12_buffer.bytes();
        if (nv12 == nullptr) {
            return;
        }
        int width = m_width, height = m_height;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_y_texture == nullptr || m_uv_texture == nullptr || m_y_texture->width() != width ||
                m_y_texture->height() != height) {
                if (m_y_texture) {
                    m_y_texture->release();
                    DELETE_TO_NULL(m_y_texture);
                }

                if (m_uv_texture) {
                    m_uv_texture->release();
                    DELETE_TO_NULL(m_uv_texture);
                }

                TexParams params;
                params.internalFormat = GL_LUMINANCE;
                params.format = GL_LUMINANCE;
                m_y_texture = new Texture2D(width, height, params);

                params.internalFormat = GL_LUMINANCE_ALPHA;
                params.format = GL_LUMINANCE_ALPHA;
                m_uv_texture = new Texture2D(width / 2, height / 2, params);
            }
            m_y_texture->update((void *)nv12);
            m_uv_texture->update((void *)(nv12 + width * height));
        }

        uniform("yTexture")->set((int)m_y_texture->id());
        uniform("uvTexture")->set((int)m_uv_texture->id());

        BaseFilter::onRender(output);
    }

    void release() override {
        BaseFilter::release();
        if (m_y_texture != nullptr) {
            m_y_texture->release();
        }
        DELETE_TO_NULL(m_y_texture);
        if (m_uv_texture != nullptr) {
            m_uv_texture->release();
        }
        DELETE_TO_NULL(m_uv_texture);
    }

private:
    Array m_nv12_buffer;
    int m_width = 0;
    int m_height = 0;
    YuvStandard m_next_std = YuvStandard::BT709;
    std::mutex m_mutex;

    YuvStandard m_standard = YuvStandard::BT709;
    Texture2D *m_y_texture = nullptr;
    Texture2D *m_uv_texture = nullptr;
};
NAMESPACE_END
