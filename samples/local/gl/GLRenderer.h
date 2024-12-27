//
// Created by LiangKeJin on 2024/7/27.
//
#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


class GLRenderer {
public:
    static int run(int width=1280, int height=720, const char *title="MyCppOpenGL");

private:
    // 在 Imgui::render() 之前
    static void onRender(int width, int height);

    // 画 imgui
    static void onRenderImgui(int width, int height, ImGuiIO& io);

    // 在 Imgui::render() 之后
    static void onPostRender(int width, int height);

    // 窗口关闭
    static void onExit();
};