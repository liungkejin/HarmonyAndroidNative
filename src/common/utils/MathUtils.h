//
// Created on 2024/7/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include <cmath>
#ifdef _MSC_VER
#include <corecrt_math_defines.h>
#endif

NAMESPACE_DEFAULT

class MathUtils {
public:
    static void rotatePoint(float &x, float &y, float cx, float cy, float angle) {
        float radians = angle / 180.0f * M_PI;
        float sinTheta = sin(radians);
        float cosTheta = cos(radians);
        rotatePoint(x, y, cx, cy, sinTheta, cosTheta);
    }
    
    static void rotatePoint(float &x, float &y, float cx, float cy, float sinTheta, float cosTheta) {
        float dx = x - cx;
        float dy = y - cy;
        float nx = dx * cosTheta - dy * sinTheta;
        float ny = dx * sinTheta + dy * cosTheta;
        x = nx + cx;
        y = ny + cy;
    }
};

NAMESPACE_END
