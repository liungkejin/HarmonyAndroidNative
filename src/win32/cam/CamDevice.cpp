//
// Created by wutacam on 2025/1/13.
//

#include "CamDevice.h"

#include <amvideo.h>
#include <win32/dshow/DeviceEnumerator.h>

NAMESPACE_DEFAULT

CamDevice CamDevice::getDevice(const std::string &id, const std::string &name, IBaseFilter *filter) {
    CamDevice device(id, name);

    IEnumPins *enumPins = nullptr;
    filter->EnumPins(&enumPins);

    enumPins->Reset();
    IPin *pin = nullptr;
    ULONG pinFetched = 0;
    while (SUCCEEDED(enumPins->Next(1, &pin, &pinFetched)) && pinFetched && pin) {
        IEnumMediaTypes *emt = nullptr;
        pin->EnumMediaTypes(&emt);
        emt->Reset();


        AM_MEDIA_TYPE *pmt = nullptr;
        ULONG pcFetched = 0;
        while (emt->Next(1, &pmt, &pcFetched) == S_OK && pcFetched && pmt) {
            // _INFO("AM_MEDIA_TYPE[majorType: %s, subType: %s]", DSUtils::mediaTypeString(pmt->majortype), WinUtils::guidToString(pmt->subtype));
            if (pmt->formattype == FORMAT_VideoInfo && pmt->cbFormat >= sizeof(VIDEOINFOHEADER) && pmt->pbFormat) {
                VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER *) pmt->pbFormat;

                CamFmt fmt = CamUtils::videoTypeToFmt(pmt->subtype);

                CamStreamInfo *pStreamInfo = device.findStream(fmt);
                if (pStreamInfo == nullptr) {
                    CamStreamInfo streamInfo;
                    streamInfo.m_fmt = fmt;
                    streamInfo.m_planes = pvih->bmiHeader.biPlanes;
                    streamInfo.addCamSize(pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight);
                    device.addStreamInfo(streamInfo);
                } else {
                    pStreamInfo->addCamSize(pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight);
                }
            }
        }
        emt->Release();
    }
    enumPins->Release();

    return device;
}

std::vector<CamDevice> CamDevice::enumAllDevices() {
    std::vector<CamDevice> devices;

    DeviceEnumerator::enumVideoInputDevices([&devices](const std::string &id, const std::string &name, IPropertyBag *, IBaseFilter *filter) {
        auto d = getDevice(id, name, filter);
        devices.push_back(d);
        return false;
    });

    _INFO("Found %d video input devices", devices.size());
    int index = 0;
    for (auto &d : devices) {
        _INFO("Video Input[%d]: %s", index, d.toString());
        index += 1;
    }

    return devices;
}


NAMESPACE_END
