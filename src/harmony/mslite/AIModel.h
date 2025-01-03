//
// Created by LiangKeJin on 2025/1/2.
//

#pragma once

#include "Namespace.h"
#include "common/Object.h"
#include "AITensor.h"
#include "harmony/mslite/AIUtils.h"
#include <vector>

NAMESPACE_DEFAULT

class AITensorArray : Object {
public:
    explicit AITensorArray(OH_AI_TensorHandleArray array) : m_array(array) {}

    AITensorArray(const AITensorArray &o) : m_array(o.m_array), Object(o) {}

public:
    size_t size() const {
        return m_array.handle_num;
    }

    AITensor operator[](size_t index) const {
        if (index >= m_array.handle_num) {
            _ERROR("index out of range: %d", index);
            return AITensor();
        }
        return AITensor(m_array.handle_list[index], false);
    }

    OH_AI_TensorHandleArray& value() {
        return m_array;
    }

private:
    OH_AI_TensorHandleArray m_array;
};


class AIModel : Object {
public:
    AIModel() {
        m_handle = OH_AI_ModelCreate();
        _FATAL_IF(!m_handle, "OH_AI_ModelCreate failed");
    }

    AIModel(const AIModel &o) : m_handle(o.m_handle), Object(o) {}

    ~AIModel() {
        if (no_reference() && m_handle) {
            OH_AI_ModelDestroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
    OH_AI_ModelHandle &value() { return m_handle; }
    
    OH_AI_Status build(const void *data, size_t dataSize, OH_AI_ModelType type, const OH_AI_ContextHandle ctx) {
        auto status = OH_AI_ModelBuild(m_handle, data, dataSize, type, ctx);
        _ERROR_IF(status, "OH_AI_ModelBuild error: %s", AIUtils::statusStr(status));
        return status;
    }

    OH_AI_Status buildFromFile(const char *path, OH_AI_ModelType type, const OH_AI_ContextHandle ctx) {
        auto status = OH_AI_ModelBuildFromFile(m_handle, path, type, ctx);
        _ERROR_IF(status, "OH_AI_ModelBuild error: %s", AIUtils::statusStr(status));
        return status;
    }

    OH_AI_Status resize(std::vector<AITensor> tensors, OH_AI_ShapeInfo *shape_infos, size_t shape_info_num) {
        OH_AI_TensorHandleArray inputs;
        OH_AI_TensorHandle handleList[tensors.size()];
        inputs.handle_num = tensors.size();
        inputs.handle_list = handleList;
        for (int i = 0; i < tensors.size(); i++) {
            handleList[i] = tensors[i].value();
        }
        auto status = OH_AI_ModelResize(m_handle, inputs, shape_infos, shape_info_num);
        _ERROR_IF(status, "OH_AI_ModelResize error: %s", AIUtils::statusStr(status));
        return status;
    }

    AITensorArray getInputs() {
        OH_AI_TensorHandleArray inputs = OH_AI_ModelGetInputs(m_handle);
        return AITensorArray(inputs);
    }

    AITensorArray getOutputs() {
        OH_AI_TensorHandleArray outputs = OH_AI_ModelGetOutputs(m_handle);
        return AITensorArray(outputs);
    }

    AITensorArray predict(AITensorArray& inputs, const OH_AI_KernelCallBack before, const OH_AI_KernelCallBack after) {
        OH_AI_TensorHandleArray outputArray;
        auto status = OH_AI_ModelPredict(m_handle, inputs.value(), &outputArray, before, after);
        _ERROR_IF(status, "OH_AI_ModelPredict error: %s", AIUtils::statusStr(status));
        return AITensorArray(outputArray);
    }

    AITensor getInputByTensorName(const char *name) {
        OH_AI_TensorHandle handle = OH_AI_ModelGetInputByTensorName(m_handle, name);
        return AITensor(handle, false);
    }

    AITensor getOutputByTensorName(const char *name) {
        OH_AI_TensorHandle handle = OH_AI_ModelGetOutputByTensorName(m_handle, name);
        return AITensor(handle, false);
    }

private:
    OH_AI_ModelHandle m_handle;
};

NAMESPACE_END
