//
// Created by LiangKeJin on 2024/11/13.
//

#pragma once

#include <ZNamespace.h>
#include <common/Object.h>
#include <common/egl/GLEngine.h>
#include <harmony/xcomp/XComponent.h>

NAMESPACE_DEFAULT

class XCompGLBinder : public XComponentListener {
public:
    explicit XCompGLBinder(GLEngine *e) : m_engine(e) {}

public:
    void bind(const char *compName);

    void unbind();

public:
    void onSurfaceCreated(XComponent &component) override;

    void onSurfaceChanged(XComponent &component) override;

    void onSurfaceDestroyed(XComponent &component) override;

private:
    GLEngine *m_engine;
    std::string m_bind_comp_name;
};

NAMESPACE_END