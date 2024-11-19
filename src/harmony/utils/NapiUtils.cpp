//
// Created on 2024/4/21.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "NapiUtils.h"

NAMESPACE_DEFAULT

class CallbackContext {
public:
    CallbackContext(napi_env env, napi_value cb) : m_env(env) { m_callback_ref = m_env.createRef(cb); }

    CallbackContext(const CallbackContext &other)
        : m_env(other.m_env), m_callback_ref(other.m_callback_ref), m_code(other.m_code) {}

    ~CallbackContext() {
        if (m_callback_ref) {
            m_env.deleteRef(m_callback_ref);
            m_callback_ref = nullptr;
        }
    }

    void setResult(int code) { m_code = code; }

    void callback() {
        _FATAL_IF(!m_callback_ref, "callback has been destroyed!");
        m_env.runInScope([&]() {
            napi_value cb = m_env.getRefValue(m_callback_ref);
            napi_value retArg = m_env.create(m_code);
            napi_value ret;
            napi_call_function(m_env.env(), nullptr, cb, 1, &retArg, &ret);

            m_env.deleteRef(m_callback_ref);
            m_callback_ref = nullptr;
        });
    }

public:
    NapiEnv m_env;
    napi_ref m_callback_ref;
    int m_code = 0;
};

std::mutex _g_map_mutex;
std::unordered_map<std::string, CallbackContext *> _g_callback_map;

bool NapiUtils::setCallback(const char *name, napi_env env, napi_value callback) {
    std::lock_guard<std::mutex> lock(_g_map_mutex);
    auto it = _g_callback_map.find(name);
    if (it != _g_callback_map.end()) {
        _WARN("callback(%s) already exists, skip this callback", name);
        return false;
    }
    _g_callback_map[name] = new CallbackContext(env, callback);
    return true;
}

bool NapiUtils::callback(const char *name, int code, bool delAfterCb) {
    std::lock_guard<std::mutex> lock(_g_map_mutex);
    auto it = _g_callback_map.find(name);
    if (it == _g_callback_map.end()) {
        _WARN("callback(%s) not found", name);
        return false;
    }
    if (delAfterCb) {
        _g_callback_map.erase(it);
    }

    CallbackContext *tx = it->second;
    tx->setResult(code);
    tx->m_env.queueWork((void *)tx, [](uv_work_t *work) {},
                        [](uv_work_t *res, int status) {
                            std::lock_guard<std::mutex> lock(_g_map_mutex);
                            CallbackContext *tx = static_cast<CallbackContext *>(res->data);
                            tx->callback();
                            delete tx;
                        });
    return true;
}

void NapiUtils::deleteCallback(const char *name) {
    std::lock_guard<std::mutex> lock(_g_map_mutex);
    auto it = _g_callback_map.find(name);
    if (it!= _g_callback_map.end()) {
        _g_callback_map.erase(it);
        delete it->second;
    }
}


NAMESPACE_END