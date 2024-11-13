//
// Created by LiangKeJin on 2024/11/13.
//


#include "XCompGLBinder.h"

NAMESPACE_DEFAULT

void XCompGLBinder::bind(const char *compName) {
    if (compName == nullptr) {
        this->unbind();
        m_engine->updateSurface(nullptr, 0, 0);
        return;
    }

    if (m_bind_comp_name == compName) {
        _WARN("GLEngine(%s) already bind component: %s", m_engine->name(), compName);
        return;
    }

    _INFO("GLEngine(%s) bind component: %s", m_engine->name(), compName);
    if (!m_bind_comp_name.empty()) {
        // 取消之前的绑定
        XCompMgr::unregisterListener(m_bind_comp_name.c_str(), this);
    }

    m_bind_comp_name = compName;
    auto component = XCompMgr::getComponent(compName);
    if (component != nullptr) {
        onSurfaceCreated(*component);
    } else if (!m_bind_comp_name.empty()) {
        m_engine->removeSurface();
    }
    XCompMgr::registerListener(compName, this);
}

void XCompGLBinder::unbind() {
    if (!m_bind_comp_name.empty()) {
        XCompMgr::unregisterListener(m_bind_comp_name.c_str(), this);
        m_bind_comp_name = "";
    }
}

void XCompGLBinder::onSurfaceCreated(XComponent &component) {
    uint64_t width = 0, height = 0;
    component.getSize(width, height);
    if (width > 0 && height > 0) {
        m_engine->updateSurface(component.window(), (int) width, (int) height);
    } else {
        _WARN("onSurfaceCreated XComponent(%s) size is invalid: %lu x %lu", component.id(), width, height);
    }
}

void XCompGLBinder::onSurfaceChanged(XComponent &component) {
    uint64_t width = 0, height = 0;
    component.getSize(width, height);
    if (width > 0 && height > 0) {
        m_engine->updateSurface(component.window(), (int) width, (int) height);
    } else {
        _WARN("onSurfaceChanged XComponent(%s) size is invalid: %lu x %lu", component.id(), width, height);
    }
}

void XCompGLBinder::onSurfaceDestroyed(XComponent &component) {
    m_engine->removeSurface();
}

NAMESPACE_END