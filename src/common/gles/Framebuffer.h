//
// Created on 2024/6/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "GLUtil.h"
#include "Texture.h"

NAMESPACE_DEFAULT

class Framebuffer {
public:
    void create(GLuint width, GLuint height) {
        if (m_texture == nullptr || m_texture->width() != width || m_texture->height() != height) {
            _INFO("Framebuffer::create(%d, %d)", width, height);
            Texture2D *ntex = new Texture2D(width, height);
            ntex->update(nullptr);
            attachColorTexture(ntex, true);
        }
    }

    inline bool available() const { return m_ref_count == 0 && valid(); }

    inline void ref() { m_ref_count += 1; }

    inline void unref() {
        _WARN_RETURN_IF(m_ref_count == 0, void(), "Framebuffer::unref() ref count == 0, can't unref")
        m_ref_count -= 1;
    }

    inline bool valid() const { return m_texture != nullptr && m_fb_id != INVALID_GL_ID; }

    inline GLuint id() const { return m_fb_id; }

    inline const Texture2D *texture() const { return m_texture; }

    inline const Texture2D& textureNonnull() const {
        _FATAL_IF(!m_texture, "texture of framebuffer is nullptr");
        return *m_texture;
    }

    inline GLuint texID() const { return m_texture == nullptr ? INVALID_GL_ID : m_texture->id(); }

    inline GLuint texWidth() const { return m_texture == nullptr ? 0 : m_texture->width(); }

    inline GLuint texHeight() const { return m_texture == nullptr ? 0 : m_texture->height(); }

    bool bind() {
        if (valid()) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fb_id);
            return true;
        }
        return false;
    }

    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    uint8_t *readPixels() {
        if (bind()) {
            GLuint width = m_texture->width();
            GLuint height = m_texture->height();
            uint8_t *pixels = new uint8_t[width * height * 4];
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            unbind();

            return pixels;
        }
        return nullptr;
    }

    void release() {
        _INFO("Framebuffer(%d)::release()", m_fb_id);
        detachColorTexture();
        if (m_fb_id != INVALID_GL_ID) {
            glDeleteFramebuffers(1, &m_fb_id);
            m_fb_id = INVALID_GL_ID;
        }
        _WARN_IF(m_ref_count > 0, "Framebuffer(%d)::release() ref count: %d > 0", m_fb_id, m_ref_count);
    }

private:
    GLuint createFbId() {
        if (m_fb_id == INVALID_GL_ID) {
            glGenFramebuffers(1, &m_fb_id);
        }
        return m_fb_id;
    }

    bool attachColorTexture(Texture2D *texture, bool own = false) {
        _FATAL_IF(texture == nullptr || !texture->valid(), "Framebuffer::attachColorTexture failed, texture invalid!")
        GLuint fbId = createFbId();

        if (m_texture != nullptr) {
            detachColorTexture();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbId);
        glBindTexture(GL_TEXTURE_2D, texture->id());
        // set texture as colour attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id(), 0);

        // unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        this->m_texture = texture;
        this->m_owning_texture = own;
        _INFO("Framebuffer(%d)::attachColorTexture(%d), own: %d", m_fb_id, m_texture->id(), m_owning_texture);
        return true;
    }

    void detachColorTexture() {
        if (m_texture == nullptr) {
            return;
        }

        _INFO("Framebuffer(%d)e::detachColorTexture(%d), own: %d", m_fb_id, m_texture->id(), m_owning_texture);
        if (m_fb_id != INVALID_GL_ID) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fb_id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if (m_owning_texture) {
            m_texture->release();
            delete m_texture;
        }
        m_texture = nullptr;
    }

private:
    Texture2D *m_texture = nullptr;
    bool m_owning_texture = false;

    GLuint m_fb_id = INVALID_GL_ID;

    int m_ref_count = 0;
};

NAMESPACE_END