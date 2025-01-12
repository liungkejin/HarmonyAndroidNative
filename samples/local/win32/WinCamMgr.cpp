//
// Created by Kejin on 2025/1/12.
//

#include "WinCamMgr.h"

#include <Windows.h>
#include <uuids.h>
#include <amvideo.h>
#include <strmif.h>
#include <vector>

NAMESPACE_DEFAULT

std::string ConvertBSTRToString(wchar_t* bstr) {
    if (!bstr) {
        return "";
    }
    int size = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, bstr, -1, &result[0], size, nullptr, nullptr);
    return result;
}

class CamInfo {
public:
    void setName(const std::string &name) {
        m_name = name;
    }

    void putResolution(int k, int v) {
        m_res_list.emplace_back(k, v);
    }

private:
    std::string m_name;
    std::vector<std::pair<int, int>> m_res_list;
};

void getCamSupportResolutions(IBaseFilter *pFilter) {
    HRESULT hr = 0;
    std::vector<IPin *> pins;
    IEnumPins *enumPins = nullptr;
    pFilter->EnumPins(&enumPins);

    std::vector<std::pair<int, int>> result;

    while (true) {
        IPin *pin = nullptr;
        hr = enumPins->Next(1, &pin, nullptr);
        if (hr != S_OK) {
            break;
        }
        PIN_INFO pInfo;
        pin->QueryPinInfo(&pInfo);

        IEnumMediaTypes *emt = nullptr;
        pin->EnumMediaTypes(&emt);


        while (true) {
            AM_MEDIA_TYPE *pmt = nullptr;
            hr = emt->Next(1, &pmt, nullptr);
            if (FAILED(hr)) {
                break;
            }

            if ((pmt->formattype == FORMAT_VideoInfo) && (pmt->cbFormat >= sizeof(VIDEOINFOHEADER)) && pmt->pbFormat !=
                                                                                                       nullptr) {
                VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER *) pmt->pbFormat;
                _INFO("VIDEOINFOHEADER: bitrate: %d, source rect[%d, %d, %d, %d], target rect[%d, %d, %d, %d]",
                      pvih->dwBitRate, pvih->rcSource.left, pvih->rcSource.top,
                      pvih->rcSource.right, pvih->rcSource.bottom,
                      pvih->rcTarget.left, pvih->rcTarget.top, pvih->rcTarget.right, pvih->rcTarget.bottom);
                _INFO("VIDEOINFOHEADER: header[size: %d, width: %d, height: %d, planes: %d, bitCount: %d, compression: %d, sizeImage: %d, xPelsPerMeter: %d, yPerlsPerMeter: %d, ClrUsed: %d, ClrImportant: %d]",
                      pvih->bmiHeader.biSize,
                      pvih->bmiHeader.biWidth,
                      pvih->bmiHeader.biHeight,
                      pvih->bmiHeader.biPlanes,
                      pvih->bmiHeader.biBitCount,
                      pvih->bmiHeader.biCompression,
                      pvih->bmiHeader.biSizeImage,
                      pvih->bmiHeader.biXPelsPerMeter,
                      pvih->bmiHeader.biYPelsPerMeter,
                      pvih->bmiHeader.biClrUsed,
                      pvih->bmiHeader.biClrImportant);
            }

//            if (pmt->cbFormat) {
//                CoTaskMemFree((PVOID)pmt->pbFormat);
//                pmt->cbFormat = 0;
//                pmt->pbFormat = nullptr;
//            }
//            if (pmt->pUnk != nullptr) {
//                pmt->pUnk->Release();
//                pmt->pUnk = nullptr;
//            }
        }
//        emt->Release();
    }
}

void WinCamMgr::test() {
    CoInitialize(nullptr);

    HRESULT hr;
    ICreateDevEnum *pDevEnum = nullptr;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                          (void **) &pDevEnum);
    if (FAILED(hr)) {
        _ERROR("CoCreateInstance failed: %d", hr);
        if (pDevEnum) {
            pDevEnum->Release();
        }
        return;
    }

    IEnumMoniker *pEnumCat = nullptr;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
    if (FAILED(hr)) {
        _ERROR("ICreateDevEnum->CreateClassEnumerator failed: %d", hr);
        pDevEnum->Release();
        return;
    }

    IMoniker *pMoniker = nullptr;
    ULONG cFetched;
    int index = 0;
    while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) {
        IPropertyBag *pPropBag;
        hr = pMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void **) &pPropBag);
        if (SUCCEEDED(hr)) {
            IBaseFilter *pFilter = nullptr;
            hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void **) &pFilter);
            if (FAILED(hr)) {
                pPropBag->Release();
                continue;
            }

            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"FriendlyName", &varName, nullptr);
            if (SUCCEEDED(hr)) {
                std::string name = ConvertBSTRToString(varName.bstrVal);

                _INFO("camera[%d] name: %s", index, name);
                getCamSupportResolutions(pFilter);
            }
        }
        pPropBag->Release();
        index += 1;
    }
    pMoniker->Release();
}

NAMESPACE_END