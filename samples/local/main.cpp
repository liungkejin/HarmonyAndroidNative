#include "gl/GLRenderer.h"
#include <common/AppContext.h>
#include <common/utils/FileUtils.h>

#ifdef WIN32
#include <stringapiset.h>
#include <Windows.h>
#endif

using namespace znative;

// Main code
int main(int argc, char** argv)
{
#ifdef WIN32
    system("chcp 65001");
    SetConsoleOutputCP(CP_UTF8);
    setbuf(stdout, nullptr);
#endif
    std::string filesDir = "../../../assets";
    std::string cacheDir = "./caches";
    FileUtils::mkDir(cacheDir.c_str());
    znative::AppContext::initialize(filesDir, cacheDir);
    GLRenderer::run();
    return 0;
}
