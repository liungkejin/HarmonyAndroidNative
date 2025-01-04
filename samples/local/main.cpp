#include "gl/GLRenderer.h"
#include <common/AppContext.h>
#include <common/utils/FileUtils.h>
#include <common/media/img/ZImage.h>
#include <opencv2/imgcodecs.hpp>

using namespace znative;

// Main code
int main(int argc, char** argv)
{
    {
        // test ZImage
        cv::Mat src = cv::imread("../../../assets/images/lyf.jpg");
        ZImage img(src.data, src.cols, src.rows, F_BGR, false);
        cv::Mat nv21 = img.convertTo(F_YUV_NV21);
        cv::imwrite("nv21.jpg", nv21);
        ZImage nv21Img(nv21.data, nv21.cols, nv21.rows, F_YUV_NV21, false);
        cv::Mat resizeNV21 = nv21Img.resize(nv21Img.width()/2, nv21Img.height()/2);
        // ZImage resizeNV21Img(resizeNV21.data, resizeNV21.cols, resizeNV21.rows, F_YUV_NV21, false);
        // cv::Mat resizeBGR = resizeNV21Img.convertTo(ZImgFormat::F_BGR);
        // cv::imwrite("test_nv21.jpg", resizeBGR);
        return 0;
    }
    std::string filesDir = "../../../assets";
    std::string cacheDir = "./caches";
    FileUtils::mkDir(cacheDir.c_str());
    znative::AppContext::initialize(filesDir, cacheDir);
    GLRenderer::run();
    return 0;
}
