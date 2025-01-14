//
// Created by wutacam on 2025/1/14.
//

#pragma once

#include <imgui.h>
#include <common/Common.h>

class IWindow {
public:
    explicit IWindow(const std::string &name) : m_name(name) {
    }

    virtual ~IWindow() {}

public:
    const std::string &name() const {
        return m_name;
    }

    // 不管窗口可不可见都会调用
    virtual void onAppInit(int width, int height) {
        //
    }

    // 不管窗口可不可见都会调用
    virtual void onAppExit() {
        //
    }

    virtual void onVisible(int width, int height) {
        _INFO("IWindow(%s)::onVisible", m_name);
    }

    virtual void onInvisible(int width, int height) {
        _INFO("IWindow(%s)::onInvisible", m_name);
    }

    virtual void onPreRender(int width, int height) {
        //
    }

    virtual void onRenderImgui(int width, int height, ImGuiIO &io) = 0;

    virtual void onPostRender(int width, int height) {
        //
    }

private:
    const std::string m_name;
};
