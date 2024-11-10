//
// Created by LiangKeJin on 2024/11/9.
//

#pragma once

#include "ZNative.h"
#include "android/utils/AssetsMgr.h"
#include <jni.h>

NAMESPACE_DEFAULT

class AppContext {
public:
    static void initialize(JNIEnv *env, jobject context);

    static AssetsMgr& assetManager();

    static int sdkInt();

    static std::string pkgName();

    static std::string filesDir();

    static std::string cacheDir();

    static std::string externalFilesDir();
};

NAMESPACE_END