//
// Created by LiangKeJin on 2024/7/27.
//
#pragma once

#ifdef _WIN32
#include <GL/glew.h>
#endif
#include "imgui.h"


class GLRenderer {
public:
    static int run(int width=1280, int height=720, const char *title="ZNativeSample");

private:
    // 初始化, 只调用一次
    static void onInit(int width, int height);

    // 在 Imgui::render() 之前
    static void onRender(int width, int height);

    // 画 imgui
    static void onRenderImgui(int width, int height, ImGuiIO& io);

    // 在 Imgui::render() 之后
    static void onPostRender(int width, int height);

    // 窗口关闭
    static void onExit();
};