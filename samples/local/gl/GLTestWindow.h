//
// Created by wutacam on 2025/1/14.
//

#pragma once

// 注意这里，不知道时因为原因，opencv的头文件必须在其他头文件之前，
// 可能因为在 znative 库中使用了 #pragma once 导致的，但是也只在使用 Visual Studio工具链时才这样
#include <opencv2/opencv.hpp>
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
