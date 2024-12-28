//
// Created by LiangKeJin on 2024/7/27.
//
#include "GLRenderer.h"
#include "common/gles/GLUtil.h"
#include "common/gles/GLCoord.h"
#include "common/gles/Texture.h"
#include "common/gles/filter/TextureFilter.h"
#include "common/gles/filter/SharpenFilter.h"
#include "common/gles/FramebufferPool.h"
#include "common/AppContext.h"
#include <opencv2/opencv.hpp>

using namespace znative;

struct GuiVars {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = false;
    float alpha_percent = 1.0f;
    bool enable_sharpen = true;
    float sharpen_strength = 0.0f;
};
static GuiVars guiVars;

TextureFilter texFilter;
SharpenFilter sharpenFilter;

FramebufferPool framebufferPool;

ImageTexture imageTexture;
GLRect vertexRect;

void GLRenderer::onRender(int viewWidth, int viewHeight) {
    auto &ccolor = guiVars.clear_color;
    GLUtil::clearColor(ccolor.x, ccolor.y, ccolor.z, ccolor.w);

    if (!imageTexture.valid()) {
        std::string testImgPath = AppContext::filesDir() + "/images/lyf.jpg";
        cv::Mat img = cv::imread(testImgPath);
        _FATAL_IF(img.empty(), "load image failed: %s", testImgPath.c_str());
        cv::Mat rgba;
        cv::cvtColor(img, rgba, cv::COLOR_BGR2RGBA);
        imageTexture.set(rgba.data, rgba.cols, rgba.rows, GL_RGBA);
    }

    Texture2D &tex = imageTexture.textureNonnull();

    FramebufferRef outputFb;
    if (guiVars.enable_sharpen) {
        auto sharpenFb= framebufferPool.obtain(tex.width(), tex.height());
        // sharpen filter
        sharpenFilter.setFullVertexCoord().setFullTextureCoord();
        sharpenFilter.viewport()
                .set(sharpenFb->texWidth(), sharpenFb->texHeight())
                .enableClearColor(1, 0, 0, 1);
        sharpenFilter.inputTexture(tex);
        sharpenFilter.setStrength(guiVars.sharpen_strength);
        sharpenFilter.setResolution((float) tex.width()/2.f, (float) tex.height()/2.f);
        sharpenFilter.render(sharpenFb.get());

        outputFb = sharpenFb;
    }

    const Texture2D &finalTex = outputFb.get() == nullptr ? tex : outputFb->textureNonnull();
    // 最后渲染到屏幕
    vertexRect = GLRect::fitCenter((float) finalTex.width(), (float) finalTex.height(),
                                   (float) viewWidth, (float) viewHeight);
    texFilter.setViewport(viewWidth, viewHeight)
            .setVertexCoord(vertexRect, (float) viewWidth, (float) viewHeight)
            .setTextureCoord(0, false, true);
    texFilter.inputTexture(finalTex);
    texFilter.alpha(guiVars.alpha_percent);
    texFilter.blend(true);
    texFilter.render();
}

void GLRenderer::onRenderImgui(int width, int height, ImGuiIO &io) {
    if (guiVars.show_demo_window) {
        ImGui::ShowDemoWindow(&guiVars.show_demo_window);
    }

    {
        // 创建一个名叫"ShaderToy"的窗口，ImGui::End()之前的所有ImGui控件都会在这个窗口中显示
        ImGui::Begin("ShaderToy");

        // Edit bools storing our window open/close state
        ImGui::Checkbox("Demo Window", &guiVars.show_demo_window);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::Checkbox("enable sharpen", &guiVars.enable_sharpen);
        if (guiVars.enable_sharpen) {
            ImGui::SliderFloat("sharpen strength", &guiVars.sharpen_strength, 0.0f, 1.0f);
        }

        ImGui::SliderFloat("alpha percent", &guiVars.alpha_percent, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float *) &guiVars.clear_color); // Edit 3 floats representing a color

//        ImGui::SameLine();
        ImGui::End();
    }
}

void GLRenderer::onPostRender(int width, int height) {
    //
}

void GLRenderer::onExit() {
    imageTexture.release();
    texFilter.release();
    sharpenFilter.release();
    framebufferPool.release();
}