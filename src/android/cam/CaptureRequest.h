//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <unordered_map>
#include <camera/NdkCameraDevice.h>
#include "Namespace.h"
#include "common/Object.h"
#include "CamCharacteristics.h"

NAMESPACE_DEFAULT

typedef ACameraDevice_request_template CamTemplate;

class CamOutputTarget : Object {
public:
    explicit CamOutputTarget(ACameraWindowType *window) {
        auto status = ACameraOutputTarget_create(window, &m_target);
        _FATAL_IF(status, "ACameraOutputTarget_create failed: %s", CamUtils::errString(status));
    }

    CamOutputTarget(const CamOutputTarget &o) : m_target(o.m_target), Object(o) {}

    ~CamOutputTarget() {
        if (no_reference() && m_target) {
            ACameraOutputTarget_free(m_target);
            m_target = nullptr;
        }
    }

public:
    const ACameraOutputTarget *value() const {
        return m_target;
    }

private:
    ACameraOutputTarget *m_target = nullptr;
};

class CaptureRequest : Object {
public:
    static CamMetadataEntry getConstEntry(const ACaptureRequest *request, uint32_t tag) {
        auto *entry = new ACameraMetadata_const_entry();
        auto status = ACaptureRequest_getConstEntry(request, tag, entry);
        if (status != ACAMERA_OK) {
            delete entry;
            entry = nullptr;
            _ERROR("ACaptureRequest_getConstEntry failed: %s", CamUtils::errString(status));
        }
        return CamMetadataEntry(entry);
    }

public:
    explicit CaptureRequest(ACaptureRequest *request) : m_request(request) {}

    CaptureRequest(const CaptureRequest &o) : m_request(o.m_request), Object(o) {}

    ~CaptureRequest() {
        if (no_reference() && m_request) {
            for (auto &i: m_targets) {
                ACaptureRequest_removeTarget(m_request, i.second.value());
            }
            m_targets.clear();
            ACaptureRequest_free(m_request);
            m_request = nullptr;
        }
    }

public:
    bool valid() const {
        return m_request != nullptr;
    }

    ACaptureRequest *value() {
        return m_request;
    }

    CamStatus addTarget(ACameraWindowType *window) {
        CamOutputTarget target(window);
        auto status = ACaptureRequest_addTarget(m_request, target.value());
        _ERROR_RETURN_IF(status, status, "ACaptureRequest_addTarget failed: %s", CamUtils::errString(status));
        this->m_targets.insert(std::make_pair(window, target));
        return status;
    }

    CamStatus removeTarget(ACameraWindowType *window) {
        auto target = m_targets.find(window);
        if (target == m_targets.end()) {
            return ACAMERA_ERROR_INVALID_PARAMETER;
        }
        m_targets.erase(target);
        auto status = ACaptureRequest_removeTarget(m_request, target->second.value());
        _ERROR_RETURN_IF(status, status, "ACaptureRequest_removeTarget failed: %s", CamUtils::errString(status));
        return status;
    }

public:
    CamMetadataEntry getEntry(uint32_t tag) {
        return CaptureRequest::getConstEntry(m_request, tag);
    }

    void setEntry_u8(uint32_t tag, const uint8_t data) {
        this->setEntry_u8(tag, 1, &data);
    }

    void setEntry_u8(uint32_t tag, uint32_t count, const uint8_t *data) {
        auto status = ACaptureRequest_setEntry_u8(m_request, tag, count, data);
        _ERROR_IF(status, "ACaptureRequest_setEntry_u8 tag(%s) failed: %s", CamUtils::tagString(tag), CamUtils::errString(status));
    }

    void setEntry_i32(uint32_t tag, const int32_t data) {
        this->setEntry_i32(tag, 1, &data);
    }

    void setEntry_i32(uint32_t tag, uint32_t count, const int32_t *data) {
        auto status = ACaptureRequest_setEntry_i32(m_request, tag, count, data);
        _ERROR_IF(status, "ACaptureRequest_setEntry_i32 tag(%s) failed: %s", CamUtils::tagString(tag), CamUtils::errString(status));
    }

    void setEntry_float(uint32_t tag, const float data) {
        this->setEntry_float(tag, 1, &data);
    }

    void setEntry_float(uint32_t tag, uint32_t count, const float *data) {
        auto status = ACaptureRequest_setEntry_float(m_request, tag, count, data);
        _ERROR_IF(status, "ACaptureRequest_setEntry_float tag(%s) failed: %s", CamUtils::tagString(tag), CamUtils::errString(status));
    }

    void setEntry_i64(uint32_t tag, const int64_t data) {
        this->setEntry_i64(tag, 1, &data);
    }

    void setEntry_i64(uint32_t tag, uint32_t count, const int64_t *data) {
        auto status = ACaptureRequest_setEntry_i64(m_request, tag, count, data);
        _ERROR_IF(status, "ACaptureRequest_setEntry_i64 tag(%s) failed: %s", CamUtils::tagString(tag), CamUtils::errString(status));
    }

    void setEntry_double(uint32_t tag, const double data) {
        this->setEntry_double(tag, 1, &data);
    }

    void setEntry_double(uint32_t tag, uint32_t count, const double *data) {
        auto status = ACaptureRequest_setEntry_double(m_request, tag, count, data);
        _ERROR_IF(status, "ACaptureRequest_setEntry_double tag(%s) failed: %s", CamUtils::tagString(tag), CamUtils::errString(status));
    }

    void setEntry_rational(uint32_t tag, const ACameraMetadata_rational data) {
        this->setEntry_rational(tag, 1, &data);
    }

    void setEntry_rational(uint32_t tag, uint32_t count, const ACameraMetadata_rational *data) {
        auto status = ACaptureRequest_setEntry_rational(m_request, tag, count, data);
        _ERROR_IF(status, "ACaptureRequest_setEntry_rational tag(%s) failed: %s", CamUtils::tagString(tag), CamUtils::errString(status));
    }

private:
    ACaptureRequest *m_request;

    std::unordered_map<ACameraWindowType *, CamOutputTarget> m_targets;
};

NAMESPACE_END
