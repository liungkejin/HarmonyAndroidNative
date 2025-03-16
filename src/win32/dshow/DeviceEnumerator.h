//
// Created by wutacam on 2025/1/13.
//

#pragma once

#include <common/Common.h>
#include <windows.h>
#include <uuids.h>
#include <strmif.h>
#include <local/win32/WinUtils.h>

NAMESPACE_DEFAULT

typedef std::function<bool(const std::string& id, const std::string& name, IPropertyBag *, IBaseFilter *)> DevEnumCallback;

class DeviceEnumerator {
public:
    static bool enumDevices(REFCLSID category, const DevEnumCallback &callback) {
        ICreateDevEnum *pDevEnum = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr,
            CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **) &pDevEnum);
        if (FAILED(hr)) {
            if (pDevEnum) {
                pDevEnum->Release();
            }
            _ERROR("CoCreateInstance(CLSID_SystemDeviceEnum) failed: %d", hr);
            return false;
        }

        IEnumMoniker *pEnumCat = nullptr;
        hr = pDevEnum->CreateClassEnumerator(category, &pEnumCat, 0);
        if (hr != S_OK) {
            if (pEnumCat) {
                pEnumCat->Release();
            }
            pDevEnum->Release();
            _ERROR("CreateClassEnumerator failed: %d", hr);
            return false;
        }

        IMoniker *pMoniker = nullptr;
        ULONG cFetched;
        int index = 0;
        while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && pMoniker && cFetched) {
            OLECHAR *displayName = nullptr;
            pMoniker->GetDisplayName(nullptr, nullptr, &displayName);
            std::string id = WinUtils::bstrToString(displayName);
            IPropertyBag *pPropBag = nullptr;
            hr = pMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void **) &pPropBag);
            if (SUCCEEDED(hr) && pPropBag) {
                IBaseFilter *pFilter = nullptr;
                hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void **) &pFilter);
                if (SUCCEEDED(hr) && pFilter) {
                    std::string devName;
                    VARIANT varName;
                    VariantInit(&varName);
                    hr = pPropBag->Read(L"FriendlyName", &varName, nullptr);
                    if (SUCCEEDED(hr)) {
                        devName = WinUtils::bstrToString(varName.bstrVal);
                    }
                    VariantClear(&varName);
                    /// callback 返回 true 表示不再继续，直接结束枚举
                    if (callback(id, devName, pPropBag, pFilter)) {
                        if (pFilter) {
                            pFilter->Release();
                        }
                        if (pPropBag) {
                            pPropBag->Release();
                        }
                        pMoniker->Release();
                        break;
                    }
                }

                if (pFilter) {
                    pFilter->Release();
                }
            }

            if (pPropBag) {
                pPropBag->Release();
            }
            pMoniker->Release();
            pMoniker = nullptr;

            index += 1;
        }

        pEnumCat->Release();
        pDevEnum->Release();
        return true;
    }

    /**
     * 枚举所有视频捕获设备
     * @param callback 枚举回调，返回true表示停止枚举
     * @return 是否枚举成功
     */
    static bool enumVideoInputDevices(const DevEnumCallback &callback) {
        return enumDevices(CLSID_VideoInputDeviceCategory, callback);
    }
};

NAMESPACE_END
