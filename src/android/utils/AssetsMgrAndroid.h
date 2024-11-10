//
// Created by LiangKeJin on 2024/11/9.
//

#pragma once

#include "ZNative.h"
#include "common/Object.h"
#include "common/utils/Array.h"
#include "common/utils/AssetsMgr.h"
#include <android/asset_manager.h>
#include <vector>

NAMESPACE_DEFAULT

class AssetFileAndroid : AssetFile {
public:
    AssetFileAndroid(const char *path, AAsset *file, AssetsReadMode mode = STREAMING) : AssetFile(path, file, mode) {}
    ~AssetFileAndroid() override {
        if (no_reference() && m_file) {
            AAsset_close((AAsset *) m_file);
            m_file = nullptr;
        }
    }

    int64_t size() override { return AAsset_getLength(asset()); }

    /**
     * 读取数据
     * @return 读取到的长度，可能不等于 length, 结束返回 0
     */
    int64_t read(void *buffer, int64_t length) override {
        return AAsset_read(asset(), buffer, length);
    }

    /**
     * 读取后的剩余长度
     * @return 剩余未读取的长度
     */
    int64_t remainingLength() override { return AAsset_getRemainingLength(asset()); }

    /**
     * @param offset
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return -1 表示错误
     */
    int64_t seek(long offset, AssetsSeekMode whence) override {
        return AAsset_seek(asset(), offset, whence);
    }

    int openFileDescriptor(int64_t *outStart, int64_t *outLength) override {
        return AAsset_openFileDescriptor(asset(), (off_t *) outStart, (off_t *) outLength);
    }

private:
    inline AAsset *asset() { return (AAsset *) m_file; }
};

class AssetFile64Android : AssetFile64 {
public:
    AssetFile64Android(const char *path, AAsset *file, AssetsReadMode mode = STREAMING) : AssetFile64(path, file, mode) {}
    ~AssetFile64Android() override {
        if (no_reference() && m_file) {
            AAsset_close((AAsset *) m_file);
            m_file = nullptr;
        }
    }

    int64_t size() override { return AAsset_getLength64(asset()); }

    /**
     * 读取数据
     * @return 读取到的长度，可能不等于 length, 结束返回 0
     */
    int64_t read(void *buffer, int64_t length) override {
        return AAsset_read(asset(), buffer, length);
    }

    /**
     * 读取后的剩余长度
     * @return 剩余未读取的长度
     */
    int64_t remainingLength() override { return AAsset_getRemainingLength64(asset()); }

    /**
     * @param offset
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return -1 表示错误
     */
    int64_t seek(long offset, AssetsSeekMode whence) override {
        return AAsset_seek64(asset(), offset, whence);
    }

    int openFileDescriptor(int64_t *outStart, int64_t *outLength) override {
        return AAsset_openFileDescriptor64(asset(), (off_t *) outStart, (off_t *) outLength);
    }

private:
    inline AAsset *asset() { return (AAsset *) m_file; }
};

class AssetDirAndroid : protected AssetDir {
public:
    AssetDirAndroid(const char *path, AAssetDir *dir) : AssetDir(path, dir) {}
    ~AssetDirAndroid() override {
        if (m_dir && no_reference()) {
            AAssetDir_close(dir());
            m_dir = nullptr;
        }
    };

public:
    int fileCount() override {
        return (int) listFiles().size();
    }

    /**
     * 列出所有子文件
     */
    const std::vector<std::string>& listFiles() override {
        if (m_all_files_loaded) {
            return m_all_files;
        }
        m_all_files_loaded = true;
        AAssetDir_rewind(dir());
        const char *name;
        while ((name = AAssetDir_getNextFileName(dir()))) {
            m_all_files.emplace_back(name);
        }
        return m_all_files;
    }

private:
    AAssetDir * dir() { return (AAssetDir *)m_dir; }

private:
    std::vector<std::string> m_all_files;
    bool m_all_files_loaded = false;
};


class AssetsMgrAndroid : protected AssetsMgr {
public:
    explicit AssetsMgrAndroid(AAssetManager *mgr) : AssetsMgr(mgr) {}

    ~AssetsMgrAndroid() override {
        //
    }

public:

    std::shared_ptr<AssetDir> openDir(const char *path) override {
        AAssetDir *dir = AAssetManager_openDir(manager(), path);
        if (dir == nullptr) {
            return nullptr;
        }
        auto* ptr = (AssetDir*) new AssetDirAndroid(path, dir);
        return std::shared_ptr<AssetDir>(ptr);
    }

    std::shared_ptr<AssetFile> openFile(const char *path, AssetsReadMode mode) override {
        AAsset *file = AAssetManager_open(manager(), path, mode);
        if (file == nullptr) {
            return nullptr;
        }
        auto* ptr = (AssetFile*) new AssetFileAndroid(path, file, mode);
        return std::shared_ptr<AssetFile>(ptr);
    }

    std::shared_ptr<AssetFile64> openFile64(const char *path, AssetsReadMode mode) override {
        AAsset *file = AAssetManager_open(manager(), path, mode);
        if (file == nullptr) {
            return nullptr;
        }
        auto* ptr = (AssetFile*) new AssetFile64Android(path, file, mode);
        return std::shared_ptr<AssetFile64>(ptr);
    }
private:
    AAssetManager *manager() { return (AAssetManager *) m_manager; }
};

NAMESPACE_END
