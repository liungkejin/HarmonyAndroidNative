//
// Created by LiangKeJin on 2024/11/9.
//

#pragma once

#include "Namespace.h"
#include "common/utils/AssetsMgr.h"
#ifdef __ANDROID__
#include <jni.h>
#elif __HARMONY_OS__
#include "harmony/utils/NapiUtils.h"
#endif

NAMESPACE_DEFAULT

class AppContext {
public:
#ifdef __ANDROID__
    static void initialize(JNIEnv *env, jobject context);

    static int sdkInt();

    static std::string externalFilesDir();

#elif __HARMONY_OS__
    static void initialize(napi_env &env, napi_value& jsResMgr,
                           std::string& dirFiles, std::string& dirCache);

    static std::string appId();

    static std::string identifier();

    static std::string fingerprint();
#endif

    /**
     * Assets manager
     * @return 不为空
     */
    static AssetsMgr* assetsManager();

    // android's package name
    static std::string bundleName();

    static std::string filesDir();

    static std::string cacheDir();
};

NAMESPACE_END