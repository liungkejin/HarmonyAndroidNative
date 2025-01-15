//
// Created by bigheadson on 2025/1/5.
//

#pragma once

// 注意这里，不知道时因为原因，opencv的头文件必须在其他头文件之前，
// 可能因为在 znative 库中使用了 #pragma once 导致的，但是也只在使用 Visual Studio工具链时才这样
#include <opencv2/opencv.hpp>

class ZTest {
public:
    static void test_ZImage();
};