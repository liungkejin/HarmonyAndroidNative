//
// Created on 2025/1/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "common/Common.h"
#include "common/Object.h"
#include "AIUtils.h"
#include <cstddef>
#include <cstdint>
#include <mindspore/format.h>
#include <mindspore/tensor.h>

NAMESPACE_DEFAULT

class AITensor : Object {
public:
    explicit AITensor(OH_AI_TensorHandle handle = nullptr, bool owner = false) : m_handle(handle), m_owner(owner) {}

    AITensor(const AITensor &o) : m_handle(o.m_handle), m_owner(o.m_owner), Object(o) {}

    ~AITensor() {
        if (no_reference() && m_owner && m_handle) {
            OH_AI_TensorDestroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
    OH_AI_TensorHandle value() const { return m_handle; }

    void create(const char *name, OH_AI_DataType type, const int64_t *shape, size_t shape_num, const void *data,
                size_t data_len) {
        if (m_handle != nullptr) {
            _WARN("tensor already created! release previous");
            release();
        }
        m_handle = OH_AI_TensorCreate(name, type, shape, shape_num, data, data_len);
    }

    AITensor clone() {
        _FATAL_IF(!m_handle, "AITensor not create!");
        auto handle = OH_AI_TensorClone(m_handle);
        return AITensor(handle, true);
    }

    void setName(const char *name) {
        _FATAL_IF(!m_handle, "AITensor not create!");
        OH_AI_TensorSetName(m_handle, name);
    }

    const char *getName() {
        _FATAL_IF(!m_handle, "AITensor not create!");
        return OH_AI_TensorGetName(m_handle);
    }

    void setDataType(OH_AI_DataType type) {
        _FATAL_IF(!m_handle, "AITensor not create!");
        OH_AI_TensorSetDataType(m_handle, type);
    }

    OH_AI_DataType getDataType() {
        _FATAL_IF(!m_handle, "AITensor not create!");
        return OH_AI_TensorGetDataType(m_handle);
    }

    void setShape(const int64_t *shape, size_t shapeNum) {
        _FATAL_IF(!m_handle, "AITensor not create!");
        OH_AI_TensorSetShape(m_handle, shape, shapeNum);
    }

    const int64_t *getShape(size_t &outShapeNum) {
        _FATAL_IF(!m_handle, "AITensor not create!");
        return OH_AI_TensorGetShape(m_handle, &outShapeNum);
    }

    void setFormat(OH_AI_Format format) { OH_AI_TensorSetFormat(m_handle, format); }

    OH_AI_Format getFormat() { return OH_AI_TensorGetFormat(m_handle); }

    void setData(void *data) { OH_AI_TensorSetData(m_handle, data); }

    const void *getData() { return OH_AI_TensorGetData(m_handle); }

    void *getMutableData() { return OH_AI_TensorGetMutableData(m_handle); }

    int64_t getElementNum() { return OH_AI_TensorGetElementNum(m_handle); }

    size_t getDataSize() { return OH_AI_TensorGetDataSize(m_handle); }

    OH_AI_Status setUserData(void *data, size_t size) {
        auto status = OH_AI_TensorSetUserData(m_handle, data, size);
        _ERROR_IF(status, "OH_AI_TensorSetUserData failed: %s", AIUtils::statusStr(status));
        return status;
    }

    void release() {
        if (m_handle) {
            OH_AI_TensorDestroy(&m_handle);
            m_handle = nullptr;
        }
    }

    std::string toString() {
        std::stringstream ss;
        ss << "AITensor(" << getName() << ") {\n";
        ss << "data type: " << AIUtils::dataTypeStr(getDataType()) << ", \n";
        ss << "shape: [";
        size_t shapeNum;
        const int64_t *shape = getShape(shapeNum);
        for (size_t i = 0; i < shapeNum; i++) {
            ss << shape[i];
            if (i < shapeNum - 1) {
                ss << ", ";
            }
        }
        ss << "], \n";
        ss << "format: " << AIUtils::formatStr(getFormat()) << ", \n";
        ss << "data_size: " << getDataSize() << ", \n";
        ss << "element_num: " << getElementNum() << "\n}";
        return ss.str();
    }

private:
    bool m_owner;
    OH_AI_TensorHandle m_handle;
};

NAMESPACE_END
