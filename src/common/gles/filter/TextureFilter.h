//
// Created on 2024/7/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "BaseFilter.h"

NAMESPACE_DEFAULT

class TextureFilter : public BaseFilter {
public:
    TextureFilter() : BaseFilter("texture_filter") {
        defAttribute("position", DataType::FLOAT_POINTER)->bind(vertexCoord());
        defAttribute("inputTextureCoordinate", DataType::FLOAT_POINTER)->bind(textureCoord());
        defUniform("inputImageTexture", DataType::SAMPLER_2D);
        defUniform("alpha", DataType::FLOAT)->set(1.0f);
    }

    std::string vertexShader() override {
        std::string vs = R"(
attribute vec4 position;
attribute vec2 inputTextureCoordinate;
varying highp vec2 textureCoordinate;
void main() {
    gl_Position = position;
    textureCoordinate = inputTextureCoordinate;
})";
        return CORRECT_VERTEX_SHADER(vs);
    }

    std::string fragmentShader() override {
        std::string fs = R"(
varying highp vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform mediump float alpha;
void main() {
    highp vec4 c = texture2D(inputImageTexture, textureCoordinate);
    gl_FragColor = vec4(c.rgb, c.a*alpha);
})";
        return CORRECT_FRAGMENT_SHADER(fs);
    }

    TextureFilter &inputTexture(int id) {
        uniform("inputImageTexture")->set(id);
        return *this;
    }

    TextureFilter &inputTexture(const Texture &texture) {
        uniform("inputImageTexture")->set((int)texture.id());
        return *this;
    }

    TextureFilter &blend(bool enable) {
        m_blend = enable;
        return *this;
    }

    TextureFilter &alpha(float a) {
        uniform("alpha")->set(a);
        return *this;
    }
    
    void simpleRender(const Texture &tex, Framebuffer *output = nullptr) {
        simpleRender(tex.id(), tex.width(), tex.height(), output);
    }
    
    void simpleRender(int id, int width, int height, Framebuffer *output = nullptr) {
        viewport().set(width, height).enableClearColor(0, 0, 0, 1);
        setFullTextureCoord().setFullVertexCoord();
        inputTexture(id).blend(false).render(output);
    }
    
    void centerCropRender(const Texture &tex, Framebuffer &output, int rotation = 0) {
        viewport().set(output.texWidth(), output.texHeight());
        GLRect rect;
        if (rotation == 90 || rotation == 270) {
            rect = GLRect::centerCrop(tex.height(), tex.width(), output.texWidth(), output.texHeight());
        } else {
            rect = GLRect::centerCrop(tex.width(), tex.height(), output.texWidth(), output.texHeight());
        }
        setVertexCoord(rect, output.texWidth(), output.texHeight()).setTextureCoord(rotation, false, false);
        inputTexture(tex.id()).blend(false).render(&output);
    }
    
    void centerCropRender(Framebuffer &tex, int viewWidth, int viewHeight, int texRotation = 0) {
        viewport().set(viewWidth, viewHeight);
        GLRect rect;
        if (texRotation == 90 || texRotation == 270) {
            rect = GLRect::centerCrop(tex.texHeight(), tex.texWidth(),viewWidth, viewHeight);
        } else {
            rect = GLRect::centerCrop(tex.texWidth(), tex.texHeight(), viewWidth, viewHeight);
        }
        
        setVertexCoord(rect, viewWidth, viewHeight).setTextureCoord(texRotation, false, false);
        inputTexture(tex.texID()).blend(false).render();
    }
    
    void onRender(Framebuffer *output) override {
        if (m_blend) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
        BaseFilter::onRender(output);
    }

    void onPostRender(Framebuffer *output) override {
        BaseFilter::onPostRender(output);
        if (m_blend) {
            glDisable(GL_BLEND);
        }
    }

private:
    bool m_blend = false;
};

NAMESPACE_END
