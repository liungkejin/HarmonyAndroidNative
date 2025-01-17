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

struct CamOpenCfg {
    DirectShowCameraDevice curDevice;
    DirectShowVideoFormat videoFormat;

    void choose(const DirectShowCameraDevice& dev) {
        curDevice = dev;

        // 选择第一个支持的分辨率
        std::vector<DirectShowVideoFormat> formats = dev.getAllSupportedUniqueVideoFormats();
        // 默认选择第一个有效的格式
        if (formats.empty()) {
            _FATAL("No valid video format found for camera: %s", name());
        }
        std::vector<DirectShowVideoFormat> resolutions = dev.getAllResolutionOfFormat(formats[0].getVideoType());
        if (resolutions.empty()) {
            _FATAL("No valid resolution found for camera: %s", name());
        }

        videoFormat = resolutions[0];
    }

    void chooseFormat(const DirectShowVideoFormat& fmt) {
        auto allResolutions = curDevice.getAllResolutionOfFormat(fmt.getVideoType());
        if (allResolutions.empty()) {
            _FATAL("No valid resolution found for camera: %s, type: %s", name(), fmt.getVideoTypeString());
        }
        videoFormat = allResolutions[0];
    }

    void chooseResolution(const DirectShowVideoFormat& fmt) {
        videoFormat = fmt;
    }

    bool valid() {
        return curDevice.valid() && videoFormat.valid();
    }

    // std::string id() {
    //     return curDevice.getDevicePath();
    // }

    std::string name() {
        return curDevice.getFriendlyName();
    }

    int width() {
        return videoFormat.getWidth();
    }

    int height() {
        return videoFormat.getHeight();
    }

    std::string sizeStr() {
        return std::to_string(width()) + "x" + std::to_string(height());
    }

    void clear() {
        curDevice = DirectShowCameraDevice();
    }
};

std::vector<DirectShowCameraDevice> all_camera_devices;
CamOpenCfg cam_open_cfg;
bool convert_data_to_rgb24 = false;

Camera *m_camera = nullptr;
Camera * getCamera() {
    if (m_camera == nullptr) {
        m_camera = new Camera();
    }
    return m_camera;
}

Frame m_frame;

ImageTexture m_img_tex;

int64_t m_camera_fps = 0;
int64_t m_camera_frame_count = 0;
int64_t m_camera_start_ms = 0;

void WinCamTestWindow::onVisible(int width, int height) {
    all_camera_devices = getCamera()->getDirectShowCameras();
    for (auto &dev : all_camera_devices) {
        _INFO("Camera device: %s", dev);
    }
    if (!all_camera_devices.empty()) {
        auto & first = all_camera_devices[0];
        cam_open_cfg.choose(first);
    } else {
        cam_open_cfg.clear();
    }
}

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

            if (m_frame.getRawFrameType() == MEDIASUBTYPE_RGB24) {
                int bytes = 0;
                auto data = m_frame.getFrameData(bytes, false, true);
                m_img_tex.set(data.get(), m_frame.getWidth(), m_frame.getHeight(), GL_BGR);
            }
        }
    }
}

void WinCamTestWindow::onRenderImgui(int width, int height, ImGuiIO &io) {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Win32 Camera Test", 0, window_flags);
    ImGui::SetWindowSize(ImVec2(width-300, height));
    ImGui::SetWindowPos(ImVec2(300, 0));

    if (ImGui::Button("刷新")) {
        all_camera_devices = m_camera->getDirectShowCameras();
        if (all_camera_devices.empty()) {
            cam_open_cfg.clear();
        } else {
            DirectShowCameraDevice *selectedDev = nullptr;
            for (auto &dev : all_camera_devices) {
                if (dev.getFriendlyName() == cam_open_cfg.name()) {
                    selectedDev = &dev;
                    break;
                }
            }
            if (selectedDev) {
                cam_open_cfg.choose(*selectedDev);
            } else {
                cam_open_cfg.choose(all_camera_devices[0]);
            }
        }
    }
    ImGui::SameLine();
    if (!all_camera_devices.empty()) {
        ImGui::Text("相机设置：");
        ImGui::SameLine();
        static ImGuiComboFlags flags = ImGuiComboFlags_WidthFitPreview;
        if (ImGui::BeginCombo(":", cam_open_cfg.name().c_str(), flags)) {
            for (auto &device : all_camera_devices) {
                bool is_selected = (device.getFriendlyName() == cam_open_cfg.name());
                if (ImGui::Selectable(device.getFriendlyName().c_str(), is_selected)) {
                    cam_open_cfg.choose(device);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (cam_open_cfg.valid()) {
            ImGui::SameLine();
            if (ImGui::BeginCombo("|", cam_open_cfg.videoFormat.getVideoTypeString().c_str(), flags)) {
                auto formats = cam_open_cfg.curDevice.getAllSupportedUniqueVideoFormats();
                for (auto &f : formats) {
                    bool is_selected = (f.getVideoType() == cam_open_cfg.videoFormat.getVideoType());
                    if (ImGui::Selectable(f.getVideoTypeString().c_str(), is_selected)) {
                        cam_open_cfg.chooseFormat(f);
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::BeginCombo(" ", cam_open_cfg.sizeStr().c_str(), flags)) {
                auto resolutions = cam_open_cfg.curDevice.getAllResolutionOfFormat(cam_open_cfg.videoFormat.getVideoType());
                for (auto &r : resolutions) {
                    bool is_selected = (r.getWidth() == cam_open_cfg.width() && r.getHeight() == cam_open_cfg.height());
                    if (ImGui::Selectable((std::to_string(r.getWidth()) + "x" + std::to_string(r.getHeight())).c_str(), is_selected)) {
                        cam_open_cfg.chooseResolution(r);
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
    } else {
        ImGui::Text("No camera device found.");
    }
    std::string camLabel = ((getCamera()->isOpened() || getCamera()->isCapturing()) ? "关闭相机" : "打开相机");
    if (ImGui::Button(camLabel.c_str())) {
        if (getCamera()->isOpened() || getCamera()->isCapturing()) {
            if (getCamera()->isCapturing()) {
                getCamera()->StopCapture();
            }
            getCamera()->Close();
        } else {
            if (cam_open_cfg.valid()) {
                bool success = getCamera()->Open(cam_open_cfg.curDevice, cam_open_cfg.videoFormat, convert_data_to_rgb24);
                _INFO("open camera: %s\nformat: %s", cam_open_cfg.curDevice, cam_open_cfg.videoFormat);
                if (success) {
                    m_camera->StartCapture();
                }
            }
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("输出RGB24", &convert_data_to_rgb24);
    ImGui::SameLine();
    ImGui::Text("camera fps: %lld", m_camera_fps);

    if (m_img_tex.valid()) {
        auto tex =  m_img_tex.texture();
        if (tex) {
            ImGui::Image((ImTextureID)tex->id(), ImVec2(tex->width(), tex->height()));
        }
    }

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