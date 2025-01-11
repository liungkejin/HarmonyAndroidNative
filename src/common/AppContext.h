//
// Created by LiangKeJin on 2024/11/9.
//

#pragma once

#include "Namespace.h"
#include "common/utils/AssetsMgr.h"
#ifdef __ANDROID__
#include <jni.h>
#elif defined(__HARMONYOS__)
#include "harmony/utils/NapiUtils.h"
#include <rawfile/raw_file_manager.h>
#endif

NAMESPACE_DEFAULT

class AppContext {
public:
#ifdef __ANDROID__
    static void initialize(JNIEnv *env, jobject context);

    static int sdkInt();

    static std::string externalFilesDir();

    /**
     * Assets manager
     * @return 不为空
     */
    static AssetsMgr* assetsManager();

#elif defined(__HARMONYOS__)
    static void initialize(napi_env env, napi_value jsResMgr,
                           std::string& dirFiles, std::string& dirCache);

    static NativeResourceManager *rawAssetsManager();

    /**
     * Assets manager
     * @return 不为空
     */
    static AssetsMgr* assetsManager();

    static std::string appId();

    static std::string identifier();

    static std::string fingerprint();
#else
    static void initialize(std::string& dirFiles, std::string& dirCache);
#endif

    // android's package name
    static std::string bundleName();

    static std::string filesDir();

    static std::string cacheDir();
};

NAMESPACE_END