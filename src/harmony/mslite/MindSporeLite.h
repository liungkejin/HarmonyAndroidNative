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
    void setDeviceInfo(OH_AI_DeviceType type = OH_AI_DEVICETYPE_CPU, bool enableFP16 = false) {
        auto deviceInfo = AIDeviceInfo(type);
        deviceInfo.setF16Enable(enableFP16);
        m_context.addDeviceInfo(deviceInfo);
    }

    AIContext &context() {
        return m_context;
    }

    bool buildModel(const char *path, OH_AI_ModelType type) {
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

        return m_model.build(data.data(), data.size(), type, m_context.value()) == OH_AI_STATUS_SUCCESS;
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
    AIContext m_context;
};

NAMESPACE_END
