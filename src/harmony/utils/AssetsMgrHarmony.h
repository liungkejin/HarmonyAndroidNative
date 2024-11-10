//
// Created on 2024/7/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "common/utils/AssetsMgr.h"
#include <rawfile/raw_file_manager.h>
#include <rawfile/raw_dir.h>
#include <rawfile/raw_file.h>

NAMESPACE_DEFAULT

class AssetDirHarmony : AssetDir {
public:
    AssetDirHarmony(const char *path, RawDir *dir) : AssetDir(path, dir) {};
    ~AssetDirHarmony() override {
        if (m_dir && no_reference()) {
            OH_ResourceManager_CloseRawDir(dir());
            m_dir = nullptr;
        }
    };
    
public:
    int fileCount() override { return OH_ResourceManager_GetRawFileCount(dir()); }

    const std::vector<std::string> &listFiles() override {
        if (m_all_files_loaded) {
            return m_all_files;
        }
        m_all_files_loaded = true;
        int count = fileCount();
        RawDir *d = dir();
        for (int i = 0; i < count; i++) {
            const char *name = OH_ResourceManager_GetRawFileName(d, i);
            if (name) {
                m_all_files.emplace_back(name);
            }
        }
        return m_all_files;
    }

private:
    inline RawDir * dir() { return (RawDir *)m_dir; }
    
private:
    std::vector<std::string> m_all_files;
    bool m_all_files_loaded = false;
};


class AssetFileHarmony : AssetFile {
public:
    AssetFileHarmony(const char *path, RawFile *file,
                  AssetsReadMode mode = AssetsReadMode::STREAMING) : AssetFile(path, file, mode){};

    ~AssetFileHarmony() override {
        if (m_file && no_reference()) {
            OH_ResourceManager_CloseRawFile(file());
            m_file = nullptr;
        }
    }
public:

    /**
     * 文件总的大小
     */
    int64_t size() override {
        return OH_ResourceManager_GetRawFileSize(file());
    }

    /**
     * 剩余未读取的长度
     */
    int64_t remainingLength() override {
        return OH_ResourceManager_GetRawFileRemainingLength(file());
    }

    /**
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return
     */
    int64_t seek(long offset, AssetsSeekMode whence) override {
        return OH_ResourceManager_SeekRawFile(file(), offset, whence);
    }

    /**
     * @param buffer
     * @param length
     * @return 读取到的长度，可能不等于 length, 结束返回 0
     */
    int64_t read(void *buffer, int64_t length) override {
        return OH_ResourceManager_ReadRawFile(file(), buffer, length);
    }

    /**
     * 获取文件描述符
     * @param outStart 文件描述符的起始位置
     * @param outLength 文件的长度
     * @return 文件描述符
     */
    int openFileDescriptor(int64_t *outStart, int64_t *outLength) override {
        RawFileDescriptor descriptor;
        if (OH_ResourceManager_GetRawFileDescriptor(file(), descriptor)) {
            *outStart = descriptor.start;
            *outLength = descriptor.length;
            return descriptor.fd;
        }
        return -1;
    }

private:
    inline RawFile *file() { return (RawFile *)m_file; };
};

class AssetFile64Harmony : AssetFile64 {
public:
    AssetFile64Harmony(const char *path, RawFile64 *file, 
                    AssetsReadMode mode = AssetsReadMode::STREAMING) : AssetFile64(path, file, mode){};

    ~AssetFile64Harmony() override {
        if (m_file && no_reference()) {
            OH_ResourceManager_CloseRawFile64(file());
            m_file = nullptr;
        }
    }
public:

    /**
     * 文件总的大小
     */
    int64_t size() override {
        return OH_ResourceManager_GetRawFileSize64(file());
    }

    /**
     * 剩余未读取的长度
     */
    int64_t remainingLength() override {
        return OH_ResourceManager_GetRawFileRemainingLength64(file());
    }

    /**
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return
     */
    int64_t seek(long offset, AssetsSeekMode whence) override {
        return OH_ResourceManager_SeekRawFile64(file(), offset, whence);
    }

    /**
     * @param buffer
     * @param length
     * @return 读取到的长度，可能不等于 length, 结束返回 0
     */
    int64_t read(void *buffer, int64_t length) override {
        return OH_ResourceManager_ReadRawFile64(file(), buffer, length);
    }

    /**
     * 获取文件描述符
     * @param outStart 文件描述符的起始位置
     * @param outLength 文件的长度
     * @return 文件描述符
     */
    int openFileDescriptor(int64_t *outStart, int64_t *outLength) override {
        RawFileDescriptor64 descriptor;
        if (OH_ResourceManager_GetRawFileDescriptor64(file(), &descriptor)) {
            *outStart = descriptor.start;
            *outLength = descriptor.length;
            return descriptor.fd;
        }
        return -1;
    }

private:
    inline RawFile64 *file() { return (RawFile64 *)m_file; };
};

class AssetsMgrHarmony : AssetsMgr {
public:
    explicit AssetsMgrHarmony(NativeResourceManager *mgr) : AssetsMgr(mgr) {}

    ~AssetsMgrHarmony() override {
        if (no_reference() && m_manager) {
            OH_ResourceManager_ReleaseNativeResourceManager(manager());
            m_manager = nullptr;
        }
    }

public:

    bool isDirectory(const char *path) override {
        return OH_ResourceManager_IsRawDir(manager(), path);
    }

    std::shared_ptr<AssetDir> openDir(const char *path) override {
        RawDir *dir = OH_ResourceManager_OpenRawDir(manager(), path);
        if (dir == nullptr) {
            return nullptr;
        }
        auto* ptr = (AssetDir*) new AssetDirHarmony(path, dir);
        return std::shared_ptr<AssetDir>(ptr);
    }

    std::shared_ptr<AssetFile> openFile(const char *path, AssetsReadMode mode) override {
        RawFile *file = OH_ResourceManager_OpenRawFile(manager(), path);
        if (file == nullptr) {
            return nullptr;
        }
        auto* ptr = (AssetFile*) new AssetFileHarmony(path, file, mode);
        return std::shared_ptr<AssetFile>(ptr);
    }

    std::shared_ptr<AssetFile64> openFile64(const char *path, AssetsReadMode mode) override {
        RawFile64 *file = OH_ResourceManager_OpenRawFile64(manager(), path);
        if (file == nullptr) {
            return nullptr;
        }
        auto* ptr = (AssetFile*) new AssetFile64Harmony(path, file, mode);
        return std::shared_ptr<AssetFile64>(ptr);
    }

private:
    inline NativeResourceManager *manager() { return (NativeResourceManager *)m_manager; }
};


NAMESPACE_END