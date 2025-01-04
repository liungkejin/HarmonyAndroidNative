//
// Created on 2025/1/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma

#include "AIContext.h"
#include "AITensor.h"
#include "AIModel.h"
#include "common/AppContext.h"

NAMESPACE_DEFAULT

class MindSporeLite {
public:
    explicit MindSporeLite() {}

public:
    AIDeviceInfo &deviceInfo() {
        return m_dev_info;
    }

    AIContext &context() {
        return m_context;
    }

    bool buildModel(const char *path) {
        RawData data;
        if (strncmp(path, "assets://", 9) == 0) {
            path += 9;
            data = AppContext::assetsManager()->readAll(path);
        } else {
            data = FileUtils::read(path);
        }

        if (data.size() == 0) {
            _ERROR("read model file failed: %s", path);
            return false;
        }
        
        m_context.addDeviceInfo(m_dev_info);
        return m_model.build(data.data(), data.size(), m_context.value()) == OH_AI_STATUS_SUCCESS;
    }

    AITensorArray getInputs() {
        return m_model.getInputs();
    }

    AITensorArray predict(AITensorArray &inputs,
                          const OH_AI_KernelCallBack before = nullptr,
                          const OH_AI_KernelCallBack after = nullptr) {
        return m_model.predict(inputs, before, after);
    }

private:

    AIModel m_model;
    AIDeviceInfo m_dev_info;
    AIContext m_context;
};

NAMESPACE_END
