//
// Created by LiangKeJin on 2024/11/9.
//

#include "AppContext.h"
#include <bundle/native_interface_bundle.h>
#include <rawfile/raw_file_manager.h>

NAMESPACE_DEFAULT

struct Context {
    napi_ref native_resource_mgr = nullptr;
    AssetsMgr* assets_mgr = nullptr;
};
static Context g_context;
static bool initialized() {
    return g_context.native_resource_mgr != nullptr;
}

void AppContext::initialize(napi_env &env, napi_value& jsResMgr) {
    _WARN_RETURN_IF(initialized(), void(), "AppContext already initialized")

    NapiEnv napiEnv(env);
    g_context.native_resource_mgr = napiEnv.createRef(jsResMgr);

    NativeResourceManager *resMgr = OH_ResourceManager_InitNativeResourceManager(
            env, napiEnv.getRefValue(g_context.native_resource_mgr));
    _FATAL_IF(resMgr == nullptr, "OH_ResourceManager_InitNativeResourceManager failed")
    g_context.assets_mgr = new AssetsMgr(resMgr);
}

AssetsMgr& AppContext::assetsMgr() {
    _FATAL_IF(!initialized(), "AppContext::assetsMgr() called before initialized")
    return *g_context.assets_mgr;
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

NAMESPACE_END