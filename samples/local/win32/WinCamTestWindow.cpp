//
// Created by wutacam on 2025/1/14.
//

#include "WinCamTestWindow.h"
#include <windows.h>
#include <vector>
#include <local/win32/cam/CamDevice.h>
#include <local/win32/dshow/DSUtils.h>
#include <camera/camera.h>
#include <common/gles/Texture.h>

using namespace DirectShowCamera;
using namespace znative;

std::vector<CameraDevice> all_camera_devices;
std::string selected_camera_device_name;
std::string selected_camera_device_path;

Camera *m_camera = nullptr;
Frame m_frame;

ImageTexture m_img_tex;

int64_t m_camera_fps = 0;
int64_t m_camera_frame_count = 0;
int64_t m_camera_start_ms = 0;

void WinCamTestWindow::onPreRender(int width, int height) {
    if (m_camera && m_camera->isOpened() && m_camera->isCapturing()) {
        if (m_camera->getFrame(m_frame, true)) {
            _INFO("getNewFrame: Size(%dx%d), frameSize: %d, frameType: %d, raw frame type: %s",
                m_frame.getWidth(), m_frame.getHeight(), m_frame.getFrameSize(), m_frame.getFrameType(), DSUtils::videoTypeString(m_frame.getRawFrameType()));
            m_camera_frame_count += 1;
            if (m_camera_start_ms == 0) {
                m_camera_start_ms = TimeUtils::nowMs();
            }
            int64_t costMs = TimeUtils::nowMs() - m_camera_start_ms;
            if (m_camera_frame_count > 1 && costMs > 0) {
                m_camera_fps = (int64_t) (m_camera_frame_count * 1000.0 / costMs + 0.5);
            }
            if (costMs > 5000) {
                m_camera_frame_count = 0;
                m_camera_start_ms = TimeUtils::nowMs();
            }

            int bytes = 0;
            auto data = m_frame.getFrameData(bytes);
            m_img_tex.set(data.get(), m_frame.getWidth(), m_frame.getHeight(), GL_BGR);
        }
    }
}

void WinCamTestWindow::onRenderImgui(int width, int height, ImGuiIO &io) {
    ImGui::Begin("Win32 Camera Test");

    if (ImGui::Button("枚举所有相机设备:")) {
        if (m_camera == nullptr) {
            m_camera = new Camera();
        }
        all_camera_devices = m_camera->getCameras();
        if (all_camera_devices.empty()) {
            selected_camera_device_name = "";
            selected_camera_device_path = "";
        } else {
            selected_camera_device_name = "";
            for (auto & d : all_camera_devices) {
                if (d.getDevicePath() == selected_camera_device_path) {
                    selected_camera_device_name = d.getFriendlyName();
                    selected_camera_device_path = d.getDevicePath();
                }
            }
            if (selected_camera_device_name.empty()) {
                selected_camera_device_name = all_camera_devices[0].getFriendlyName();
                selected_camera_device_path = all_camera_devices[0].getDevicePath();
            }
        }
    }
    ImGui::SameLine();
    if (!all_camera_devices.empty()) {
        if (ImGui::BeginCombo("Devices", selected_camera_device_name.c_str())) {
            for (auto &device : all_camera_devices) {
                bool is_selected = (device.getDevicePath() == selected_camera_device_path);
                if (ImGui::Selectable(device.getFriendlyName().c_str(), is_selected)) {
                    selected_camera_device_path = device.getDevicePath();
                    selected_camera_device_name = device.getFriendlyName();
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    } else {
        ImGui::Text("No camera device found.");
    }

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
    ImGui::SameLine();
    ImGui::Text("camera fps: %lld", m_camera_fps);

    if (m_img_tex.valid()) {
        auto tex =  m_img_tex.texture();
        if (tex) {
            ImGui::Image((ImTextureID)tex->id(), ImVec2(tex->width()/2, tex->height()/2));
        }
    }

    // if (ImGui::Button("枚举所有摄像头")) {
    //     allCamDevices = znative::CamDevice::enumAllDevices();
    // }
    // for (auto &device : allCamDevices) {
    //     ImGui::NewLine();
    //     ImGui::Button(device.name().c_str());
    //     ImGui::Text("%s", device.toString().c_str());
    // }
    ImGui::End();
}

void WinCamTestWindow::onInvisible(int width, int height) {
    if (m_camera) {
        if (m_camera->isCapturing()) {
            m_camera->StopCapture();
        }
        m_camera->Close();
        delete m_camera;
        m_camera = nullptr;
    }
}