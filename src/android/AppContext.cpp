//
// Created by LiangKeJin on 2024/11/9.
//

#include <android/asset_manager_jni.h>
#include "common/AppContext.h"
#include "android/utils/JNIUtils.h"
#include "android/utils/AssetsMgrAndroid.h"

NAMESPACE_DEFAULT

struct AndroidContext {
    AssetsMgrAndroid *asset_mgr = nullptr;
    jobject asset_mgr_global_obj = nullptr;

    std::string files_dir;
    std::string cache_dir;
    std::string external_files_dir;

    std::string package_name;

    int sdk_int = 0;
};
static AndroidContext g_context;
static bool initialized() {
    return g_context.asset_mgr != nullptr;
}

void AppContext::initialize(JNIEnv *env, jobject context) {
    _WARN_RETURN_IF(initialized(), void(), "AppContext already initialized")

    jclass contextClass = env->GetObjectClass(context);
    // 获取文件目录路径 getFilesDir
    {
        jmethodID getFilesDirMethod = env->GetMethodID(contextClass, "getFilesDir",
                                                       "()Ljava/io/File;");
        jobject filesDir = env->CallObjectMethod(context, getFilesDirMethod);
        jclass fileClass = env->GetObjectClass(filesDir);
        jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath",
                                                           "()Ljava/lang/String;");
        auto filesDirPath = (jstring) env->CallObjectMethod(filesDir, getAbsolutePathMethod);
        g_context.files_dir = JNIUtils::jstring2stdString(env, filesDirPath);
    }

    // getCachesDir
    {
        jmethodID getCacheDirMethod = env->GetMethodID(contextClass, "getCacheDir",
                                                       "()Ljava/io/File;");
        jobject cacheDir = env->CallObjectMethod(context, getCacheDirMethod);
        jclass fileClass = env->GetObjectClass(cacheDir);
        jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath",
                                                           "()Ljava/lang/String;");
        auto cacheDirPath = (jstring) env->CallObjectMethod(cacheDir, getAbsolutePathMethod);
        g_context.cache_dir = JNIUtils::jstring2stdString(env, cacheDirPath);
    }

    // external files dir
    {
        jmethodID getExternalFilesDirMethod = env->GetMethodID(contextClass, "getExternalFilesDir",
                                                               "(Ljava/lang/String;)Ljava/io/File;");
        jobject externalFilesDir = env->CallObjectMethod(context, getExternalFilesDirMethod, nullptr);
        jclass fileClass = env->GetObjectClass(externalFilesDir);
        jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath",
                                                           "()Ljava/lang/String;");
        auto externalFilesDirPath = (jstring) env->CallObjectMethod(externalFilesDir, getAbsolutePathMethod);
        g_context.external_files_dir = JNIUtils::jstring2stdString(env, externalFilesDirPath);
    }

    // package name
    {
        jmethodID getPackageNameMethod = env->GetMethodID(contextClass, "getPackageName",
                                                          "()Ljava/lang/String;");
        auto packageName = (jstring) env->CallObjectMethod(context, getPackageNameMethod);
        g_context.package_name = JNIUtils::jstring2stdString(env, packageName);
    }

    // 获取 AssetManager
    {
        jmethodID getAssetsMethod = env->GetMethodID(contextClass, "getAssets",
                                                     "()Landroid/content/res/AssetManager;");
        jobject localAssetMgr = env->CallObjectMethod(context, getAssetsMethod);
        g_context.asset_mgr_global_obj = env->NewGlobalRef(localAssetMgr);
        AAssetManager *mgr = AAssetManager_fromJava(env, g_context.asset_mgr_global_obj);
        _FATAL_IF(mgr == nullptr, "AAssetManager_fromJava failed")
        g_context.asset_mgr = new AssetsMgrAndroid(mgr);
    }
}

AssetsMgr* AppContext::assetsManager() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return (AssetsMgr *)g_context.asset_mgr;
}

int AppContext::sdkInt() {
    if (g_context.sdk_int != 0) {
        return g_context.sdk_int;
    }
    char sdk[128] = "0";
    // 获取版本号方法
    __system_property_get("ro.build.version.sdk", sdk);
    //将版本号转为 int 值
    g_context.sdk_int = (int) strtol(sdk, nullptr, 10);
    return g_context.sdk_int;
}

std::string AppContext::bundleName() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return g_context.package_name;
}

std::string AppContext::filesDir() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return g_context.files_dir;
}

std::string AppContext::cacheDir() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return g_context.cache_dir;
}

std::string AppContext::externalFilesDir() {
    _FATAL_IF(!initialized(), "AppContext not initialized")
    return g_context.external_files_dir;
}

NAMESPACE_END