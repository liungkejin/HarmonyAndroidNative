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
#include <local/win32/dshow/DSCaptureMgr.h>

using namespace DirectShowCamera;
using namespace znative;

DSCamDeviceMgr cam_mgr;

bool convert_data_to_rgb24 = false;

ImageTexture m_img_tex;

int64_t m_camera_fps = 0;
int64_t m_camera_frame_count = 0;
int64_t m_camera_start_ms = 0;

void WinCamTestWindow::onVisible(int width, int height) {
    cam_mgr.updateDeviceList();
}

void WinCamTestWindow::onPreRender(int width, int height) {
    if (cam_mgr.isOpened()) {
        auto * frame = cam_mgr.getLatestFrame(true);
        if (frame) {
            // _INFO("getNewFrame: Size(%dx%d), frameSize: %d, frameType: %d, raw frame type: %s",
            //     m_frame.getWidth(), m_frame.getHeight(), m_frame.getFrameSize(), m_frame.getFrameType(), DSUtils::videoTypeString(m_frame.getRawFrameType()));
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

            if (frame->fmt == DSVideoFmt::RGB24) {
                m_img_tex.set(frame->data, frame->width, frame->height, GL_BGR);
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
        cam_mgr.updateDeviceList();
    }
    ImGui::SameLine();

    auto &all_camera_devices = cam_mgr.getAllDevices();
    if (!all_camera_devices.empty()) {
        ImGui::Text("相机设置：");
        ImGui::SameLine();
        static ImGuiComboFlags flags = ImGuiComboFlags_WidthFitPreview;
        if (ImGui::BeginCombo(":", cam_mgr.getCfgName().c_str(), flags)) {
            for (auto &device : all_camera_devices) {
                bool is_selected = (device.name() == cam_mgr.getCfgName());
                if (ImGui::Selectable(device.name().c_str(), is_selected)) {
                    cam_mgr.setDevice(device);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (cam_mgr.currentDevice().valid()) {
            ImGui::SameLine();
            if (ImGui::BeginCombo("|", cam_mgr.getCfgInternalFmtString().c_str(), flags)) {
                auto formats = cam_mgr.getSupportedFormats();
                for (auto &f : formats) {
                    bool is_selected = (f == cam_mgr.getCfgInternalFmt());
                    if (ImGui::Selectable(DSUtils::videoFmtString((int)f).c_str(), is_selected)) {
                        cam_mgr.setCfgInternalFmt(f);
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::BeginCombo(" ", cam_mgr.getCfgSizeString().c_str(), flags)) {
                auto resolutions = cam_mgr.getStreams();
                for (auto &r : resolutions) {
                    int w = r.minWidth();
                    int h = r.minHeight();
                    bool is_selected = (w == cam_mgr.getCfgWidth() && h == cam_mgr.getCfgHeight());
                    if (ImGui::Selectable((std::to_string(w) + "x" + std::to_string(h)).c_str(), is_selected)) {
                        cam_mgr.setCfgSize(w, h);
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
    std::string camLabel = cam_mgr.isOpened() ? "关闭相机" : "打开相机";
    if (ImGui::Button(camLabel.c_str())) {
        if (cam_mgr.isOpened()) {
            cam_mgr.closeCamera();
        } else {
            cam_mgr.openCamera();
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("输出RGB24", &convert_data_to_rgb24);
    if (convert_data_to_rgb24) {
        if (cam_mgr.getCfgDesireFmt() != DSVideoFmt::RGB24) {
            cam_mgr.setCfgDesireFmt(DSVideoFmt::RGB24);
        }
    } else {
        if (cam_mgr.getCfgDesireFmt() != cam_mgr.getCfgInternalFmt()) {
            cam_mgr.setCfgDesireFmt(cam_mgr.getCfgInternalFmt());
        }
    }
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
    //
}