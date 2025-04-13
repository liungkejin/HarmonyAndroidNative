//
// Created by LiangKeJin on 2025/4/13.
//

#pragma once

#include <common/Common.h>
#include "../IWindow.h"


class NetTestWindow : public IWindow {

public:
    NetTestWindow() : IWindow("NetTestWindow") {}

protected:
    void onVisible(int width, int height) override;

    void onInvisible(int width, int height) override;

    void onPreRender(int width, int height) override;

    void onRenderImgui(int width, int height, ImGuiIO &io) override;
};

