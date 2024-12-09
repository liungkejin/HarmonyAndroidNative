//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <vector>
#include <string>
#include <camera/NdkCameraMetadata.h>
#include "Namespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include "CamUtils.h"

NAMESPACE_DEFAULT

class CamMetadataEntry : Object {
public:
    explicit CamMetadataEntry(ACameraMetadata_const_entry *entry) : m_entry(entry) {}

    CamMetadataEntry(const CamMetadataEntry &o) : m_entry(o.m_entry), Object(o) {}

    ~CamMetadataEntry() {
        if (no_reference() && m_entry) {
            delete m_entry;
            m_entry = nullptr;
        }
    }

public:

    bool valid() const {
        return m_entry != nullptr;
    }

    uint32_t tag() const {
        return m_entry->tag;
    }

    uint32_t dataCount() const {
        return m_entry->count;
    }

    uint8_t dataU8(int i = 0) const {
        _FATAL_IF(m_entry->type != ACAMERA_TYPE_BYTE, "data type is not ACAMERA_TYPE_BYTE")
        return m_entry->data.u8[i];
    }

    int32_t dataI32(int i = 0) const {
        _FATAL_IF(m_entry->type != ACAMERA_TYPE_INT32, "data type is not ACAMERA_TYPE_INT32")
        return m_entry->data.i32[i];
    }

    float dataF(int i = 0) const {
        _FATAL_IF(m_entry->type != ACAMERA_TYPE_FLOAT, "data type is not ACAMERA_TYPE_FLOAT")
        return m_entry->data.f[i];
    }

    int64_t dataI64(int i = 0) const {
        _FATAL_IF(m_entry->type != ACAMERA_TYPE_INT64, "data type is not ACAMERA_TYPE_INT64")
        return m_entry->data.i64[i];
    }

    double dataD(int i = 0) const {
        _FATAL_IF(m_entry->type != ACAMERA_TYPE_DOUBLE, "data type is not ACAMERA_TYPE_DOUBLE")
        return m_entry->data.d[i];
    }

    ACameraMetadata_rational dataRational(int i = 0) const {
        _FATAL_IF(m_entry->type != ACAMERA_TYPE_RATIONAL, "data type is not ACAMERA_TYPE_RATIONAL")
        return m_entry->data.r[i];
    }

private:
    ACameraMetadata_const_entry *m_entry = nullptr;
};

/**
 * 封装 ACameraMetadata
 */
class CamCharacteristics : Object {
public:
    static CamMetadataEntry getConstEntry(const ACameraMetadata *metadata, uint32_t tag) {
        auto *entry = new ACameraMetadata_const_entry();
        auto error = ACameraMetadata_getConstEntry(metadata, tag, entry);
        if (error) {
            delete entry;
            entry = nullptr;
            _ERROR("ACameraMetadata_getConstEntry failed: %s", CamUtils::errString(error));
        }
        return CamMetadataEntry(entry);
    }

public:
    explicit CamCharacteristics(ACameraMetadata *metadata) : m_metadata(metadata) {}

    CamCharacteristics(const CamCharacteristics &o) : m_metadata(o.m_metadata), Object(o) {}

    ~CamCharacteristics() {
        if (no_reference() && m_metadata) {
            ACameraMetadata_free(m_metadata);
            m_metadata = nullptr;
        }
    }

public:
    bool valid() {
        return m_metadata != nullptr;
    }

    CamCharacteristics copy() {
        ACameraMetadata *metadata = ACameraMetadata_copy(m_metadata);
        _FATAL_IF(metadata == nullptr, "ACameraMetadata_copy failed")
        return CamCharacteristics(metadata);
    }

    CamMetadataEntry getEntry(uint32_t tag) const {
        return getConstEntry(m_metadata, tag);
    }

    std::vector<uint32_t> getAllTags() const {
        int32_t count = 0;
        uint32_t *tags = nullptr;
        auto error = ACameraMetadata_getAllTags(m_metadata, &count, (const uint32_t **) &tags);
        if (error) {
            _ERROR("ACameraMetadata_getAllTags failed: %s", CamUtils::errString(error));
            return {};
        }
        std::vector<uint32_t> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(tags[i]);
        }
        return result;
    }

    bool isLogicalMultiCam() const {
        auto isLogicalMultiCam = ACameraMetadata_isLogicalMultiCamera(
                m_metadata, nullptr, nullptr);
        return isLogicalMultiCam;
    }

    std::vector<std::string> getLogicalMultiCamIds() const {
        std::vector<std::string> result;
        size_t numLogicalCameras = 0;
        const char **logicalCamIds = nullptr;
        auto isLogicalMultiCam = ACameraMetadata_isLogicalMultiCamera(m_metadata, &numLogicalCameras, (const char* const**)&logicalCamIds);
        if (isLogicalMultiCam) {
            for (size_t i = 0; i < numLogicalCameras; ++i) {
                result.emplace_back(logicalCamIds[i]);
            }
        }
        return result;
    }

public:

private:
    ACameraMetadata *m_metadata;
};

NAMESPACE_END
