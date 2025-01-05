//
// Created on 2025/1/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include "AIContext.h"
#include "AITensor.h"
#include "AIModel.h"
#include "common/AppContext.h"

NAMESPACE_DEFAULT

class MSLitePredictor {
public:
    explicit MSLitePredictor(bool fp16Enable = true, OH_AI_DeviceType deviceType = OH_AI_DEVICETYPE_CPU)
        : m_device_type(deviceType) {
        m_dev_info.create(deviceType);
        m_dev_info.setF16Enable(fp16Enable);
    }

public:
    void setThreadNum(int num) {
        _WARN_RETURN_IF(m_dev_added, void(), "already build, setThreadNum ignored!")
        m_context.setThreadNum(num);
    }

    void setThreadAffinityMode(int mode) {
        _WARN_RETURN_IF(m_dev_added, void(), "already build, setThreadAffinityMode ignored!")
        m_context.setThreadAffinityMode(mode);
    }

    void setParallelEnable(bool enable) {
        _WARN_RETURN_IF(m_dev_added, void(), "already build, setParallelEnable ignored!")
        m_context.setParallelEnable(enable);
    }

    void setFP16Enable(bool enable) {
        _WARN_RETURN_IF(m_dev_added, void(), "already build, setFP16Enable ignored!");
        m_dev_info.setF16Enable(enable);
    }

    bool buildFromFile(const char *path) {
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

        return build(data.data(), data.size());
    }

    bool build(const uint8_t *data, size_t size) {
        if (m_build_flag) {
            _FATAL("already built");
        }

        if (!m_dev_added) {
            m_dev_added = true;
            m_context.addDeviceInfo(m_dev_info);
        }
        auto status = m_model.build(data, size, m_context.value());
        if (status) {
            return false;
        }

        m_build_flag = true;
        return true;
    }

    bool hasBuilt() const {
        return m_build_flag;
    }

    AITensor getInput(size_t index = 0) {
        _FATAL_IF(!m_build_flag, "model not built");

        AITensorArray inputArr = m_model.getInputs();
        return inputArr[index];
    }

    AITensorArray predict(const OH_AI_KernelCallBack before = nullptr, const OH_AI_KernelCallBack after = nullptr) {
        _FATAL_IF(!m_build_flag, "model not built");

        AITensorArray inputArr = m_model.getInputs();
        return m_model.predict(inputArr, before, after);
    }

private:
    OH_AI_DeviceType m_device_type;

    bool m_dev_added = false;
    bool m_build_flag = false;

    AIModel m_model;
    AIContext m_context;
    AIDeviceInfo m_dev_info;
};

NAMESPACE_END
