//
// Created by bigheadson on 2025/1/7.
//

#pragma once

#include "common/Common.h"

NAMESPACE_DEFAULT

class NNTensor;
class NNTensorDesc : Object {
    friend class NNTensor;
public:
    explicit NNTensorDesc() {
        m_handle = OH_NNTensorDesc_Create();
        _FATAL_IF(m_handle == nullptr, "OH_NNTensorDesc_Create failed");
    }

    explicit NNTensorDesc(NN_TensorDesc *desc, bool owner = false) {
        m_handle = desc;
        m_owner = owner;
    }

    NNTensorDesc(const NNTensorDesc &o): Object(o), m_handle(o.m_handle), m_owner(o.m_owner) {}

    ~NNTensorDesc() {
        if (m_owner && m_handle && no_reference()) {
            OH_NNTensorDesc_Destroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
    bool valid() const {
        return m_handle != nullptr;
    }

    bool setName(const char *name) {
        auto code = OH_NNTensorDesc_SetName(m_handle, name);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNTensorDesc_SetName failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    std::string getName() {
        const char *name = nullptr;
        auto code = OH_NNTensorDesc_GetName(m_handle, &name);
        _ERROR_IF(code != OH_NN_SUCCESS, "OH_NNTensorDesc_GetName failed, code: %s", HiAIUtils::errStr(code));
        return name;
    }

    bool setDataType(OH_NN_DataType dataType) {
        auto code = OH_NNTensorDesc_SetDataType(m_handle, dataType);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNTensorDesc_SetDataType failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    OH_NN_DataType getDataType() {
        OH_NN_DataType dataType;
        auto code = OH_NNTensorDesc_GetDataType(m_handle, &dataType);
        _ERROR_IF(code != OH_NN_SUCCESS, "OH_NNTensorDesc_GetDataType failed, code: %s", HiAIUtils::errStr(code));
        return dataType;
    }

    bool setShape(const NNShape& shape) {
        auto shapeLength = shape.length();
        const int32_t *shapeArray = shape.shape();
        auto code = OH_NNTensorDesc_SetShape(m_handle, shapeArray, shapeLength);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNTensorDesc_SetShape failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    NNShape getShape() {
        int32_t *shape = nullptr;
        size_t shapeLength = 0;
        auto code = OH_NNTensorDesc_GetShape(m_handle, &shape, &shapeLength);
        _ERROR_IF(code != OH_NN_SUCCESS, "OH_NNTensorDesc_GetShape failed, code: %s", HiAIUtils::errStr(code));
        return {shape, shapeLength};
    }

    bool setFormat(OH_NN_Format format) {
        auto code = OH_NNTensorDesc_SetFormat(m_handle, format);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNTensorDesc_SetFormat(%s) failed, code: %s", HiAIUtils::formatStr(format), HiAIUtils::errStr(code));
        return true;
    }

    OH_NN_Format getFormat() {
        OH_NN_Format format;
        auto code = OH_NNTensorDesc_GetFormat(m_handle, &format);
        _ERROR_IF(code != OH_NN_SUCCESS, "OH_NNTensorDesc_GetFormat failed, code: %s", HiAIUtils::errStr(code));
        return format;
    }

    size_t getElementCount() {
        size_t elementCount = 0;
        auto code = OH_NNTensorDesc_GetElementCount(m_handle, &elementCount);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, 0, "OH_NNTensorDesc_GetElementCount failed, code: %s", HiAIUtils::errStr(code));
        return elementCount;
    }

    size_t getByteSize() {
        size_t byteSize = 0;
        auto code = OH_NNTensorDesc_GetByteSize(m_handle, &byteSize);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, 0, "OH_NNTensorDesc_GetByteSize failed, code: %s",
                         HiAIUtils::errStr(code));
        return byteSize;
    }

private:
    NN_Tensor *create(size_t deviceID) {
        return OH_NNTensor_Create(deviceID, m_handle);
    }

    NN_Tensor *create(size_t deviceID, size_t size) {
        return OH_NNTensor_CreateWithSize(deviceID, m_handle, size);
    }

    NN_Tensor *createWithFd(size_t deviceID, int fd, size_t size, size_t offset) {
        return OH_NNTensor_CreateWithFd(deviceID, m_handle, fd, size, offset);
    }

private:
    NN_TensorDesc *m_handle = nullptr;
    bool m_owner = true;
};

class NNTensor : Object {
public:
    explicit NNTensor(NNTensorDesc &desc, size_t deviceID = 0) {
        m_handle = desc.create(deviceID);
        _ERROR_IF(m_handle == nullptr, "OH_NNTensor_Create failed");
    }

    explicit NNTensor(NN_Tensor *handle = nullptr, bool owner = true) : m_handle(handle), m_owner(owner) {}

    NNTensor(const NNTensor &o) : Object(o), m_handle(o.m_handle), m_owner(o.m_owner) {}

    ~NNTensor() {
        release();
    }

public:
    bool valid() const {
        return m_handle != nullptr;
    }

    NN_Tensor *value() {
        return m_handle;
    }

    bool create(NNTensorDesc &desc, size_t deviceID = 0) {
        release();
        m_handle = desc.create(deviceID);
        _ERROR_IF(m_handle == nullptr, "OH_NNTensor_Create failed");
        m_owner = true;
        return m_handle != nullptr;
    }

    void *getDataBuffer() {
        return OH_NNTensor_GetDataBuffer(m_handle);
    }

    size_t getSize() {
        size_t size = 0;
        auto code = OH_NNTensor_GetSize(m_handle, &size);
        _ERROR_IF(code != OH_NN_SUCCESS, "OH_NNTensor_GetSize failed, code: %s", HiAIUtils::errStr(code));
        return size;
    }

    size_t getOffset() {
        size_t offset = 0;
        auto code = OH_NNTensor_GetOffset(m_handle, &offset);
        _ERROR_IF(code != OH_NN_SUCCESS, "OH_NNTensor_GetOffset failed, code: %s", HiAIUtils::errStr(code));
        return offset;
    }

    void release() {
        if (m_owner && m_handle && no_reference()) {
            OH_NNTensor_Destroy(&m_handle);
            m_handle = nullptr;
        }
        reset_reference();
    }

private:
    NN_Tensor *m_handle = nullptr;
    bool m_owner = false;
};

NAMESPACE_END
