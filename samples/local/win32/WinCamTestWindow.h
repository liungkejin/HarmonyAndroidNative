//
// Created by wutacam on 2025/1/14.
//

#pragma once

#include <common/Common.h>
#include "../IWindow.h"

class WinCamTestWindow : public IWindow {
public:
    WinCamTestWindow() : IWindow("WinCamTestWindow") {}

protected:
    void onPreRender(int width, int height) override;

    void onRenderImgui(int width, int height, ImGuiIO &io) override;
};

