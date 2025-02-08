//
// Created by LiangKeJin on 2024/7/27.
//
#pragma once

#include <imgui.h>

class MainWindow {
public:
    // 初始化, 只调用一次
    static void onInit(int width, int height);

    // 在 Imgui::render() 之前
    static void onPreRender(int width, int height);

    // 画 imgui
    static void onRenderImgui(int width, int height, ImGuiIO& io);

    // 在 Imgui::render() 之后
    static void onPostRender(int width, int height);

    // 窗口关闭
    static void onExit();
};
