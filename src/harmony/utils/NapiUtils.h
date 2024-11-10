//
// Created on 2024/4/21.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <cstdint>
#include <string>
#include <uv.h>
#include <napi/native_api.h>
#include "ZNative.h"


#define NAPI_RETVAL_NOTHING

#define GET_AND_THROW_LAST_ERROR(env)                                                                                  \
    do {                                                                                                               \
        const napi_extended_error_info *errorInfo = nullptr;                                                           \
        napi_get_last_error_info((env), &errorInfo);                                                                   \
        bool isPending = false;                                                                                        \
        napi_is_exception_pending((env), &isPending);                                                                  \
        if (!isPending && errorInfo != nullptr) {                                                                      \
            const char *errorMessage =                                                                                 \
                errorInfo->error_message != nullptr ? errorInfo->error_message : "empty error message";                \
            napi_throw_error((env), nullptr, errorMessage);                                                            \
        }                                                                                                              \
    } while (0)

#define NAPI_ASSERT_BASE(env, assertion, message, retVal)                                                              \
    do {                                                                                                               \
        if (!(assertion)) {                                                                                            \
            napi_throw_error((env), nullptr, "assertion (" #assertion ") failed: " message);                           \
            return retVal;                                                                                             \
        }                                                                                                              \
    } while (0)

#define NAPI_ASSERT(env, assertion, message) NAPI_ASSERT_BASE(env, assertion, message, nullptr)

#define NAPI_ASSERT_RETURN_VOID(env, assertion, message) NAPI_ASSERT_BASE(env, assertion, message, NAPI_RETVAL_NOTHING)

#define NAPI_CALL_BASE(env, theCall, retVal)                                                                           \
    do {                                                                                                               \
        if ((theCall) != napi_ok) {                                                                                    \
            GET_AND_THROW_LAST_ERROR((env));                                                                           \
            return retVal;                                                                                             \
        }                                                                                                              \
    } while (0)

#define NAPI_CALL(env, theCall) NAPI_CALL_BASE(env, theCall, nullptr)

#define NAPI_CALL_RETURN_VOID(env, theCall) NAPI_CALL_BASE(env, theCall, NAPI_RETVAL_NOTHING)

#define DECLARE_NAPI_PROPERTY(name, val)                                                                               \
    { (name), nullptr, nullptr, nullptr, nullptr, val, napi_default, nullptr }

#define DECLARE_NAPI_STATIC_PROPERTY(name, val)                                                                        \
    { (name), nullptr, nullptr, nullptr, nullptr, val, napi_static, nullptr }

#define DECLARE_NAPI_FUNCTION(name, func)                                                                              \
    { (name), nullptr, (func), nullptr, nullptr, nullptr, napi_default, nullptr }

#define DECLARE_NAPI_FUNCTION_WITH_DATA(name, func, data)                                                              \
    { (name), nullptr, (func), nullptr, nullptr, nullptr, napi_default, data }

#define DECLARE_NAPI_STATIC_FUNCTION(name, func)                                                                       \
    { (name), nullptr, (func), nullptr, nullptr, nullptr, napi_static, nullptr }

#define DECLARE_NAPI_GETTER(name, getter)                                                                              \
    { (name), nullptr, nullptr, (getter), nullptr, nullptr, napi_default, nullptr }

#define DECLARE_NAPI_SETTER(name, setter)                                                                              \
    { (name), nullptr, nullptr, nullptr, (setter), nullptr, napi_default, nullptr }

#define DECLARE_NAPI_GETTER_SETTER(name, getter, setter)                                                               \
    { (name), nullptr, nullptr, (getter), (setter), nullptr, napi_default, nullptr }

NAMESPACE_DEFAULT

class NapiEnv {
public:
    explicit NapiEnv(napi_env env) : m_env(env) {}

public:
#define DEFINE_CERATE_METHOD(type, ntype)                                                                              \
    napi_value create(type value) {                                                                                    \
        napi_value ret;                                                                                                \
        napi_status status = napi_create_##ntype(m_env, value, &ret);                                                  \
        if (status != napi_ok) {                                                                                       \
            _ERROR("napi_create_##ntype failed: %d", status);                                                          \
            napi_throw_error(m_env, nullptr, "napi_create_##ntype failed");                                            \
        }                                                                                                              \
        return ret;                                                                                                    \
    }

    DEFINE_CERATE_METHOD(int32_t, int32)
    DEFINE_CERATE_METHOD(uint32_t, uint32)
    DEFINE_CERATE_METHOD(int64_t, int64)
    DEFINE_CERATE_METHOD(double, double)
    DEFINE_CERATE_METHOD(bool, int32)

    napi_value create(const char *str, size_t len) {
        napi_value ret;
        napi_status status = napi_create_string_utf8(m_env, str, len, &ret);
        if (status != napi_ok) {
            _ERROR("napi_create_string_utf8 failed: %d", status);
            napi_throw_error(m_env, nullptr, "napi_create_string_utf8 failed");
        }
        return ret;
    }

    napi_value create(const std::string &str) { return create(str.c_str(), str.length()); }

    napi_ref createRef(napi_value value) {
        napi_ref ret;
        napi_status status = napi_create_reference(m_env, value, 1, &ret);
        if (status != napi_ok) {
            _ERROR("napi_create_reference failed: %d", status);
            napi_throw_error(m_env, nullptr, "napi_create_reference failed");
        }
        return ret;
    }

    napi_value getRefValue(napi_ref ref) {
        napi_value ret;
        napi_status status = napi_get_reference_value(m_env, ref, &ret);
        if (status != napi_ok) {
            _ERROR("napi_get_reference_value failed: %d", status);
            napi_throw_error(m_env, nullptr, "napi_get_reference_value failed");
        }
        return ret;
    }

    void deleteRef(napi_ref ref) {
        napi_status status = napi_delete_reference(m_env, ref);
        if (status != napi_ok) {
            _ERROR("napi_delete_reference failed: %d", status);
            napi_throw_error(m_env, nullptr, "napi_delete_reference failed");
        }
    }

    void runInScope(std::function<void()> runnable) {
        napi_handle_scope scope = nullptr;
        // 打开handle scope用于管理napi_value的生命周期，否则会内存泄露。
        napi_open_handle_scope(m_env, &scope);
        _FATAL_IF(!scope, "napi_open_handle_scope failed");
        runnable();
        napi_close_handle_scope(m_env, scope);
    }

    void queueWork(void *data, uv_work_cb cb, uv_after_work_cb postCb) {
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(m_env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = data;
        uv_queue_work(loop, work, cb, postCb);
    }

    inline napi_env env() { return m_env; }

private:
    napi_env m_env;
};

class NapiArgsGetter {
public:
    NapiArgsGetter(napi_env env, napi_callback_info info, size_t argc) : m_env(env), m_info(info), m_argc(argc) {
        if (argc > 0) {
            m_argv = new napi_value[argc];
            napi_get_cb_info(env, info, &m_argc, m_argv, nullptr, nullptr);
        }
    }

    ~NapiArgsGetter() { delete[] m_argv; }

public:
#define DEFINE_GET_METHOD(ret, type)                                                                                   \
    ret type##At(int i) {                                                                                              \
        _FATAL_IF(i < 0 || i >= m_argc, "JNIArgsGetter::type##At(%d) error, args count: %d", i, m_argc)                \
        ret value;                                                                                                     \
        napi_status status = napi_get_value_##type(m_env, m_argv[i], &value);                                          \
        if (status != napi_ok) {                                                                                       \
            _ERROR("napi_get_value_##type failed: %d", status);                                                        \
            napi_throw_error(m_env, nullptr, "napi_get_value_##type failed");                                          \
        }                                                                                                              \
        return value;                                                                                                  \
    }

    DEFINE_GET_METHOD(double, double)
    DEFINE_GET_METHOD(int32_t, int32)
    DEFINE_GET_METHOD(uint32_t, uint32)
    DEFINE_GET_METHOD(int64_t, int64)
    DEFINE_GET_METHOD(bool, bool)

    std::string stringUtf8At(int i) {
        _FATAL_IF(i < 0 || i >= m_argc, "JNIArgsGetter::stringUtf8At(%d) error, args count: %d", i, m_argc)
        size_t len = 0;
        // 先拿到长度
        napi_status status = napi_get_value_string_utf8(m_env, m_argv[i], nullptr, 0, &len);
        if (status != napi_ok) {
            _ERROR("napi_get_value_string_utf8 failed: %d", status);
            napi_throw_error(m_env, nullptr, "napi_get_value_string_utf8 failed");
        }
        char str[len + 1];
        status = napi_get_value_string_utf8(m_env, m_argv[i], str, len + 1, &len);
        if (status != napi_ok) {
            _ERROR("napi_get_value_string_utf8 failed: %d", status);
            napi_throw_error(m_env, nullptr, "napi_get_value_string_utf8 failed");
        }
        return std::string(str, len);
    }

    napi_value at(int i) {
        _FATAL_IF(i < 0 || i >= m_argc, "JNIArgsGetter::at(%d) error, args count: %d", i, m_argc)
        return m_argv[i];
    }

    bool has(int i) { return i >= 0 && i < m_argc; }

    napi_value next() {
        return at(m_index++);
    }

    double nextDouble() {
        return doubleAt(m_index++);
    }

    int32_t nextInt32() {
        return int32At(m_index++);
    }

    uint32_t nextUInt32() {
        return uint32At(m_index++);
    }

    int64_t nextInt64() {
        return int64At(m_index++);
    }

    bool nextBool() {
        return boolAt(m_index++);
    }

    std::string nextString() {
        return stringUtf8At(m_index++);
    }

private:
    napi_env m_env;
    napi_callback_info m_info;
    size_t m_argc;
    napi_value *m_argv = nullptr;

    int m_index = 0;
};

class NapiCtx {
public:
    NapiCtx(napi_env env, napi_callback_info info, size_t argc=0) : env(env), args(env, info, argc) {}

public:
    NapiEnv env;
    NapiArgsGetter args;
};

class NapiUtils {
public:
    static bool setCallback(const char *name, napi_env env, napi_value callback);

    static bool callback(const char *name, int code);

    static void deleteCallback(const char *name);
};

NAMESPACE_END
