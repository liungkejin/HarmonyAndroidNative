//
// Created on 2024/6/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "../Framebuffer.h"
#include "../GLCoord.h"
#include "../Program.h"
#include "../Viewport.h"

NAMESPACE_DEFAULT

class BaseFilter {
public:
    virtual ~BaseFilter() = default;

    explicit BaseFilter(const char *name) : m_name(name) {}

    Attribute *attribute(const char *name) { return m_program.attribute(name); }

    Uniform *uniform(const char *name) { return m_program.uniform(name); }

    Viewport &viewport() { return m_viewport; }

    BaseFilter& setViewport(const Viewport &viewport) {
        m_viewport.set(viewport);
        return *this;
    }

    BaseFilter& setViewport(int width, int height, bool scissor = false) {
        m_viewport.set(width, height, scissor);
        return *this;
    }

    BaseFilter& setViewport(int x, int y, int width, int height, bool scissor = false) {
        m_viewport.set(x, y, width, height, scissor);
        return *this;
    }

    virtual BaseFilter& setVertexCoord(const GLRect &rect, float viewW, float viewH) {
        m_vertex_coords.setByGLRect(viewW, viewH, rect);
        return *this;
    }

    BaseFilter& setFullVertexCoord() {
        m_vertex_coords.setToDefault();
        return *this;
    }

    virtual BaseFilter& setVertexCoord(const float *ps, int size, GLenum drawMode, int drawCount) {
        m_vertex_coords.set(ps, size, drawMode, drawCount);
        return *this;
    }

    BaseFilter& setFullTextureCoord() {
        m_texture_coords.setToDefault();
        return *this;
    }

    virtual BaseFilter& setTextureCoord(const GLRect &rect, float texW, float texH) {
        m_texture_coords.setByGLRect(texW, texH, rect);
        return *this;
    }

    // rotation 只支持 0, 90, 180, 270
    virtual BaseFilter& setTextureCoord(int rotation, bool flipH, bool flipV) {
        m_texture_coords.setFullCoord(rotation, flipH, flipV);
        return *this;
    }

    virtual BaseFilter& setTextureCoord(const float *ps, int size, GLenum drawMode, int drawCount) {
        // 对于纹理坐标，draw mode 无效
        m_texture_coords.set(ps, size, drawMode, drawCount);
        return *this;
    }

    void render(Framebuffer *output = nullptr) {
        if (!m_program.valid()) {
            std::string vs = vertexShader();
            std::string fs = fragmentShader();
            if (!m_program.create(vs.c_str(), fs.c_str())) {
                return;
            }
            onProgramCreated();
        }

        if (output) {
            output->ref();
            output->bind();
        }

        onViewport();

        if (!m_program.attach()) {
            _ERROR("Couldn't attach filter(%s) program", m_name.c_str());
            return;
        }

        onRender(output);
        onDrawArrays();

        if (output) {
            output->unbind();
            output->unref();
        }

        onPostRender(output);
        m_program.detach();
    }

    virtual void release() { m_program.release(); }

protected:
    virtual std::string vertexShader() = 0;

    virtual std::string fragmentShader() = 0;

    Program& program() { return m_program; }

    Attribute *defAttribute(const char *name, DataType type) { return m_program.defAttribute(name, type); }

    Uniform *defUniform(const char *name, DataType type) { return m_program.defUniform(name, type); }

    VertexCoord &vertexCoord() { return m_vertex_coords; }

    TextureCoord &textureCoord() { return m_texture_coords; }

    virtual void onProgramCreated() {}

    virtual void onViewport() { m_viewport.apply(); }

    virtual void onRender(Framebuffer *output) { m_program.input(); }

    virtual void onDrawArrays() {
        glDrawArrays(m_vertex_coords.drawMode(), 0, m_vertex_coords.drawCount());
    }

    virtual void onPostRender(Framebuffer *output) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    const std::string m_name;

    Viewport m_viewport = Viewport(0, 0);
    Program m_program;

    VertexCoord m_vertex_coords;
    TextureCoord m_texture_coords;
};

NAMESPACE_END
