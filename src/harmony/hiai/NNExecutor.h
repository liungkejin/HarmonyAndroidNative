//
// Created by bigheadson on 2025/1/7.
//

#pragma once

#include "common/Common.h"
#include "HiAI.h"
#include <neural_network_runtime/neural_network_core.h>
#include <vector>

NAMESPACE_DEFAULT

class NNDevice {
public:
    static std::vector<size_t> getAllDevicesID() {
        const size_t *ids = nullptr;
        uint32_t count = 0;
        OH_NN_ReturnCode code = OH_NNDevice_GetAllDevicesID(&ids, &count);
        if (code != OH_NN_SUCCESS) {
            _ERROR("OH_NNDevice_GetAllDevicesID failed, code: %s", HiAIUtils::errStr(code));
            return {};
        }
        std::vector<size_t> result(ids, ids + count);
        return result;
    }

    static std::string deviceName(size_t id) {
        const char *name = nullptr;
        auto code = OH_NNDevice_GetName(id, &name);
        if (code != OH_NN_SUCCESS) {
            _ERROR("OH_NNDevice_GetName failed, code: %s", HiAIUtils::errStr(code));
            return "";
        }
        return name;
    }

    // 返回 -1 表示失败
    static int deviceType(size_t id) {
        OH_NN_DeviceType type;
        auto code = OH_NNDevice_GetType(id, &type);
        if (code != OH_NN_SUCCESS) {
            _ERROR("OH_NNDevice_GetType failed, code: %s", HiAIUtils::errStr(code));
            return -1;
        }
        return type;
    }

    static void dumpAllDevices() {
        auto ids = getAllDevicesID();
        _INFO("dump all devices, count: %d", ids.size());
        for (auto id : ids) {
            _INFO("device id: %d, name: %s, type: %s", id, deviceName(id), HiAIUtils::deviceTypeStr(deviceType(id)));
        }
    }
};

class NNExecutor;
class NNCompilation {
    friend class NNExecutor;
public:
    explicit NNCompilation() {}

    ~NNCompilation() {
        this->release();
    }

public:
    bool createFromModelFile(const char *modelPath) {
        this->release();
        m_handle = OH_NNCompilation_ConstructWithOfflineModelFile(modelPath);
        _ERROR_IF(!m_handle, "OH_NNCompilation_ConstructWithOfflineModelFile failed, model file: %s", modelPath);
        return m_handle != nullptr;
    }

    bool createFromModelBuffer(const void *modelBuffer, size_t size) {
        this->release();
        m_handle = OH_NNCompilation_ConstructWithOfflineModelBuffer(modelBuffer, size);
        _ERROR_IF(!m_handle, "OH_NNCompilation_ConstructWithOfflineModelBuffer failed, model buffer size: %s", size);
        return m_handle != nullptr;
    }

    bool setDevice(size_t deviceId) {
        _FATAL_IF(!m_handle, "OH_NNCompilation not create!");
        _FATAL_IF(m_has_built, "OH_NNCompilation has built!");
        auto code = OH_NNCompilation_SetDevice(m_handle, deviceId);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNCompilation_SetDevice(%d) failed, code: %s", deviceId, HiAIUtils::errStr(code));
        return true;
    }

    bool setPerformanceMode(OH_NN_PerformanceMode mode) {
        _FATAL_IF(!m_handle, "OH_NNCompilation not create!");
        _FATAL_IF(m_has_built, "OH_NNCompilation has built!");
        auto code = OH_NNCompilation_SetPerformanceMode(m_handle, mode);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNCompilation_SetPerformanceMode(%s) failed, code: %s", HiAIUtils::performanceModeStr(mode), HiAIUtils::errStr(code));
        return true;
    }

    bool setPriority(OH_NN_Priority priority) {
        _FATAL_IF(!m_handle, "OH_NNCompilation not create!");
        _FATAL_IF(m_has_built, "OH_NNCompilation has built!");
        auto code = OH_NNCompilation_SetPriority(m_handle, priority);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNCompilation_SetPriority(%s) failed, code: %s", HiAIUtils::priorityStr(priority), HiAIUtils::errStr(code));
        return true;
    }

    bool enableFloat16(bool enable) {
        _FATAL_IF(!m_handle, "OH_NNCompilation not create!");
        _FATAL_IF(m_has_built, "OH_NNCompilation has built!");
        auto code = OH_NNCompilation_EnableFloat16(m_handle, enable);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNCompilation_EnableFloat16(%d) failed, code: %s", enable, HiAIUtils::errStr(code));
        return true;
    }

    bool build() {
        _FATAL_IF(!m_handle, "OH_NNCompilation not create!");
        _FATAL_IF(m_has_built, "OH_NNCompilation has built!");
        auto code = OH_NNCompilation_Build(m_handle);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNCompilation_Build failed, code: %s", HiAIUtils::errStr(code));
        m_has_built = true;
        return true;
    }

    void release() {
        if (m_handle) {
            OH_NNCompilation_Destroy(&m_handle);
            m_handle = nullptr;
        }
        m_has_built = false;
    }

private:
    OH_NNExecutor *create() {
        _FATAL_IF(!m_handle, "OH_NNCompilation not create!");
        _FATAL_IF(!m_has_built, "OH_NNCompilation not build!");
        OH_NNExecutor *executor = OH_NNExecutor_Construct(m_handle);
        _ERROR_IF(!executor, "OH_NNExecutor_Construct failed");
        return executor;
    }

private:
    bool m_has_built = false;
    OH_NNCompilation *m_handle = nullptr;
};

class NNExecutor {
public:
    ~NNExecutor() {
        this->release();
    }

public:
    bool create(NNCompilation &compilation) {
        this->release();
        m_handle = compilation.create();
        return m_handle != nullptr;
    }

    void release() {
        if (m_handle) {
            OH_NNExecutor_Destroy(&m_handle);
            m_handle = nullptr;
        }
    }

private:
    OH_NNExecutor *m_handle = nullptr;
};

NAMESPACE_END
