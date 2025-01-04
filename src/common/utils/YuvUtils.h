//
// Created on 2024/8/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/utils/Array.h"
#include <cstdint>

NAMESPACE_DEFAULT

class YuvUtils {
public:
    static void rgbaToNV21(const uint8_t *src, int width, int height, uint8_t *dst);

    // 缩放 nv21 数据
    // @param tempMem 是临时内存，用来存储临时的uv, 大小至少为 width*height/2 + dstWidth*dstHeight/2
    // @param filterType FilterMode::kFilterNone = 0, kFilterLinear = 1, kFilterBilinear = 2, kFilterBox = 3
    static void scaleNV21(const uint8_t *src, int width, int height, uint8_t *dst, int dstWidth, int dstHeight,
                          uint8_t *tempMem, int filterType = 1);
};

class NV21Image {
public:
    void scaleTo(uint8_t *dst, int dstWidth, int dstHeight, int filterType = 1) {
        uint8_t *tmp = m_temp.obtain<uint8_t>(m_width * m_height / 2 + dstWidth * dstHeight / 2 + 100);
        YuvUtils::scaleNV21(m_data.bytes(), m_width, m_height, 
        dst, dstWidth, dstHeight, tmp, filterType);
    }
    
    void scaleTo(NV21Image &dst, int filterType = 1) {
        scaleTo(dst.data(), dst.width(), dst.height(), filterType);
    }
    
    void put(uint8_t *src, int width, int height) {
        m_width = width;
        m_height = height;
        m_data.put(src, width * height * 3 / 2);
    }
    
    void create(int width, int height) {
        m_width = width;
        m_height = height;
        m_data.obtain<uint8_t>(width * height * 3 / 2);
    }
    
    inline int width() const { return m_width; }
    
    inline int height() const { return m_height; }
    
    inline uint8_t *data() { return m_data.obtain<uint8_t>(0); }
    
private:
    int m_width = 0;
    int m_height = 0;
    Array m_data;
    
    Array m_temp;
};

NAMESPACE_END
