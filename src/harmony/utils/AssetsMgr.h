//
// Created on 2024/7/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "ZNative.h"
#include "common/Object.h"
#include <cstdint>
#include <rawfile/raw_file_manager.h>
#include <rawfile/raw_dir.h>
#include <rawfile/raw_file.h>

NAMESPACE_DEFAULT

class AssetsMgr;

class AssetsDir : Object {
private:
    friend class AssetsMgr;

    AssetsDir(const char *path, RawDir *dir) : m_path(path), m_dir(dir){};

public:
    AssetsDir(const AssetsDir &other) : m_path(other.m_path), m_dir(other.m_dir), Object(other) {}
    ~AssetsDir() {
        if (m_dir && no_reference()) {
            OH_ResourceManager_CloseRawDir(m_dir);
            m_dir = nullptr;
        }
    };

    bool exists() const { return m_dir != nullptr; }

    const char *getFileNameAt(int index) { return m_dir ? OH_ResourceManager_GetRawFileName(m_dir, index) : nullptr; }

    int fileCount() { return m_dir ? OH_ResourceManager_GetRawFileCount(m_dir) : 0; }

private:
    std::string m_path;
    RawDir *m_dir;
};

class AssetsFile : Object {
private:
    friend class AssetsMgr;
    AssetsFile(const char *path, RawFile *file) : m_path(path), m_file(file){};

public:
    AssetsFile(const AssetsFile &other) : m_path(other.m_path), m_file(other.m_file), Object(other) {}
    ~AssetsFile() {
        if (m_file && no_reference()) {
            OH_ResourceManager_CloseRawFile(m_file);
            m_file = nullptr;
        }
    }

    bool exists() const { return m_file != nullptr; }

    long size() { return m_file ? OH_ResourceManager_GetRawFileSize(m_file) : 0; }

    long remainingLength() { return m_file ? OH_ResourceManager_GetRawFileRemainingLength(m_file) : 0; }

    /**
     * @param offset
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return
     */
    int seek(long offset, int whence = 0) {
        return m_file ? OH_ResourceManager_SeekRawFile(m_file, offset, whence) : 0;
    }

    long getOffset() { return m_file ? OH_ResourceManager_GetRawFileOffset(m_file) : 0; }

    int read(void *buffer, size_t length) {
        return m_file ? OH_ResourceManager_ReadRawFile(m_file, buffer, length) : 0;
    }

private:
    std::string m_path;
    RawFile *m_file;
};

class AssetsFile64 : Object {
private:
    friend class AssetsMgr;
    AssetsFile64(const char *path, RawFile64 *file) : m_path(path), m_file(file){};

public:
    AssetsFile64(const AssetsFile64 &other) : m_path(other.m_path), m_file(other.m_file), Object(other) {}
    ~AssetsFile64() {
        if (m_file && no_reference()) {
            OH_ResourceManager_CloseRawFile64(m_file);
            m_file = nullptr;
        }
    }

    int64_t size() { return m_file ? OH_ResourceManager_GetRawFileSize64(m_file) : 0; }

    int64_t remainingLength() { return m_file ? OH_ResourceManager_GetRawFileRemainingLength64(m_file) : 0; }

    /**
     * @param offset
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return
     */
    int64_t seek(long offset, int whence = 0) {
        return m_file ? OH_ResourceManager_SeekRawFile64(m_file, offset, whence) : 0;
    }

    int64_t getOffset() { return m_file ? OH_ResourceManager_GetRawFileOffset64(m_file) : 0; }

    int64_t read(void *buffer, int64_t length) {
        return m_file ? OH_ResourceManager_ReadRawFile64(m_file, buffer, length) : 0;
    }

private:
    std::string m_path;
    RawFile64 *m_file;
};

class AssetsMgr : Object {
public:
    explicit AssetsMgr(NativeResourceManager *mgr) : m_manager(mgr) {}
    AssetsMgr(const AssetsMgr &other) : m_manager(other.m_manager), Object(other) {}

    ~AssetsMgr() {
        if (no_reference() && m_manager) {
            OH_ResourceManager_ReleaseNativeResourceManager(m_manager);
            m_manager = nullptr;
        }
    }

public:

    bool isDirectory(const char *path);

    AssetsDir openDir(const char *path);

    AssetsFile openFile(const char *path);

    AssetsFile64 openFile64(const char *path);

private:
    NativeResourceManager *m_manager;
};

NAMESPACE_END
