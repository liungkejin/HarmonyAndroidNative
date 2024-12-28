//
// Created by LiangKeJin on 2024/11/9.
//

#include "common/AppContext.h"

NAMESPACE_DEFAULT

struct LocalContext {
    std::string files_dir;
    std::string cache_dir;
};
static LocalContext g_context;
static bool initialized() {
    return !g_context.files_dir.empty();
}

void AppContext::initialize(std::string& dirFiles, std::string& dirCache) {
    _WARN_RETURN_IF(initialized(), void(), "AppContext already initialized")

    g_context.files_dir = dirFiles;
    g_context.cache_dir = dirCache;
}

std::string AppContext::bundleName() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return "local";
}

std::string AppContext::filesDir() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return g_context.files_dir;
}

std::string AppContext::cacheDir() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return g_context.cache_dir;
}

NAMESPACE_END