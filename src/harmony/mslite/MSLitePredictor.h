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
        if (m_context) {
            delete m_context;
            m_context = nullptr;
        }
        if (m_dev_info) {
            delete m_dev_info;
            m_dev_info = nullptr;
        }
    }

public:
    void setThreadNum(int num) {
        m_thread_num = num;
    }

    void setThreadAffinityMode(int mode) {
        m_thread_affinity_mode = mode;
    }

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
        if (m_model) {
            delete m_model;
        }
        m_model = new AIModel();
        
        if (m_dev_info) {
            delete m_dev_info;
        }
        m_dev_info = new AIDeviceInfo();
        if (!m_dev_info->create(m_device_type)) {
            _ERROR("create device info failed, type: %s", AIUtils::deviceTypeStr(m_device_type));
            delete m_dev_info;
            m_dev_info = nullptr;
            return false;
        }
        if (m_device_type != OH_AI_DEVICETYPE_KIRIN_NPU && m_device_type != OH_AI_DEVICETYPE_GPU) {
            m_dev_info->setF16Enable(m_fp16_enable);
        }
        m_dev_info->setPerformanceMode(OH_AI_PERFORMANCE_EXTREME);
        
        if (m_context) {
            delete m_context;
        }
        m_context = new AIContext();
        if (m_thread_num > 0) {
            m_context->setThreadNum(m_thread_num);
        }
        if (m_thread_affinity_mode >= 0) {
            m_context->setThreadAffinityMode(m_thread_affinity_mode);
        }
        m_context->setParallelEnable(m_parallel_enable);
        m_context->addDeviceInfo(*m_dev_info);
        
        auto status = m_model->build(data, size, m_context->value());
        if (status) {
            _INFO("build failed! \n device context: %s, \n device info: %s", m_context->toString(), m_dev_info->toString());
            release();
            return false;
        }
        
        _INFO("build success! \n device context: %s, \n device info: %s", m_context->toString(), m_dev_info->toString());
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

        // fixme 释放 context 和 device info 会崩溃
//        if (m_context) {
//            delete m_context;
//            m_context = nullptr;
//        }
//        if (m_dev_info) {
//            delete m_dev_info;
//            m_dev_info = nullptr;
//        }
        if (m_model) {
            delete m_model;
            m_model = nullptr;
            m_context = nullptr;
            m_dev_info = nullptr;
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
    AIContext *m_context = nullptr;
    AIDeviceInfo *m_dev_info = nullptr;
};

NAMESPACE_END
