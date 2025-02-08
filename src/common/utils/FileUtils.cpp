//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#include "FileUtils.h"

#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FilePath.h>

NAMESPACE_DEFAULT

// 文件的绝对路径
std::string FileUtils::fullPath(const char *path) {
    cppfs::FilePath p(path);
    return p.fullPath();
}

// 带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx.txt"
std::string FileUtils::fileName(const char *path) {
    cppfs::FilePath p(path);
    return p.fileName();
}

// 不带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx"
std::string FileUtils::fileBaseName(const char *path) {
    cppfs::FilePath p(path);
    return p.baseName();
}

// 文件的扩展名 "/dir/xxx.txt" 返回 ".txt"
std::string FileUtils::fileExtension(const char *path) {
    cppfs::FilePath p(path);
    return p.extension();
}

// 文件的父文件夹 "/dir/xxx.txt" 返回 "/dir"
std::string FileUtils::fileParentDir(const char *path) {
    cppfs::FilePath p(path);
    return p.directoryPath();
}

size_t FileUtils::write(const char *filepath, const void *data, size_t len) {
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

RawData FileUtils::read(const char *filepath) {
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

size_t FileUtils::read(const char *filepath, uint8_t *out, size_t size, size_t offset) {
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

size_t FileUtils::fileLength(const char *filepath) {
    return cppfs::fs::open(filepath).size();
}

size_t FileUtils::fileLength(FILE *file) {
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

bool FileUtils::deleteFile(const char *str) {
    bool success = cppfs::fs::open(str).remove();
    _ERROR_IF(!success, "delete file(%s) failed", str);
    return success;
}

bool FileUtils::mkDir(const char *str) {
    if (isDirectory(str)) {
        return true;
    }
    bool success = cppfs::fs::open(str).createDirectory();
    _ERROR_IF(!success, "mkdir(%s) failed!", str);
    return success;
}

void FileUtils::remove(const char *dirOrFile) {
    if (isDirectory(dirOrFile)) {
        cppfs::fs::open(dirOrFile).removeDirectoryRec();
    } else {
        deleteFile(dirOrFile);
    }
}

bool FileUtils::exist(const char *path) {
    return cppfs::fs::open(path).exists();
}

bool FileUtils::isDirectory(const char *path) {
    return cppfs::fs::open(path).isDirectory();
}

std::vector<std::string> FileUtils::listFiles(const char *path) {
    return cppfs::fs::open(path).listFiles();
}

std::vector<std::string> FileUtils::listFilesSort(const char *path,
                                                  const std::function<int(const std::string &a,
                                                                          const std::string &b)> &comp) {
    std::vector<std::string> files = cppfs::fs::open(path).listFiles();
    // sort files by alpha
    if (comp == nullptr) {
        std::sort(files.begin(), files.end());
    } else {
        std::sort(files.begin(), files.end(), comp);
    }
    return files;
}

NAMESPACE_END