//
// Created by LiangKeJin on 2024/11/9.
//

#include "common/AppContext.h"
#include "harmony/utils/AssetsMgrHarmony.h"
#include <bundle/native_interface_bundle.h>
#include <rawfile/raw_file_manager.h>

#include <memory>

NAMESPACE_DEFAULT

struct HarmonyContext {
    napi_ref native_resource_mgr = nullptr;
    AssetsMgrHarmony *assets_mgr = nullptr;
    std::string files_dir;
    std::string cache_dir;
};
static HarmonyContext g_context;
static bool initialized() {
    return g_context.native_resource_mgr != nullptr;
}

void AppContext::initialize(napi_env &env, napi_value& jsResMgr,
                            std::string& dirFiles, std::string& dirCache) {
    _WARN_RETURN_IF(initialized(), void(), "AppContext already initialized")

    NapiEnv napiEnv(env);
    g_context.native_resource_mgr = napiEnv.createRef(jsResMgr);

    NativeResourceManager *resMgr = OH_ResourceManager_InitNativeResourceManager(
            env, napiEnv.getRefValue(g_context.native_resource_mgr));
    _FATAL_IF(resMgr == nullptr, "OH_ResourceManager_InitNativeResourceManager failed")
    g_context.assets_mgr = new AssetsMgrHarmony(resMgr);

    g_context.files_dir = dirFiles;
    g_context.cache_dir = dirCache;
}

AssetsMgr* AppContext::assetsManager() {
    _FATAL_IF(!initialized(), "AppContext::assetsMgr() called before initialized")
    return (AssetsMgr *) g_context.assets_mgr;
}

std::string AppContext::appId() {
    return OH_NativeBundle_GetAppId();
}

std::string AppContext::bundleName() {
    OH_NativeBundle_ApplicationInfo appInfo = OH_NativeBundle_GetCurrentApplicationInfo();
    return appInfo.bundleName;
}

std::string AppContext::fingerprint() {
    OH_NativeBundle_ApplicationInfo appInfo = OH_NativeBundle_GetCurrentApplicationInfo();
    return appInfo.fingerprint;
}

std::string AppContext::identifier() {
    return OH_NativeBundle_GetAppIdentifier();
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