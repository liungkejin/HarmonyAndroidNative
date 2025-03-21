//
// Created by LiangKeJin on 2025/3/16.
//

#pragma once

#include "../IWindow.h"

class LibusbWindow : public IWindow {
public:
    LibusbWindow() : IWindow("测试 Libusb") {

    }
public:
    void onAppInit(int width, int height) override;

    void onAppExit() override;

    void onRenderImgui(int width, int height, ImGuiIO &io) override;
};

