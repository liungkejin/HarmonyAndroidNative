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
};
static GuiVars guiVars;

TextureFilter texFilter;
SharpenFilter sharpenFilter;

FramebufferPool framebufferPool;

ImageTexture imageTexture;
GLRect vertexRect;

void GLRenderer::onRender(int width, int height) {
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

    vertexRect = GLRect::fitCenter((float) tex.width(), (float) tex.height(), (float) width, (float) height);
    texFilter.setVertexCoord(vertexRect, (float) width, (float) height);
    texFilter.setTextureCoord(0, false, true);
    texFilter.inputTexture(tex);
    texFilter.alpha(guiVars.alpha_percent);
    texFilter.blend(true);
    texFilter.render();
}

void GLRenderer::onRenderImgui(int width, int height, ImGuiIO &io) {
    if (guiVars.show_demo_window) {
        ImGui::ShowDemoWindow(&guiVars.show_demo_window);
    }

    {
        ImGui::Begin("ShaderToy");

        // Edit bools storing our window open/close state
        ImGui::Checkbox("Demo Window", &guiVars.show_demo_window);

        // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("percent", &guiVars.alpha_percent, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float *) &guiVars.clear_color); // Edit 3 floats representing a color

        ImGui::SameLine();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
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
}