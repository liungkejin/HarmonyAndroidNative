//
// Created on 2024/7/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "Namespace.h"
#include "common/utils/Array.h"
#include "common/utils/RawData.h"
#include "common/Object.h"
#include <cstdint>
#include <vector>

NAMESPACE_DEFAULT

// 仅支持 Android
enum AssetsReadMode {
    UNKNOWN = 0,
    RANDOM = 1,
    STREAMING = 2,
    BUFFER = 3,
};

enum AssetsSeekMode {
    MODE_SET = 0,
    MODE_CUR = 1,
    MODE_END = 2,
};

class AssetFile : protected Object {
public:
    AssetFile(const char *path, void *file, AssetsReadMode mode = STREAMING) : m_path(path), m_file(file),
                                                                               m_mode(mode) {};

    AssetFile(const AssetFile &other) : m_path(other.m_path), m_file(other.m_file), m_mode(other.m_mode),
                                        m_data(other.m_data), m_data_loaded(other.m_data_loaded), Object(other) {}

    virtual ~AssetFile() {}

public:

    inline AssetsReadMode mode() const { return m_mode; }

    /**
     * 文件总的大小
     */
    virtual int64_t size() = 0;

    /**
     * 剩余未读取的长度
     */
    virtual int64_t remainingLength() = 0;

    /**
     * @param whence    0: SEEK_SET
     *                  1: SEEK_CUR
     *                  2: SEEK_END
     * @return
     */
    virtual int64_t seek(long offset, AssetsSeekMode whence) = 0;

    /**
     * @param buffer
     * @param length
     * @return 读取到的长度，可能不等于 length, 结束返回 0
     */
    virtual int64_t read(void *buffer, int64_t length) = 0;

    /**
     * 获取文件描述符
     * @param outStart 文件描述符的起始位置
     * @param outLength 文件的长度
     * @return 文件描述符
     */
    virtual int openFileDescriptor(int64_t *outStart, int64_t *outLength) = 0;

public:

    RawData readAll() {
        int64_t length = size();
        if (length < 1) {
            return {};
        }
        RawData data(length);
        seek(0, AssetsSeekMode::MODE_SET);
        int64_t readLength = read(data.data(), length);
        _ERROR_RETURN_IF(readLength != length, RawData(), "readAll failed, read length(%d) != file length(%d)", readLength, length);
        return data;
    }

    /**
     * 读取所有数据
     * @return buffer
     */
    const uint8_t *readAll(int64_t &outLength) {
        if (m_data_loaded) {
            outLength = (int64_t) m_data.capacity();
            return m_data.bytes();
        }
        seek(0, AssetsSeekMode::MODE_SET);
        int64_t size = this->size();
        if (size < 1) {
            outLength = 0;
            return nullptr;
        }
        auto *buffer = m_data.obtain<uint8_t>(size, true);
        outLength = read(buffer, size);
        m_data_loaded = outLength > 0;
        return buffer;
    }

    std::string readText() {
        int64_t length;
        const uint8_t *data = readAll(length);
        if (data == nullptr) {
            return "";
        }
        return {(const char *) data, (size_t) length};
    }

protected:
    AssetsReadMode m_mode;
    std::string m_path;
    void *m_file;

private:
    Array m_data;
    bool m_data_loaded = false;
};

typedef AssetFile AssetFile64;


class AssetDir : protected Object {
public:
    AssetDir(const char *path, void *dir) : m_path(path), m_dir(dir) {};

    AssetDir(const AssetDir &other) : m_path(other.m_path), m_dir(other.m_dir), Object(other) {}

    virtual ~AssetDir() {}

    /**
     * @return 子文件数量
     */
    virtual int fileCount() = 0;

    /**
     * 列出所有子文件
     */
    virtual const std::vector<std::string> &listFiles() = 0;

protected:
    std::string m_path;
    void *m_dir;
};


class AssetsMgr : protected Object {
public:
    explicit AssetsMgr(void *mgr) : m_manager(mgr) {}

    AssetsMgr(const AssetsMgr &other) : m_manager(other.m_manager), Object(other) {}

    virtual ~AssetsMgr() {}

public:
    bool isFile(const char *path) {
        return openFile(path) != nullptr;
    }

    virtual bool isDirectory(const char *path) {
        return openDir(path) != nullptr;
    }

    RawData readAll(const char *path) {
        auto file = openFile(path);
        if (file == nullptr) {
            return {};
        }
        return file->readAll();
    }

    /**
     * 打开目录
     * @param path
     * @return 没有文件夹打开失败返回 nullptr
     */
    virtual std::shared_ptr<AssetDir> openDir(const char *path) = 0;

    std::shared_ptr<AssetFile> openFile(const char *path) {
        return openFile(path, AssetsReadMode::STREAMING);
    }

    /**
     * 打开文件
     * @param path
     * @return 没有文件打开失败返回 nullptr
     */
    virtual std::shared_ptr<AssetFile> openFile(const char *path, AssetsReadMode mode) = 0;

    std::shared_ptr<AssetFile64> openFile64(const char *path) {
        return openFile64(path, AssetsReadMode::STREAMING);
    }

    /**
     * 打开大文件
     * @param path
     * @return 没有文件打开失败返回 nullptr
     */
    virtual std::shared_ptr<AssetFile64> openFile64(const char *path, AssetsReadMode mode) = 0;

protected:
    void *m_manager;
};

NAMESPACE_END
