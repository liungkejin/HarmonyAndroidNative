//
// Created by LiangKeJin on 2025/3/16.
//

#pragma once

#include "LibusbMgr.h"
#include "../IWindow.h"

class LibusbWindow : public IWindow, znative::LibusbDeviceListener {
public:
    LibusbWindow() : IWindow("测试 Libusb") {

    }
public:
    void onAppInit(int width, int height) override;

    void onAppExit() override;

    void onPreRender(int width, int height) override;

    void onRenderImgui(int width, int height, ImGuiIO &io) override;

public:
    void onDevicePlug(znative::LibusbDevice& dev) override;

    void onDeviceUnplug(znative::LibusbDevice& dev) override;
};

