#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <tinyformat.h>
#include <cstdio>
#include <iostream>

#ifndef LOG_TAG
#define LOG_TAG "zzz_native"
#endif

#ifdef __HARMONY_OS__
#include <hilog/log.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN 0x100

#ifdef __DEBUG__
#define __LOG_DEBUG(msg) OH_LOG_DEBUG(LOG_APP, "%{public}s", msg);
#define __LOG_INFO(msg) OH_LOG_INFO(LOG_APP, "%{public}s", msg);
#else
#define __LOG_DEBUG(msg)
#define __LOG_INFO(msg)
#endif

#define __LOG_WARN(msg) OH_LOG_WARN(LOG_APP, "%{public}s", msg);
#define __LOG_ERROR(msg) OH_LOG_ERROR(LOG_APP, "%{public}s", msg);

#elif defined(__ANDROID__)
#include <android/log.h>

#ifdef __DEBUG__
#define __LOG_DEBUG(msg) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", msg)
#define __LOG_INFO(msg)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", msg)
#else
#define __LOG_DEBUG(msg)
#define __LOG_INFO(msg)
#endif

#define __LOG_WARN(msg)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s", msg)
#define __LOG_ERROR(msg) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", msg)

#else

#define __LOG_DEBUG(msg) fprintf(stdout, "%s\n", msg);
#define __LOG_INFO(msg) fprintf(stdout, "%s\n", msg);
#define __LOG_WARN(msg) fprintf(stdout, "%s\n", msg);
#define __LOG_ERROR(msg) fprintf(stderr, "%s\n", msg);

#endif

// 严格模式，返回空直接抛出运行时异常
#ifdef __DEBUG__
#define STRICT_MODE true
#else
#define STRICT_MODE false
#endif

extern FILE *__g_logFile;

static inline void setLogFile(const char *path) { __g_logFile = fopen(path, "a"); }

static inline std::string __prettyMethodName(const std::string &prettyFunction) {
    size_t begin = prettyFunction.find(' ') + 1;
    size_t end = prettyFunction.find('(') - begin;

    return prettyFunction.substr(begin, end) + "()";
}

#define __PRETTY_FORMAT(fmt, args...)                                                                                  \
    tfm::format("[%s:%d] " fmt, __prettyMethodName(__PRETTY_FUNCTION__), __LINE__, ##args)

#define _DEBUG(fmt, args...)                                                                                           \
    do {                                                                                                               \
        std::string _log_str = __PRETTY_FORMAT(fmt, ##args);                                                           \
        if (__g_logFile != nullptr) {                                                                                  \
            fprintf(__g_logFile, "[D] %s\n", _log_str.c_str());                                                        \
            fflush(__g_logFile);                                                                                       \
        } else {                                                                                                       \
            __LOG_DEBUG(_log_str.c_str());                                                                             \
        }                                                                                                              \
    } while (0)

#define _INFO(fmt, args...)                                                                                            \
    do {                                                                                                               \
        std::string _log_str = __PRETTY_FORMAT(fmt, ##args);                                                           \
        if (__g_logFile != nullptr) {                                                                                  \
            fprintf(__g_logFile, "[I] %s\n", _log_str.c_str());                                                        \
            fflush(__g_logFile);                                                                                       \
        } else {                                                                                                       \
            __LOG_INFO(_log_str.c_str());                                                                              \
        }                                                                                                              \
    } while (0)

#define _WARN(fmt, args...)                                                                                            \
    do {                                                                                                               \
        std::string _log_str = __PRETTY_FORMAT(fmt, ##args);                                                           \
        if (__g_logFile != nullptr) {                                                                                  \
            fprintf(__g_logFile, "[W] %s\n", _log_str.c_str());                                                        \
            fflush(__g_logFile);                                                                                       \
        } else {                                                                                                       \
            __LOG_WARN(_log_str.c_str());                                                                              \
        }                                                                                                              \
    } while (0)

#define _ERROR(fmt, args...)                                                                                           \
    do {                                                                                                               \
        std::string _log_str = __PRETTY_FORMAT(fmt, ##args);                                                           \
        if (__g_logFile != nullptr) {                                                                                  \
            fprintf(__g_logFile, "[E] %s\n", _log_str.c_str());                                                        \
            fflush(__g_logFile);                                                                                       \
        } else {                                                                                                       \
            __LOG_ERROR(_log_str.c_str());                                                                             \
        }                                                                                                              \
        if (STRICT_MODE) {                                                                                             \
            throw std::runtime_error(_log_str);                                                                        \
        }                                                                                                              \
    } while (0)

#define _FATAL(fmt, args...)                                                                                           \
    do {                                                                                                               \
        std::string _log_str = __PRETTY_FORMAT(fmt, ##args);                                                           \
        if (__g_logFile != nullptr) {                                                                                  \
            fprintf(__g_logFile, "[E] %s\n", _log_str.c_str());                                                        \
            fflush(__g_logFile);                                                                                       \
        } else {                                                                                                       \
            __LOG_ERROR(_log_str.c_str());                                                                             \
        }                                                                                                              \
        throw std::runtime_error(_log_str);                                                                            \
    } while (0)

#define _ASSERT(condition) if (condition) { _FATAL("" #condition); }

#define _FATAL_IF(condition, fmt, args...)                                                                             \
    if (condition) {                                                                                                   \
        _FATAL(fmt, ##args);                                                                                           \
    }

#define _ERROR_RETURN_IF(condition, retcode, fmt, args...)                                                             \
    if (condition) {                                                                                                   \
        _ERROR(fmt, ##args);                                                                                           \
        return (retcode);                                                                                              \
    }

#define _ERROR_RETURN(condition, retcode)                                                                              \
    if (condition) {                                                                                                   \
        return (retcode);                                                                                              \
    }

#define _ERROR_IF(condition, fmt, args...)                                                                             \
    if (condition) {                                                                                                   \
        _ERROR(fmt, ##args);                                                                                           \
    }

#define _WARN_RETURN_IF(condition, retcode, fmt, args...)                                                              \
    if (condition) {                                                                                                   \
        _WARN(fmt, ##args);                                                                                            \
        return (retcode);                                                                                              \
    }

#define _WARN_IF(condition, fmt, args...)                                                                              \
    if (condition) {                                                                                                   \
        _WARN(fmt, ##args);                                                                                            \
    }

#define _INFO_IF(condition, fmt, args...)                                                                              \
    if (condition) {                                                                                                   \
        _INFO(fmt, ##args);                                                                                            \
    }

#define _CHECK_RESULT(error, fmt, args...)                                                                              \
    if (error) {                                                                                                       \
        _ERROR("Error(%d): " #fmt, error, args);                                                                       \
    } else {                                                                                                           \
        _INFO(fmt, args);                                                                                              \
    }

#define _CHECK_FUNC(result, func)                                                                                      \
    auto result = func;                                                                                                \
    if (result) {                                                                                                      \
        _ERROR("" #func " failed: %d", result);                                                                        \
    } else {                                                                                                           \
        _INFO("" #func " success");                                                                                    \
    }

#pragma clang diagnostic pop