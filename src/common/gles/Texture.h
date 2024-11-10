//
// Created on 2024/6/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "GLUtil.h"
#include "common/utils/BaseUtils.h"
#include "common/utils/Array.h"

NAMESPACE_DEFAULT


struct TexParams {
    GLint magFilter = GL_LINEAR;
    GLint minFilter = GL_LINEAR;
    GLint wrapS = GL_CLAMP_TO_EDGE;
    GLint wrapT = GL_CLAMP_TO_EDGE;

    GLenum target = GL_TEXTURE_2D;
    GLint level = 0;
    GLint internalFormat = GL_RGBA;
    GLint border = 0;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
};

class Texture {
public:
    Texture(GLuint id, GLuint width, GLuint height) : m_id(id), m_width(width), m_height(height) {}
    Texture(const Texture &other) : m_id(other.m_id), m_width(other.m_width), m_height(other.m_height) {}

    inline GLuint id() const { return m_id; }
    inline GLuint width() const { return m_width; }
    inline GLuint height() const { return m_height; }

    inline bool valid() const { return m_id != INVALID_GL_ID && m_width > 0 && m_height > 0; }

protected:
    GLuint m_id = 0;
    const GLuint m_width;
    const GLuint m_height;
};

class Texture2D : public Texture {
public:
    Texture2D(GLuint width, GLuint height) : Texture(INVALID_GL_ID, width, height) {}
    Texture2D(GLuint width, GLuint height, const TexParams &params)
        : Texture(INVALID_GL_ID, width, height), m_params(params) {}
    Texture2D(const Texture2D &o) : Texture(o), m_params(o.m_params) {}

public:
    void update(const void *pixels) {
        if (valid()) {
            updateTexture2D(m_id, m_width, m_height, m_params, pixels);
        } else {
            m_id = genTexture2D(m_width, m_height, m_params, pixels);
            _INFO("Texture2D created: %d, %d, %d", m_id, m_width, m_height);
        }
    }

    const TexParams& params() const {
        return m_params;
    }

    void release() {
        if (m_id != INVALID_GL_ID) {
            glDeleteTextures(1, &m_id);
            m_id = INVALID_GL_ID;
        }
    }

public:
    static Texture2D create(GLuint width, GLuint height, GLenum format = GL_RGBA) {
        TexParams param = {
            .format = format
        };
        Texture2D tex(width, height, param);
        tex.update(nullptr);
        return tex;
    }

    static void updateTexture2D(GLuint id, GLuint width, GLuint height, const TexParams &params, const void *pixels) {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, params.format, params.type, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    static GLuint genTexture2D(GLuint width, GLuint height, const TexParams &params, const void *pixels) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT);
        glTexImage2D(GL_TEXTURE_2D, params.level, params.internalFormat, width, height, params.border, params.format,
                     params.type, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

private:
    TexParams m_params;
};

class ImageTexture {
public:
    ~ImageTexture() {
        DELETE_TO_NULL(m_tex);
    }

    void set(const uint8_t * data, int width, int height, GLenum format = GL_RGBA) {
        int channels;
        switch (format) {
            case GL_RGBA:
#ifdef GLAPI
            case GL_BGRA:
#endif
                channels = 4;
                break;
#ifdef GLAPI
            case GL_BGR:
#endif
            case GL_RGB:
                channels = 3;
                break;
            case GL_ALPHA:
                channels = 1;
                break;
            default:
                _FATAL("unsupported image format: %d", format);
                break;
        }

        std::lock_guard<std::mutex> lock(m_update_mutex);

        int dataSize = width * height * channels;
        m_img.put(data, dataSize);
        m_width = width;
        m_height = height;
        m_format = format;
        m_tex_need_update = true;
    }

    Texture2D& textureNonnull() {
        Texture2D * tex = texture();
        _FATAL_IF(!tex, "texture is nullptr!!")
        return *tex;
    }

    Texture2D* texture() {
        if (m_width == 0 || m_height == 0) {
            _WARN("image not set! get texture failed!");
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_update_mutex);
        if (m_tex == nullptr || m_tex->width() != m_width || m_tex->height() != m_height || m_tex->params().format != m_format) {
            DELETE_TO_NULL(m_tex);
            TexParams params = {
                    .format = m_format
            };
            m_tex = new Texture2D(m_width, m_height, params);
        }

        if (m_tex_need_update) {
            m_tex->update(m_img.bytes());
            m_tex_need_update = false;
        }

        return m_tex;
    }

private:
    Array m_img;
    int m_width = 0;
    int m_height = 0;
    GLenum m_format = GL_RGBA;
    Texture2D *m_tex = nullptr;
    bool m_tex_need_update = false;

    std::mutex m_update_mutex;
};

NAMESPACE_END