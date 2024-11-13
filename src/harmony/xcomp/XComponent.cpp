//
// Created on 2024/6/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "XComponent.h"
#include <map>
#include <vector>

NAMESPACE_DEFAULT

static std::map<std::string, XComponent> g_component_map;
static std::map<std::string, std::vector<XComponentListener*>> g_component_listeners;

//static EGLCtx testCtx;

static XComponent * findComponent(OH_NativeXComponent *comp) {
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {0};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    OH_NativeXComponent_GetXComponentId(comp, idStr, &idSize);
    
    auto itComp = g_component_map.find(idStr);
    if (itComp == g_component_map.end()) {
        _ERROR("XComMgr: Could not find component: %s", idStr);
        return nullptr;
    }
    itComp->second = XComponent(comp);
    return &itComp->second;
}

static void onSurfaceCreatedCB(OH_NativeXComponent *component, void *window) {
    _INFO("XComMgr: onSurfaceCreated, native component: %p, window: %p", component, window);
    XComponent *xc = findComponent(component);
    if (xc == nullptr) {
        return;
    }
    xc->updateWindow(window);
    uint64_t width, height;
    if (!xc->getSize(width, height)) {
        _WARN("XComMgr: Could not get size of component: %s", component);
    }
    _INFO("XComMgr: onSurfaceCreated, window size: %lu x %lu", width, height);
//    testCtx.makeCurrent(window);
//    GLUtil::clearColor(1, 0, 0, 1);
//    testCtx.swapBuffers();
    
    auto itListeners = g_component_listeners.find(xc->id());
    if (itListeners == g_component_listeners.end()) {
        return;
    }
    for (auto &listener : itListeners->second) {
        listener->onSurfaceCreated(*xc);
    }
}

static void onSurfaceChangedCB(OH_NativeXComponent *component, void *window) {
    _INFO("XComMgr: onSurfaceChanged, native component: %p", component);
    XComponent *xc = findComponent(component);
    if (xc == nullptr) {
        return;
    }
    xc->updateWindow(window);
    auto itListeners = g_component_listeners.find(xc->id());
    if (itListeners == g_component_listeners.end()) {
        return;
    }
    for (auto &listener : itListeners->second) {
        listener->onSurfaceChanged(*xc);
    }
}

static void onSurfaceDestroyedCB(OH_NativeXComponent *component, void *window) {
    _INFO("XComMgr: onSurfaceDestroyed, native component: %p", component);
    XComponent *xc = findComponent(component);
    if (xc == nullptr) {
        return;
    }
    xc->updateWindow(window);
    auto itListeners = g_component_listeners.find(xc->id());
    if (itListeners == g_component_listeners.end()) {
        return;
    }
    for (auto &listener : itListeners->second) {
        listener->onSurfaceDestroyed(*xc);
    }
    g_component_listeners.erase(itListeners);
}

static void dispatchTouchEventCB(OH_NativeXComponent *component, void *window) {
    _INFO("XComMgr: dispatchTouchEvent, native component: %p", component);
    XComponent *xc = findComponent(component);
    if (xc == nullptr) {
        return;
    }
    xc->updateWindow(window);
    auto itListeners = g_component_listeners.find(xc->id());
    if (itListeners == g_component_listeners.end()) {
        return;
    }
    for (auto &listener : itListeners->second) {
        listener->dispatchTouchEvent(*xc);
    }
}

static void dispatchMouseEventCB(OH_NativeXComponent *component, void *window) {
    _INFO("XComMgr: dispatchMouseEvent, native component: %p", component);
    XComponent *xc = findComponent(component);
    if (xc == nullptr) {
        return;
    }
    xc->updateWindow(window);
    auto itListeners = g_component_listeners.find(xc->id());
    if (itListeners == g_component_listeners.end()) {
        return;
    }
    for (auto &listener : itListeners->second) {
        listener->dispatchMouseEvent(*xc);
    }
}

static OH_NativeXComponent_Callback g_x_component_callback{
    .OnSurfaceCreated = onSurfaceCreatedCB,
    .OnSurfaceChanged = onSurfaceChangedCB,
    .OnSurfaceDestroyed = onSurfaceDestroyedCB,
    .DispatchTouchEvent = dispatchTouchEventCB
};

static OH_NativeXComponent_MouseEvent_Callback g_x_component_mouse_event_callback{
    .DispatchMouseEvent = dispatchMouseEventCB
};


napi_value XCompMgr::initialize(napi_env env, napi_value exports) {
    _INFO("XCompMgr::initialize");

    napi_status status;
    napi_value xcompObj = nullptr;
    OH_NativeXComponent *nativeXComponent = nullptr;

    status = napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &xcompObj);
    if (status != napi_ok) {
        _INFO("XCompMgr::initialize() no XComponent object found, status: %d", status);
        return exports;
    }

    status = napi_unwrap(env, xcompObj, reinterpret_cast<void **>(&nativeXComponent));
    if (status != napi_ok) {
        _WARN("XCompMgr::initialize() XComponent napi_unwrap failed: %d", status);
        return exports;
    }
    
    if (nativeXComponent == nullptr) {
        _WARN("XCompMgr::initialize() nativeXComponent is nullptr");
        return exports;
    }
    
    XComponent component(nativeXComponent);
    _INFO("XCompMgr::initialize() success, register xcomponent(%s), ptr: %p", component.id(), nativeXComponent);
    
    std::string idstr = component.id();
    if (g_component_map.find(idstr) == g_component_map.end()) {
        g_component_map.insert({idstr, component});
    }

    int32_t error = OH_NativeXComponent_RegisterCallback(nativeXComponent, &g_x_component_callback);
    _INFO("XCompMgr::initialize() register xcomponent callback: %d", error);
    error = OH_NativeXComponent_RegisterMouseEventCallback(nativeXComponent, &g_x_component_mouse_event_callback);
    _INFO("XCompMgr::initialize() register xcomponent mouse event callback: %d", error);
    return exports;
}

void XCompMgr::registerListener(const char *compId, XComponentListener *listener) {
    auto it = g_component_listeners.find(compId);
    if (it == g_component_listeners.end()) {
        std::vector<XComponentListener *> listeners;
        listeners.push_back(listener);
        g_component_listeners[compId] = listeners;
    } else {
        // 除重
        for (auto &l : it->second) {
            if (l == listener) {
                return;
            }
        }
        it->second.push_back(listener);
    }
}

void XCompMgr::unregisterListener(const char *compId, XComponentListener *listener) {
    auto it = g_component_listeners.find(compId);
    if (it == g_component_listeners.end()) {
        return;
    }
    for (auto i = it->second.begin(); i != it->second.end(); i++) {
        if (*i == listener) {
            it->second.erase(i);
            break;
        }
    }
    if (it->second.empty()) {
        g_component_listeners.erase(it);
    }
}

XComponent *XCompMgr::getComponent(const char *compId) {
    if (compId == nullptr) {
        return nullptr;
    }
    auto it = g_component_map.find(compId);
    if (it == g_component_map.end()) {
        return nullptr;
    }
    return &it->second;
}

NAMESPACE_END