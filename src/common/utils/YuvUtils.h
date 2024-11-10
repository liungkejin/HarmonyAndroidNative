//
// Created on 2024/8/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include <cstdint>

NAMESPACE_DEFAULT

class YuvUtils {
public:
    
    static void rgbaToNV21(uint8_t *src, int width, int height, uint8_t *dst);
    
    // 缩放 nv21 数据
    // @param tempMem 是临时内存，用来存储临时的uv, 大小至少为 width*height/2 + dstWidth*dstHeight/2
    // @param filterType FilterMode::kFilterNone = 0, kFilterLinear = 1, kFilterBilinear = 2, kFilterBox = 3
    static void scaleNV21(uint8_t *src, int width, int height, uint8_t *dst, int dstWidth, int dstHeight,
                          uint8_t *tempMem, int filterType = 1);
};

NAMESPACE_END
