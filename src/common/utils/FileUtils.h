//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/Object.h"
#include "common/utils/RawData.h"
#include <cstddef>
#include <cstdint>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FilePath.h>

NAMESPACE_DEFAULT

/// deprecated , 使用 cppfs
class FileUtils {
public:
    // 文件的绝对路径
    static std::string fullPath(const char *path) {
        cppfs::FilePath p(path);
        return p.fullPath();
    }

    // 带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx.txt"
    static std::string fileName(const char *path) {
        cppfs::FilePath p(path);
        return p.fileName();
    }

    // 不带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx"
    static std::string fileBaseName(const char *path) {
        cppfs::FilePath p(path);
        return p.baseName();
    }

    // 文件的扩展名 "/dir/xxx.txt" 返回 ".txt"
    static std::string fileExtension(const char *path) {
        cppfs::FilePath p(path);
        return p.extension();
    }

    // 文件的父文件夹 "/dir/xxx.txt" 返回 "/dir"
    static std::string fileParentDir(const char *path) {
        cppfs::FilePath p(path);
        return p.directoryPath();
    }

    static size_t write(const char *filepath, const void *data, size_t len) {
        std::FILE *file = std::fopen(filepath, "w+");
        if (file) {
            size_t written = fwrite(data, len, 1, file);
            fclose(file);

            return written;
        } else {
            _ERROR("write to file(%s) failed", filepath);
        }
        return 0;
    }
    
    static RawData read(const char *filepath) {
        std::FILE *file = fopen(filepath, "r");
        if (file == nullptr) {
            return RawData();
        }
        std::fseek(file, 0, SEEK_END);
        size_t len = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        RawData data(len);
        size_t readSize = std::fread(data.data(), 1, len, file);
        _ERROR_IF(readSize != len, "read file(%s) error, file length(%d) != read size(%d)", filepath, len, readSize);
        return data;
    }
    
    static size_t read(const char *filepath, uint8_t *out, size_t size, size_t offset = 0) {
        std::FILE *file = std::fopen(filepath, "r");
        if (file == nullptr) {
            return 0;
        }
        if (offset > 0) {
            std::fseek(file, offset, SEEK_SET);
        }
        int readCount = std::fread(out, 1, size, file);
        std::fclose(file);
        return readCount;
    }

    static size_t fileLength(const char *filepath) {
        return cppfs::fs::open(filepath).size();
    }

    static size_t fileLength(FILE *file) {
        if (file) {
            std::fseek(file, 0, SEEK_END);
            size_t length = std::ftell(file);
            std::rewind(file);

            return length;
        } else {
            _WARN("get file length from FILE* failed");
        }
        return 0;
    }

    static bool deleteFile(const char *str) {
        bool success = cppfs::fs::open(str).remove();
        _ERROR_IF(!success, "delete file(%s) failed", str);
        return success;
    }

    static bool mkDir(const char *str, uint32_t mode = 777) {
        if (isDirectory(str)) {
            return true;
        }
        bool success = cppfs::fs::open(str).createDirectory();
        _ERROR_IF(!success, "mkdir(%s) failed!", str);
        return success;
    }

    static void remove(const char *dirOrFile) {
        if (isDirectory(dirOrFile)) {
            cppfs::fs::open(dirOrFile).removeDirectoryRec();
        } else {
            deleteFile(dirOrFile);
        }
    }
    
    static bool exist(const char *path) {
        return cppfs::fs::open(path).exists();
    }

    static bool isDirectory(const char *path) {
        return cppfs::fs::open(path).isDirectory();
    }

    static std::vector<std::string> listFiles(const char *path) {
        return cppfs::fs::open(path).listFiles();
    }

    static std::vector<std::string> listFilesSort(const char *path,
        const std::function<int(const std::string& a, const std::string& b)>& comp = nullptr) {
        std::vector<std::string> files = cppfs::fs::open(path).listFiles();
        // sort files by alpha
        if (comp == nullptr) {
            std::sort(files.begin(), files.end());
        } else {
            std::sort(files.begin(), files.end(), comp);
        }
        return files;
    }
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
