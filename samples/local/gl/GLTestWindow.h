//
// Created by wutacam on 2025/1/14.
//

#pragma once

#include <imgui.h>
#include <common/Common.h>
#include "../IWindow.h"

class GLTestWindow : public IWindow {
public:
    GLTestWindow() : IWindow("GLTestWindow") {
    }

public:
    void onPreRender(int width, int height) override;

    void onRenderImgui(int width, int height, ImGuiIO &io) override;

    void onAppExit() override;
};
