//
// Created by wutacam on 2025/1/13.
//

#ifndef WINUTILS_H
#define WINUTILS_H

#include <codecvt>
#include <locale>
#include <common/common.h>
#include <windows.h>

NAMESPACE_DEFAULT
class WinUtils {
public:
    static std::wstring stringToWString(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    static std::string wstringToString(const std::wstring& wstr) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }

    // wchar_t to std::string
    static std::string bstrToString(wchar_t *bstr) {
        if (!bstr) {
            return "";
        }
        int size = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, nullptr, 0, nullptr, nullptr);
        std::string result(size - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, bstr, -1, &result[0], size, nullptr, nullptr);
        return result;
    }

    static wchar_t *stringToBstr(const std::string &str) {
        // 首先计算所需的 wchar_t 数组的大小，包括结束符
        size_t size = str.size() + 1;
        // 使用 std::vector 来存储 wchar_t 数组，避免手动释放内存
        std::vector<wchar_t> buffer(size);
        // 调用 mbstowcs 函数进行转换
        size_t result = mbstowcs(&buffer[0], str.c_str(), size);
        if (result == (size_t)-1) {
            _ERROR("stringToBstr: mbstowcs() failed");
            return nullptr;
        }
        // 返回转换后的 wchar_t*
        return &buffer[0];
    }

    static std::string guidToString(const GUID &guid) {
        int buf_len = 64;
        char buf[64] = {0};
        _snprintf(
            buf,
            buf_len,
            "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
        return std::string(buf);
    }
};

NAMESPACE_END

#endif //WINUTILS_H
