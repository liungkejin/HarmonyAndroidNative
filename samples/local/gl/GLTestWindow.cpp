//
// Created by LiangKeJin on 2024/7/27.
//
#include "GLTestWindow.h"

#include "common/gles/GLUtil.h"
#include "common/gles/GLCoord.h"
#include "common/gles/Texture.h"
#include "common/gles/filter/TextureFilter.h"
#include "common/gles/filter/SharpenFilter.h"
#include "common/gles/FramebufferPool.h"
#include "common/AppContext.h"
#include "common/gles/filter/ColorAdjustFilter.h"

#include "../test/ZTest.h"

using namespace znative;

struct GuiVars {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float alpha_percent = 1.0f;
    bool enable_sharpen = true;
    float sharpen_strength = 0.0f;

    bool enable_color_adjust = true;
    float brightness_strength = 0.0f;
    float contrast_strength = 0.0f;
    float saturation_strength = 0.0f;
    float wb_temperature_strength = 0.0f;
    float wb_tint_strength = 0.0f;
    float exposure_strength = 0.0f;
    float highlights_strength = 0.0f;
    float shadows_strength = 0.0f;
    float film_grain_strength = 0.0f;
};
static GuiVars guiVars;

TextureFilter texFilter;
SharpenFilter sharpenFilter;
ColorAdjustFilter colorAdjustFilter;

FramebufferPool framebufferPool;

ImageTexture imageTexture;
GLRect vertexRect;

void GLTestWindow::onPreRender(int viewWidth, int viewHeight) {
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
                .set(sharpenFb->texWidth(), sharpenFb->texHeight());
        sharpenFilter.inputTexture(tex);
        sharpenFilter.setStrength(guiVars.sharpen_strength);
        sharpenFilter.setResolution((float) tex.width()/2.f, (float) tex.height()/2.f);
        sharpenFilter.render(sharpenFb.get());

        outputFb = sharpenFb;
    }

    if (guiVars.enable_color_adjust) {
        const Texture2D &inTex = outputFb.get() == nullptr ? tex : outputFb->textureNonnull();

        auto colorFb = framebufferPool.obtain(inTex.width(), inTex.height());
        // color adjust filter
        colorAdjustFilter.setFullVertexCoord().setFullTextureCoord();
        colorAdjustFilter.viewport()
                .set(colorFb->texWidth(), colorFb->texHeight());
        colorAdjustFilter.inputTexture((int)inTex.id());
        colorAdjustFilter.setBrightness(guiVars.brightness_strength);
        colorAdjustFilter.setContrast(guiVars.contrast_strength);
        colorAdjustFilter.setSaturation(guiVars.saturation_strength);
        colorAdjustFilter.setWhiteBalanceTemperature(guiVars.wb_temperature_strength);
        colorAdjustFilter.setWhiteBalanceTint(guiVars.wb_tint_strength);
        colorAdjustFilter.setExposure(guiVars.exposure_strength);
        colorAdjustFilter.setHighlights(guiVars.highlights_strength);
        colorAdjustFilter.setShadows(guiVars.shadows_strength);
        colorAdjustFilter.setFilmGrain(guiVars.film_grain_strength);
        colorAdjustFilter.render(colorFb.get());

        outputFb = colorFb;
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

void GLTestWindow::onRenderImgui(int width, int height, ImGuiIO &io) {
    {
        // 创建一个名叫"ShaderToy"的窗口，ImGui::End()之前的所有ImGui控件都会在这个窗口中显示
        ImGui::Begin("GLTestWindow");

        ImGui::SliderFloat("alpha percent", &guiVars.alpha_percent, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float *) &guiVars.clear_color); // Edit 3 floats representing a color

        ImGui::Checkbox("enable sharpen", &guiVars.enable_sharpen);
        if (guiVars.enable_sharpen) {
            ImGui::SliderFloat("sharpen strength", &guiVars.sharpen_strength, 0.0f, 1.0f);
        }

        ImGui::Checkbox("enable color adjust", &guiVars.enable_color_adjust);
        if (guiVars.enable_color_adjust) {
            if (ImGui::Button("reset color adjust")) {
                // reset all color adjust params
                guiVars.brightness_strength = 0.0f;
                guiVars.contrast_strength = 0.0f;
                guiVars.saturation_strength = 0.0f;
                guiVars.wb_temperature_strength = 0.0f;
                guiVars.wb_tint_strength = 0.0f;
                guiVars.exposure_strength = 0.0f;
                guiVars.highlights_strength = 0.0f;
                guiVars.shadows_strength = 0.0f;
                guiVars.film_grain_strength = 0.0f;
            }
            ImGui::SliderFloat("brightness", &guiVars.brightness_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("contrast", &guiVars.contrast_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("saturation", &guiVars.saturation_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("wb_temperature", &guiVars.wb_temperature_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("wb_tint", &guiVars.wb_tint_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("exposure", &guiVars.exposure_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("highlights", &guiVars.highlights_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("shadows", &guiVars.shadows_strength, -1.0f, 1.0f);
            ImGui::SliderFloat("film grain", &guiVars.film_grain_strength, 0.0f, 1.0f);
        }

        if (ImGui::Button("test ZImage")) {
            ZTest::test_ZImage();
        }

//        ImGui::SameLine();
        ImGui::End();
    }
}

void GLTestWindow::onAppExit() {
    imageTexture.release();
    texFilter.release();
    sharpenFilter.release();
    colorAdjustFilter.release();
    framebufferPool.release();
}