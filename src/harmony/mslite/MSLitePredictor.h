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
#include <mindspore/types.h>

NAMESPACE_DEFAULT

class MSLitePredictor {
public:
    MSLitePredictor() = default;
    
    ~MSLitePredictor() {
        if (m_model) {
            delete m_model;
            m_model = nullptr;
        }
    }

public:
    void setThreadNum(int num) {
        m_thread_num = num;
    }

    void setThreadAffinityMode(int mode) {
        m_thread_affinity_mode = mode;
    }

    // 只对 cpu 有效，如果使用 nnrt 时，设置 parallel 会导致编译失败
    void setParallelEnable(bool enable) {
        m_parallel_enable = enable;
    }

    void setFP16Enable(bool enable) {
        m_fp16_enable = enable;
    }
    
    void setDeviceType(OH_AI_DeviceType deviceType) {
        m_device_type = deviceType;
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
        AIDeviceInfo devInfo;
        if (!devInfo.create(m_device_type)) {
            _ERROR("create device info failed, type: %s", AIUtils::deviceTypeStr(m_device_type));
            return false;
        }
        if (m_device_type != OH_AI_DEVICETYPE_KIRIN_NPU && m_device_type != OH_AI_DEVICETYPE_GPU) {
            devInfo.setF16Enable(m_fp16_enable);
        }
        devInfo.setPerformanceMode(OH_AI_PERFORMANCE_HIGH);

        AIContext context;
        if (m_thread_num > 0) {
            context.setThreadNum(m_thread_num);
        }
        if (m_thread_affinity_mode >= 0) {
            context.setThreadAffinityMode(m_thread_affinity_mode);
        }
        if (m_device_type == OH_AI_DEVICETYPE_CPU) {
            context.setParallelEnable(m_parallel_enable);
        }
        context.addDeviceInfo(devInfo);

        if (m_model) {
            delete m_model;
        }
        m_model = new AIModel();
        auto status = m_model->build(data, size, context.value());
        if (status) {
            _INFO("build failed! \n device context: %s, \n device info: %s", context.toString(), devInfo.toString());
            release();
            return false;
        }
        
        _INFO("build success! \n device context: %s, \n device info: %s", context.toString(), devInfo.toString());
        m_build_flag = true;
        return true;
    }

    bool hasBuilt() const {
        return m_build_flag;
    }

    AITensor getInput(size_t index = 0) {
        _FATAL_IF(!m_build_flag, "model not built");

        AITensorArray inputArr = m_model->getInputs();
        return inputArr[index];
    }

    AITensorArray predict(const OH_AI_KernelCallBack before = nullptr, const OH_AI_KernelCallBack after = nullptr) {
        _FATAL_IF(!m_build_flag, "model not built");

        AITensorArray inputArr = m_model->getInputs();
        return m_model->predict(inputArr, before, after);
    }
    
    void release() {
        m_build_flag = false;

        if (m_model) {
            delete m_model;
            m_model = nullptr;
        }
    }

private:
    OH_AI_DeviceType m_device_type = OH_AI_DEVICETYPE_CPU;
    int m_thread_num = -1;
    int m_fp16_enable = true;
    bool m_parallel_enable = true;
    int m_thread_affinity_mode = -1;

    bool m_build_flag = false;

    AIModel *m_model = nullptr;
};

NAMESPACE_END
