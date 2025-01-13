//
// Created by Kejin on 2025/1/12.
//

#include "WinCamMgr.h"

#include <Windows.h>
#include <local/win32/cam/CamDevice.h>

NAMESPACE_DEFAULT

void WinCamMgr::test() {
    CoInitialize(nullptr);

    CamDevice::enumAllDevices();
}

NAMESPACE_END
