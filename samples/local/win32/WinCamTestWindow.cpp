//
// Created by wutacam on 2025/1/14.
//

#include "WinCamTestWindow.h"
#include <windows.h>
#include <vector>
#include <local/win32/cam/CamDevice.h>
#include <camera/camera.h>

using namespace DirectShowCamera;

std::vector<znative::CamDevice> allCamDevices;

Camera *m_camera = nullptr;
Frame m_frame;

void WinCamTestWindow::onPreRender(int width, int height) {
    if (m_camera && m_camera->isOpened() && m_camera->isCapturing()) {
        if (m_camera->getNewFrame(m_frame)) {
            _INFO("getNewFrame: Size(%dx%d), frameSize: %d, frameType: %d", m_frame.getWidth(), m_frame.getHeight(), m_frame.getFrameSize(), m_frame.getFrameType());
        }
    }
}

void WinCamTestWindow::onRenderImgui(int width, int height, ImGuiIO &io) {
    ImGui::Begin("Win32 Camera Test");

    static std::string openCamLabel = "打开相机";
    if (ImGui::Button(openCamLabel.c_str())) {
        if (m_camera == nullptr) {
            openCamLabel = "关闭相机";
            m_camera = new Camera();

            std::vector<CameraDevice> allCamDevices = m_camera->getCameras();
            for (auto &c : allCamDevices) {
                _INFO("Camera Device: %s", c);
            }

            bool success = m_camera->Open();
            _INFO("open camera result: %d", success);
            if (success) {
                m_camera->StartCapture();
            }
        } else {
            openCamLabel = "打开相机";
            if (m_camera->isCapturing()) {
                m_camera->StopCapture();
            }

            delete m_camera;
            m_camera = nullptr;
        }
    }

    if (ImGui::Button("枚举所有摄像头")) {
        allCamDevices = znative::CamDevice::enumAllDevices();
    }
    for (auto &device : allCamDevices) {
        ImGui::NewLine();
        ImGui::Button(device.name().c_str());
        ImGui::Text("%s", device.toString().c_str());
    }
    ImGui::End();
}
