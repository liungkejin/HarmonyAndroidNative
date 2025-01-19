//
// Created by LiangKeJin on 2024/7/27.
//
#pragma once

#ifdef _WIN32
#include <GL/glew.h>
#endif
#include "imgui.h"
#include "gl/GLTestWindow.h"

class MainApp {
public:
    static int run(int width = 1280, int height = 720, const char *title = "ZNative");
};
