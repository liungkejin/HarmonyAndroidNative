//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "Namespace.h"
#include "common/Log.h"
#include "common/Object.h"
#include "common/utils/RawData.h"
#include <cstddef>
#include <cstdint>
#include <string>

NAMESPACE_DEFAULT

/// deprecated , 使用 cppfs
class FileUtils {
public:
    // 文件的绝对路径
    static std::string fullPath(const char *path);

    // 带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx.txt"
    static std::string fileName(const char *path);

    // 不带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx"
    static std::string fileBaseName(const char *path);

    // 文件的扩展名 "/dir/xxx.txt" 返回 ".txt"
    static std::string fileExtension(const char *path);

    // 文件的父文件夹 "/dir/xxx.txt" 返回 "/dir"
    static std::string fileParentDir(const char *path);

    static size_t write(const char *filepath, const void *data, size_t len);
    
    static RawData read(const char *filepath);
    
    static size_t read(const char *filepath, uint8_t *out, size_t size, size_t offset = 0);

    static size_t fileLength(const char *filepath);

    static size_t fileLength(FILE *file);

    static bool deleteFile(const char *str);

    static bool mkDir(const char *str);

    static void remove(const char *dirOrFile);
    
    static bool exist(const char *path);

    static bool isDirectory(const char *path);

    static std::vector<std::string> listFiles(const char *path);

    static std::vector<std::string> listFilesSort(const char *path,
        const std::function<int(const std::string& a, const std::string& b)>& comp = nullptr);
};


class File : Object {
public:
    explicit File(const char *path) : m_path(path) {}

    File(const File& o) : Object(o) , m_path(o.m_path) {}

public:
    bool exist() const {
        return FileUtils::exist(m_path.c_str());
    }

    bool isDirectory() const {
        return FileUtils::isDirectory(m_path.c_str());
    }

    size_t write(const uint8_t *data, const size_t size) const { return FileUtils::write(m_path.c_str(), data, size); }

    size_t length() const { return FileUtils::fileLength(m_path.c_str()); }

    size_t read(uint8_t *out, size_t size) const {
        return FileUtils::read(m_path.c_str(), out, size);
    }

    RawData readAll() const {
        return FileUtils::read(m_path.c_str());
    }

    const std::string &path() { return m_path; }

    bool remove() const {
        return FileUtils::deleteFile(m_path.c_str());
    }

    std::string fullPath() const {
        return FileUtils::fullPath(m_path.c_str());
    }

    std::string name() const {
        return FileUtils::fileName(m_path.c_str());
    }

    std::string baseName() const {
        return FileUtils::fileBaseName(m_path.c_str());
    }

    std::string extension() const {
        return FileUtils::fileExtension(m_path.c_str());
    }

    std::string parentDir() const {
        return FileUtils::fileParentDir(m_path.c_str());
    }
private:
    std::string m_path;
};

NAMESPACE_END
