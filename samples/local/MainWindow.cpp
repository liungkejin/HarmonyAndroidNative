//
// Created by LiangKeJin on 2024/7/27.
//

#include "MainWindow.h"
#include "IWindow.h"

#include <cstdio>
#include <common/gles/GLUtil.h>

#include "gl/GLTestWindow.h"

struct WindowHost {
    IWindow* window;
    bool visible = false;
    bool gui_checked = false;
};

static GLTestWindow g_gl_test_window;
static WindowHost g_gl_test_window_host = { &g_gl_test_window, false, false };

std::vector<WindowHost *> g_all_windows;

static bool g_show_demo_window = false;
static ImVec4 g_background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void MainWindow::onInit(int width, int height) {
    std::string version = znative::GLUtil::glVersion();
    _INFO("onInit, window size(%dx%d), GL version: %s", width, height, version);

    g_all_windows.push_back(&g_gl_test_window_host);

    for (auto * p : g_all_windows) {
        p->window->onAppInit(width, height);
    }
}

void MainWindow::onPreRender(int width, int height) {
    znative::GLUtil::clearColor(g_background_color.x, g_background_color.y, g_background_color.z, g_background_color.w);
    for (auto * p : g_all_windows) {
        if (p->visible && !p->gui_checked) {
            p->window->onInvisible(width, height);
        } else if (!p->visible && p->gui_checked) {
            p->window->onVisible(width, height);
        }
        p->visible = p->gui_checked;
    }

    for (auto * p : g_all_windows) {
        if (p->visible) {
            p->window->onPreRender(width, height);
        }
    }
}

void MainWindow::onRenderImgui(int width, int height, ImGuiIO &io) {
    if (g_show_demo_window) {
        ImGui::ShowDemoWindow(&g_show_demo_window);
    }

    ImGui::Begin("Main");

    // Edit bools storing our window open/close state
    ImGui::Checkbox("显示 demo window", &g_show_demo_window);
    ImGui::ColorEdit3("背景颜色", (float *) &g_background_color); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::Checkbox("GL Test Window", &g_gl_test_window_host.gui_checked);
    ImGui::End();

    for (auto * p : g_all_windows) {
        if (p->visible) {
            p->window->onRenderImgui(width, height, io);
        }
    }
}

void MainWindow::onPostRender(int width, int height) {
    for (auto * p : g_all_windows) {
        if (p->visible) {
            p->window->onPostRender(width, height);
        }
    }
}

void MainWindow::onExit() {
    for (auto * p : g_all_windows) {
        p->window->onAppExit();
    }
}