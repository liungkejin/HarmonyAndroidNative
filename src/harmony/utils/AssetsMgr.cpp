//
// Created on 2024/7/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "AssetsMgr.h"

NAMESPACE_DEFAULT

bool AssetsMgr::isDirectory(const char *path) {
    return OH_ResourceManager_IsRawDir(m_manager, path);
}

AssetsDir AssetsMgr::openDir(const char *path) {
    return {path, OH_ResourceManager_OpenRawDir(m_manager, path)};
}

AssetsFile AssetsMgr::openFile(const char *path) {
    return {path, OH_ResourceManager_OpenRawFile(m_manager, path)};
}

AssetsFile64 AssetsMgr::openFile64(const char *path) {
    return {path, OH_ResourceManager_OpenRawFile64(m_manager, path)};
}

NAMESPACE_END