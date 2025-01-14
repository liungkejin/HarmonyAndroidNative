
#include <common/AppContext.h>
#include <common/utils/FileUtils.h>
#include "MainApp.h"

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
#ifdef LOCAL_ASSETS_PATH
    std::string filesDir = LOCAL_ASSETS_PATH;
#else
    std::string filesDir = "../../../assets";
#endif
    std::string cacheDir = "./caches";
    FileUtils::mkDir(cacheDir.c_str());
    znative::AppContext::initialize(filesDir, cacheDir);
    MainApp::run();
    return 0;
}
