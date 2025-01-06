//
// Created by bigheadson on 2025/1/7.
//

#pragma once

#include "common/Common.h"

NAMESPACE_DEFAULT

class NNTensorDesc : Object {
public:
    explicit NNTensorDesc() {
        m_handle = OH_NNTensorDesc_Create();
        _FATAL_IF(m_handle == nullptr, "OH_NNTensorDesc_Create failed");
    }

    NNTensorDesc(const NNTensorDesc &o): Object(o), m_handle(o.m_handle) {}

    ~NNTensorDesc() {
        if (m_handle) {
            OH_NNTensorDesc_Destroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
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


private:
    NN_TensorDesc *m_handle = nullptr;
};

NAMESPACE_END
