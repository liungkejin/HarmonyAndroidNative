//
// Created by Kejin on 2025/1/12.
//

#include "WinCamMgr.h"

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

NAMESPACE_DEFAULT

// 摄像头设备对象
IMFActivate ** devices = nullptr;
// 底层媒体源对象
IMFMediaSource* source = nullptr;
// 上层数据读取对象
IMFSourceReader * reader = nullptr;

void WinCamMgr::test() {
    CoInitialize(nullptr);

    IMFAttributes *attributes = nullptr;
    IMFMediaType *mediaType = nullptr;

    UINT32 width, height;
    GUID subtype;
    UINT32 count = 0;
    DWORD index = 0, flag = 0;
    LONGLONG timestamp = 0;
    bool exitFlag = false;

    // 枚举设备
    auto hr = MFCreateAttributes(&attributes, 1);
    _FATAL_IF(FAILED(hr), "create attr error: %d", hr)

    hr = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    _FATAL_IF(FAILED(hr), "set guid error: %d", hr)

    hr = MFEnumDeviceSources(attributes, &devices, &count);
    _ERROR_RETURN_IF(FAILED(hr), void(), "MFEnumDeviceSources failed: %d", hr)

    _INFO("Enum device sources count: %d", count);
    hr = MFCreateDeviceSource(devices[0], &source);
    _ERROR_RETURN_IF(FAILED(hr), void(), "MFCreateDeviceSource failed: %d", hr)

    hr = MFCreateSourceReaderFromMediaSource(source, nullptr, &reader);
    _ERROR_RETURN_IF(FAILED(hr), void(), "MFCreateSourceReaderFromMediaSource failed: %d", hr)

    hr = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &mediaType);
    _ERROR_RETURN_IF(FAILED(hr), void(), "GetCurrentMediaType failed: %d", hr)

    // 获取图像格式
    hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
    _INFO("subtype: %s", subtype.Data4);
    // 获取分辨率
    hr = MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height);
    _INFO("frame size: %d x %d", width, height);

    attributes->Release();
    source->Release();
    reader->Release();
    mediaType->Release();
    for (int i = 0; i < count; ++i) {
        devices[i]->Release();
    }
}

NAMESPACE_END