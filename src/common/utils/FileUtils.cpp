//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#include "FileUtils.h"

#include <filesystem>

NAMESPACE_DEFAULT

bool FileUtils::test() {
    FileUtils::mkDir("fstest");
    FileUtils::mkDir("fstest/1");
    FileUtils::mkDir("fstest/2");
    FileUtils::mkDir("fstest/abc");
    FileUtils::write("fstest/1.txt", "1", 1);
    FileUtils::write("fstest/2.txt", "20000", 5);
    FileUtils::write("fstest/1/11.txt", "11", 2);

    _INFO("fullPath(.): %s", fullPath("."));
    _INFO("filename(fstest/1.txt): %s", fileName("fstest/1.txt"));
    _INFO("fileBasename(fstest/1.txt): %s", fileBaseName("fstest/1.txt"));
    _INFO("fileExtension(fstest/1.txt): %s", fileExtension("fstest/1.txt"));
    _INFO("fileParentDir(fstest/1.txt): %s", fileParentDir("fstest/1.txt"));
    _INFO("fileLength(fstest/1.txt): %d", fileLength("fstest/1.txt"));
    auto data = read("fstest/1.txt");
    _INFO("fileContent(fstest/1.txt): %s", data.toString());
    _INFO("fileLength(fstest/2.txt): %d", fileLength("fstest/2.txt"));
    data = read("fstest/2.txt");
    _INFO("fileContent(fstest/2.txt): %s", data.toString());

    {
        std::stringstream ss;
        auto list = listFiles("fstest");
        for (const auto &f : list) {
            ss << f << "\n";
        }
        _INFO("listFiles(fstest): size(%d) \n%s", list.size(), ss.str().c_str());
    }

    _INFO("file exist(fstest/1.txt): %d", exist("fstest/1.txt"));
    _INFO("deleteFile(fstest/1.txt): %d", deleteFile("fstest/1.txt"));
    _INFO("file exist(fstest/1.txt): %d", exist("fstest/1.txt"));

    {
        std::stringstream ss;
        auto list = listFiles("fstest");
        for (const auto &f : list) {
            ss << f << "\n";
        }
        _INFO("listFiles(fstest): size(%d) \n%s", list.size(), ss.str().c_str());
    }

    _INFO("isDirectory(fstest): %d", isDirectory("fstest"));
    _INFO("isDirectory(fstest/2.txt): %d", isDirectory("fstest/2.txt"));

    {
        std::stringstream ss;
        auto list = listFilesSort("fstest");
        for (const auto &f : list) {
            ss << f << "\n";
        }
        _INFO("listFilesSort(fstest): size(%d) \n%s", list.size(), ss.str().c_str());
    }

    _INFO("remove(fstest): %d", remove("fstest"));

    {
        std::stringstream ss;
        auto list = listFilesSort("fstest");
        for (const auto &f : list) {
            ss << f << "\n";
        }
        _INFO("listFilesSort(fstest): size(%d) \n%s", list.size(), ss.str().c_str());
    }

    return true;
}

// 文件的绝对路径
std::string FileUtils::fullPath(const char *path) {
    auto p = std::filesystem::canonical(path);
    return p.u8string();
}

// 带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx.txt"
std::string FileUtils::fileName(const char *path) {
    // 使用 std::filesystem
    std::filesystem::path p(path);
    return p.filename().u8string();
}

// 不带文件类型的文件名 "/dir/xxx.txt" 返回 "xxx"
std::string FileUtils::fileBaseName(const char *path) {
    std::filesystem::path p(path);
    return p.stem().u8string();
}

// 文件的扩展名 "/dir/xxx.txt" 返回 ".txt"
std::string FileUtils::fileExtension(const char *path) {
    std::filesystem::path p(path);
    return p.extension().u8string();
}

// 文件的父文件夹 "/dir/xxx.txt" 返回 "/dir"
std::string FileUtils::fileParentDir(const char *path) {
    std::filesystem::path p(path);
    return p.parent_path().u8string();
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
    std::FILE *file = std::fopen(filepath, "r");
    if (file == nullptr) {
        return {};
    }
    std::fseek(file, 0, SEEK_END);
    size_t len = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    RawData data(len);
    size_t readSize = std::fread(data.data(), 1, len, file);
    std::fclose(file);
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
    size_t readCount = std::fread(out, 1, size, file);
    std::fclose(file);
    return readCount;
}

size_t FileUtils::fileLength(const char *filepath) {
    std::error_code err;
    size_t s = std::filesystem::file_size(filepath, err);
    _ERROR_IF(err, "get file length(%s) failed: %s", filepath, err.message());
    return s;
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
    std::error_code err;
    bool success = std::filesystem::remove(str, err);
    _ERROR_IF(err, "delete file(%s) failed: %ws", str, err.message());
    return success;
}

bool FileUtils::mkDir(const char *str) {
    if (isDirectory(str)) {
        return true;
    }
    std::error_code err;
    bool success = std::filesystem::create_directory(str, err);
    _ERROR_IF(err, "mkdir(%s) failed: %s", str, err.message());
    return success;
}

bool FileUtils::remove(const char *dirOrFile) {
    std::error_code err;
    std::filesystem::remove_all(dirOrFile, err);
    _ERROR_IF(err, "remove(%s) failed: %s", dirOrFile, err.message());
    return err.value() == 0;
}

bool FileUtils::exist(const char *path) {
    std::error_code error;
    bool result = std::filesystem::exists(path, error);
    _ERROR_IF(error, "exist(%s) failed: %s", path, error.message());
    return result;
}

bool FileUtils::isDirectory(const char *path) {
    std::error_code err;
    return std::filesystem::is_directory(path, err);
}

std::vector<std::string> FileUtils::listFiles(const char *path) {
    if (!isDirectory(path)) {
        return {};
    }
    std::vector<std::string> files;
    std::error_code error;
    for (const auto &entry : std::filesystem::directory_iterator(path, error)) {
        files.push_back(entry.path().u8string());
    }
    _ERROR_IF(error, "listFiles(%s) failed: %s", path, error.message());

    return files;
}

std::vector<std::string> FileUtils::listFilesSort(const char *path,
                                                  const std::function<int(const std::string &a,
                                                                          const std::string &b)> &comp) {
    std::vector<std::string> files = listFiles(path);
    // sort files by alpha
    if (comp == nullptr) {
        std::sort(files.begin(), files.end());
    } else {
        std::sort(files.begin(), files.end(), comp);
    }
    return files;
}

NAMESPACE_END