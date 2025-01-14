//
// Created by wutacam on 2025/1/14.
//

#include "WinCamTestWindow.h"
#include <windows.h>
#include <vector>
#include <local/win32/cam/CamDevice.h>

std::vector<znative::CamDevice> allCamDevices;

void WinCamTestWindow::onRenderImgui(int width, int height, ImGuiIO &io) {
    ImGui::Begin("Win32 Camera Test");
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
