//
// Created by bigheadson on 2025/1/5.
//

#include "ZTest.h"

#include <common/media/img/ZImage.h>

#include "common/AppContext.h"

using namespace znative;

void ZTest::test_ZImage() {
    _INFO("Test ZImage start");
    cv::Mat src = cv::imread("../../../assets/images/lyf.jpg");
    ZImage srcImg(src.data, src.cols, src.rows, ZImgFormat::F_BGR, false);

    ZImgFormat allFormats[] = {
        ZImgFormat::F_RGBA,
        ZImgFormat::F_BGRA,
        ZImgFormat::F_RGB,
        ZImgFormat::F_BGR,
        ZImgFormat::F_YUV_NV21,
        ZImgFormat::F_GRAY
    };

    for (ZImgFormat srcF: allFormats) {
        _INFO("test source format: %s", ZImage::formatStr(srcF));
        ZImage s = srcImg.convertToImg(srcF);
        for (ZImgFormat dstF: allFormats) {
            _INFO("test destination format: %s", ZImage::formatStr(dstF));
            ZImage d = s.convertToImg(dstF);

            ZImage resized = d.resizeToImg(d.width()/3, d.height()/3);
            cv::Mat bgr = resized.convertToMat(F_BGR);
            cv::imwrite("test_zimage_" + ZImage::formatStr(srcF) + "_2_" + ZImage::formatStr(dstF) + ".jpg", bgr);
        }
    }
    _INFO("Test ZImage end");
}
