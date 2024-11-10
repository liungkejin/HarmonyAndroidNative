//
// Created by LiangKeJin on 2024/11/9.
//

#pragma once

#include "ZNative.h"
#include "utils/NapiUtils.h"
#include "utils/AssetsMgr.h"

NAMESPACE_DEFAULT

class AppContext {
public:
    static void initialize(napi_env &env, napi_value& jsResMgr);

    static AssetsMgr& assetsMgr();

    static std::string appId();

    static std::string identifier();

    static std::string bundleName();

    static std::string fingerprint();
};

NAMESPACE_END

