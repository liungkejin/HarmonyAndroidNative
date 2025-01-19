//
// Created on 2024/6/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <Namespace.h>
#include "napi/native_api.h"
#include <common/egl/GLEngine.h>
#include <ace/xcomponent/native_interface_xcomponent.h>

NAMESPACE_DEFAULT

class XComponent {
public:
    explicit XComponent(OH_NativeXComponent *component) : m_component(component) {}
    XComponent(const XComponent &other) : m_component(other.m_component), m_window(other.m_window) {}

    std::string id() {
        uint64_t m_id_str_size = OH_XCOMPONENT_ID_LEN_MAX + 1;
        char m_id_str[OH_XCOMPONENT_ID_LEN_MAX + 1] = {0};

        int32_t error = OH_NativeXComponent_GetXComponentId(m_component, m_id_str, &m_id_str_size);
        _ERROR_RETURN_IF(error, "", "OH_NativeXComponent_GetXComponentId failed: %d", error);

        return m_id_str;
    }

    inline OH_NativeXComponent *component() const { return m_component; }

    inline void updateWindow(void *window) { m_window = window; }

    inline void *window() const { return m_window; }

    bool getSize(uint64_t &width, uint64_t &height) {
        if (m_window == nullptr) {
            width = height = 0;
            _WARN("XComponent::getSize failed, m_window is nullptr");
            return false;
        }
        int32_t error = OH_NativeXComponent_GetXComponentSize(m_component, m_window, &width, &height);
        _WARN_RETURN_IF(error, false, "OH_NativeXComponent_GetXComponentSize failed: %d", error);
        return true;
    }

    bool getOffset(double &x, double &y) {
        if (m_window == nullptr) {
            x = y = 0;
            _WARN("XComponent::getOffset failed, m_window is nullptr");
            return false;
        }
        int32_t error = OH_NativeXComponent_GetXComponentOffset(m_component, m_window, &x, &y);
        _ERROR_RETURN_IF(error, false, "OH_NativeXComponent_GetXComponentOffset failed: %d", error);
        return true;
    }

public:
    bool bindToGLEngine(GLEngine &engine) {
        uint64_t width, height;
        if (!getSize(width, height)) {
            _WARN("XComponent::bindToGLEngine failed, getSize failed");
            return false;
        }
        engine.updateSurface(m_window, (int) width, (int) height);
        return true;
    }

private:
    OH_NativeXComponent *m_component;
    void *m_window = nullptr;
};

class XComponentListener {
public:
    virtual void onSurfaceCreated(XComponent &component) = 0;
    virtual void onSurfaceChanged(XComponent &component) = 0;
    virtual void onSurfaceDestroyed(XComponent &component) = 0;
    virtual void dispatchTouchEvent(XComponent &component){};
    virtual void dispatchMouseEvent(XComponent &component) {}
};

class XCompMgr {
public:
    static napi_value initialize(napi_env env, napi_value exports);

    static void registerListener(const char *compId, XComponentListener *listener);

    static void unregisterListener(const char *compId, XComponentListener *listener);

    static XComponent *getComponent(const char *compId);
};

NAMESPACE_END
