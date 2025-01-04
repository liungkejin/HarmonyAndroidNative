//
// Created by LiangKeJin on 2024/11/10.
//

#include "YuvUtils.h"
#include <libyuv.h>

NAMESPACE_DEFAULT

void YuvUtils::rgbaToNV21(const uint8_t *src, int width, int height, uint8_t *dst) {
    libyuv::ARGBToNV21(src, width*4, dst, width, dst+width*height, width, width, height);
}

void YuvUtils::rgbToNV21(const uint8_t *src, int width, int height, uint8_t *dst) {
    libyuv::RGBToNV21(src, width*3, dst, width, dst+width*height, width, width, height);
}

void YuvUtils::bgraToNV21(const uint8_t *src, int width, int height, uint8_t *dst) {
    libyuv::ABGRToNV21(src, width*4, dst, width, dst+width*height, width, width, height);
}

void YuvUtils::bgrToNV21(const uint8_t *src, int width, int height, uint8_t *dst) {
    libyuv::BGRToNV21(src, width*3, dst, width, dst+width*height, width, width, height);
}

// 缩放 nv21 数据
// @param tempMem 是临时内存，用来存储临时的uv, 大小至少为 width*height/2 + dstWidth*dstHeight/2
// @param filterType FilterMode::kFilterNone = 0, kFilterLinear = 1, kFilterBilinear = 2, kFilterBox = 3
void YuvUtils::scaleNV21(const uint8_t *src, int width, int height, 
    uint8_t *dst, int dstWidth, int dstHeight, uint8_t *tempMem, int filterType) {
    
    libyuv::FilterMode filterMode;
    if (filterType == 1) {
        filterMode = libyuv::FilterMode::kFilterLinear;
    } else if (filterType == 2) {
        filterMode = libyuv::FilterMode::kFilterBilinear;
    } else if (filterType == 3) {
        filterMode = libyuv::FilterMode::kFilterBox;
    } else {
        filterMode = libyuv::FilterMode::kFilterNone;
    }

    // 先 scale Y plane
    libyuv::ScalePlane(src, width, width, height, dst, dstWidth, dstWidth, dstHeight, filterMode);

    uint8_t *ownMem = nullptr;
    if (tempMem == nullptr) {
        ownMem = (uint8_t *)malloc(width * height / 2 + dstWidth * dstHeight / 2 + 100);
        tempMem = ownMem;
    }

    // 这里要保证 width, height 能被 2 整除
    int halfWidth = (width + 1) >> 1;
    int halfHeight = (height + 1) >> 1;
    const uint8_t *vu = src + width * height;
    uint8_t *tempV = tempMem;
    uint8_t *tempU = tempV + halfWidth * halfHeight;
    // 然再分离 vu
    libyuv::SplitUVPlane(vu, width, tempV, halfWidth, tempU, halfWidth, halfWidth, halfHeight);

    // 再 resize v 和 u
    int halfDstWidth = (dstWidth + 1) >> 1;
    int halfDstHeight = (dstHeight + 1) >> 1;
    uint8_t *tempScaleV = tempU + halfWidth * halfHeight;
    uint8_t *tempScaleU = tempScaleV + halfDstWidth * halfDstHeight;
    libyuv::ScalePlane(tempV, halfWidth, halfWidth, halfHeight, tempScaleV, halfDstWidth, halfDstWidth,
                       halfDstHeight, filterMode);
    libyuv::ScalePlane(tempU, halfWidth, halfWidth, halfHeight, tempScaleU, halfDstWidth, halfDstWidth,
                       halfDstHeight, filterMode);

    // 再 merge 回去
    uint8_t *dstVU = dst + dstWidth * dstHeight;
    libyuv::MergeUVPlane(tempScaleV, halfDstWidth, tempScaleU, halfDstWidth, dstVU, dstWidth, halfDstWidth,
                         halfDstHeight);
    if (ownMem != nullptr) {
        free(ownMem);
    }
}
NAMESPACE_END
