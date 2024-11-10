//
// Created by LiangKeJin on 2024/11/9.
//

#pragma once

#include "ZNative.h"
#include "common/Object.h"
#include <android/asset_manager.h>
#include <vector>

NAMESPACE_DEFAULT

class AssetsMgr;
class AssetsDir;

class AssetsFile : Object {
private:
    friend class AssetsMgr;
    friend class AssetsDir;

    AssetsFile(const char *path, AAsset *file, int mode) : m_path(path), m_file(file), m_open_mode(mode) {};

public:
    AssetsFile(const AssetsFile &other) : m_path(other.m_path), m_file(other.m_file), m_open_mode(other.m_open_mode), Object(other) {}

    ~AssetsFile() {
        if (m_file && no_reference()) {
            AAsset_close(m_file);
            m_file = nullptr;
        }
    }

    inline bool exists() const { return m_file != nullptr; }

    /**
     * 0 AAASSET_MODE_UNKNOWN
     * 1 AAASSET_MODE_RANDOM
     * 2 AAASSET_MODE_STREAMING
     * 3 AAASSET_MODE_BUFFER
     */
    inline int mode() const { return m_open_mode; }

    /**
     * 读取全部内容, mode 必须是 AAASSET_MODE_BUFFER
     */
    const void *readAll() {
        return m_file ? AAsset_getBuffer(m_file) : nullptr;
    }

    bool isAllocated() {
        return m_file != nullptr && AAsset_isAllocated(m_file);
    }

    int64_t size() { return m_file ? AAsset_getLength64(m_file) : 0; }

    /**
     * 读取数据
     * @return 读取到的长度，可能不等于 length, 结束返回 0
     */
    int read(void *buffer, size_t length) {
        return m_file ? AAsset_read(m_file, buffer, length) : 0;
    }

    /**
     * 读取后的剩余长度
     * @return 剩余未读取的长度
     */
    int64_t remainingLength() { return m_file ? AAsset_getRemainingLength64(m_file) : 0; }

    /**
     * @param offset
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return -1 表示错误
     */
    int64_t seek(long offset, int whence = 0) {
        return m_file ? AAsset_seek64(m_file, offset, whence) : 0;
    }

private:
    std::string m_path;
    AAsset *m_file;
    int m_open_mode;
};

class AssetsDir : Object {
private:
    friend class AssetsMgr;

    AssetsDir(AAssetManager *mgr, const char *path, AAssetDir *dir) : m_mgr(mgr), m_path(path), m_dir(dir){};

public:
    AssetsDir(const AssetsDir &other) :  m_mgr(other.m_mgr), m_path(other.m_path), m_dir(other.m_dir), Object(other) {}
    ~AssetsDir() {
        if (m_dir && no_reference()) {
            AAssetDir_close(m_dir);
            m_dir = nullptr;
        }
    };

    bool exists() const { return m_dir != nullptr; }

    /**
     * @return 最后一个返回 nullptr
     */
    const char * nextFileName() {
        return m_dir ? AAssetDir_getNextFileName(m_dir) : nullptr;
    }

    /**
     * 列出所有子文件
     */
    std::vector<std::string> listFiles() {
        std::vector<std::string> files;
        if (m_dir) {
            AAssetDir_rewind(m_dir);
            const char *name;
            while ((name = AAssetDir_getNextFileName(m_dir))) {
                files.emplace_back(name);
            }
        }
        return files;
    }

    bool exitsFile(const char *fileName) {
        if (m_dir) {
            AAssetDir_rewind(m_dir);
            const char *name;
            while ((name = AAssetDir_getNextFileName(m_dir))) {
                if (strcmp(name, fileName) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * 重置迭代器
     */
    void rewind() {
        if (m_dir) {
            AAssetDir_rewind(m_dir);
        }
    }

    AssetsFile openFile(const char *name, int mode = AASSET_MODE_STREAMING) {
        std::string path = m_path + "/" + name;
        AAsset *file = nullptr;
        if (m_dir) {
            file = AAssetManager_open(m_mgr, path.c_str(), mode);
        }
        return {path.c_str(), file, mode};
    }

private:
    AAssetManager *m_mgr;
    std::string m_path;
    AAssetDir *m_dir;
};

class AssetsMgr : Object {
public:
    explicit AssetsMgr(AAssetManager *manager) : m_manager(manager) {}
    AssetsMgr(const AssetsMgr &other) : m_manager(other.m_manager), Object(other) {}

    AssetsDir openDir(const char *path) {
        AAssetDir *dir = AAssetManager_openDir(m_manager, path);
        return {m_manager, path, dir};
    }

    AssetsFile openFile(const char *path, int mode = AASSET_MODE_STREAMING) {
        AAsset *file = AAssetManager_open(m_manager, path, mode);
        return {path, file, mode};
    }

private:
    AAssetManager *m_manager;
};

NAMESPACE_END
