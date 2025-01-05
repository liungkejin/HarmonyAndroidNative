#include "gl/GLRenderer.h"
#include <common/AppContext.h>
#include <common/utils/FileUtils.h>
#include "test/ZTest.h"

using namespace znative;

// Main code
int main(int argc, char** argv)
{
    std::string filesDir = "../../../assets";
    std::string cacheDir = "./caches";
    FileUtils::mkDir(cacheDir.c_str());
    znative::AppContext::initialize(filesDir, cacheDir);
    GLRenderer::run();
    return 0;
}
