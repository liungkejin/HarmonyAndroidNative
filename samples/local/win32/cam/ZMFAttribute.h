//
// Created by wutacam on 2025/1/12.
//

#pragma once

#include <common/Common.h>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>

NAMESPACE_DEFAULT
// 封装 IMFAttributes
class ZMFAttribute : Object {
public:
    ZMFAttribute() : m_owner(true) {
        HRESULT hr = MFCreateAttributes(&m_attr, 1);
        _FATAL_IF(FAILED(hr), "Failed to create attributes");
    }

    explicit ZMFAttribute(IMFAttributes *attr, bool owner = false) : m_owner(owner), m_attr(attr) {}

    ZMFAttribute(const ZMFAttribute &o) : Object(o), m_owner(o.m_owner), m_attr(o.m_attr) {}

    ~ZMFAttribute() {
        if (m_owner && no_reference() && m_attr) {
            m_attr->Release();
            m_attr = nullptr;
        }
    }

public:
    IMFAttributes *ptr() const { return m_attr; }

    bool setGUID(const GUID& key, const GUID& value) {
        auto hr = m_attr->SetGUID(key, value);
        return SUCCEEDED(hr);
    }

    bool getGUID(const GUID& key, GUID& value) const {
        auto hr = m_attr->GetGUID(key, &value);
        return SUCCEEDED(hr);
    }

    bool setSourceType(const GUID& type) {
        return setGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, type);
    }

private:
    bool m_owner = true;
    IMFAttributes *m_attr = nullptr;
};

NAMESPACE_END
