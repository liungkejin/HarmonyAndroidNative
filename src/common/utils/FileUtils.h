//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/Object.h"
#include "common/utils/Array.h"
#include "common/utils/Base.h"
#include "common/utils/RawData.h"
#include <cstddef>
#include <cstdint>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

NAMESPACE_DEFAULT

class Directory : Object {
public:
    explicit Directory(const char *path) : m_path(path) {}

    Directory(const Directory& o) : m_path(o.m_path), m_dir(o.m_dir), Object(o) {}

    ~Directory() {
        if (m_dir && no_reference()) {
            closedir(m_dir);
        }
    }

public:
    bool exist() {
        return dir() != nullptr;
    }

    inline std::string path() const {
        return m_path;
    }

    std::vector<std::string> listFiles() {
        std::vector<std::string> files;
        DIR *d = dir();
        if (d) {
            struct dirent *entry;
            while ((entry = readdir(d)) != nullptr) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                files.emplace_back(entry->d_name);
            }
        }
        return files;
    }

    std::vector<std::string> listFilesAlphaSort() {
        struct dirent **pdir = nullptr;
        int n = scandir(m_path.c_str(), &pdir, nullptr, alphasort);
        std::vector<std::string> files;
        for (int i = 0; i < n; ++i) {
            struct dirent *filename = pdir[i];
            if (strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0) {
                continue;
            }
            files.emplace_back(filename->d_name);
        }
        if (pdir) {
            free(pdir);
        }
        return files;
    }


private:
    DIR * dir() {
        if (m_dir == nullptr) {
            m_dir = opendir(m_path.c_str());
        }
        return m_dir;
    }

private:
    DIR *m_dir = nullptr;
    std::string m_path;
};

class FileUtils {
public:
    static size_t write(const char *filepath, void *data, size_t len) {
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
        std::FILE *file = std::fopen(filepath, "r");
        if (file) {
            std::fseek(file, 0, SEEK_END);
            size_t length = std::ftell(file);
            fclose(file);

            return length;
        } else {
            _WARN("get file length of file(%s) failed", filepath);
        }
        return 0;
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
        int err = std::remove(str);
        _WARN_IF(err, "delete file(%s) failed: %s", str, std::strerror(err));
        return err == 0;
    }

    static void mkDir(const char *str, uint32_t mode = 777) {
        int err = mkdir(str, mode);
        _WARN_IF(err, "mkdir(%s) failed: %s", str, std::strerror(err));
    }
    
    static bool exist(const char *path) {
        return access(path, 0) == 0;
    }

    static bool isDirectory(const char *path) {
        struct stat st = {};
        if (stat(path, &st) == 0) {
            return S_ISDIR(st.st_mode);
        }
        return false;
    }
};


class File : Object {
public:
    explicit File(const char *path) : m_path(path) {}

    File(const File& o) : m_path(o.m_path), Object(o) {}

public:
    bool exist() {
        return FileUtils::exist(m_path.c_str());
    }

    size_t write(uint8_t *data, size_t size) { return FileUtils::write(m_path.c_str(), data, size); }

    size_t length() { return FileUtils::fileLength(m_path.c_str()); }

    size_t read(uint8_t *out, size_t size) {
        return FileUtils::read(m_path.c_str(), out, size);
    }

    RawData readAll() {
        return FileUtils::read(m_path.c_str());
    }

    const std::string &path() { return m_path; }

    bool remove() {
        return FileUtils::deleteFile(m_path.c_str());
    }

private:
    std::string m_path;
};

NAMESPACE_END
