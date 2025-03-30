//
// Created by LiangKeJin on 2025/3/16.
//

#pragma once

#include "LibusbMgr.h"
#include "../IWindow.h"

class LibusbWindow : public IWindow, znative::LibusbDeviceListener, znative::LibusbDeviceTransferListener {
public:
    LibusbWindow() : IWindow("测试 Libusb") {

    }
public:
    void onAppInit(int width, int height) override;

    void onAppExit() override;

protected:
    void onVisible(int width, int height) override;

    void onInvisible(int width, int height) override;

public:
    void onPreRender(int width, int height) override;

    void onRenderImgui(int width, int height, ImGuiIO &io) override;

public:
    void onDevicePlug(const znative::LibusbDeviceInfo& dev) override;

    void onDeviceUnplug(const znative::LibusbDeviceInfo& dev) override;

    void onDeviceListUpdate(std::list<znative::LibusbDeviceInfo> devList) override;

public:
    void onDataRecv(const uint8_t* data, const int len) override;

    void onDataSend(const int len) override;
};

