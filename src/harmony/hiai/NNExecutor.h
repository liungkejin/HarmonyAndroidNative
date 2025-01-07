//
// Created by bigheadson on 2025/1/7.
//

#pragma once

#include "common/Common.h"
#include "HiAI.h"
#include "NNTensor.h"
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

    /**
     * 查找第一个设备名是 HIAI_F 开头的设备 id
     * @return
     */
    static size_t findHiAiDevice() {
        auto ids = getAllDevicesID();
        for (auto id : ids) {
            auto name = deviceName(id);
            if (name == "HIAI_F") {
                return id;
            }
        }
        return 0;
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
        if (m_handle == nullptr) {
            return false;
        }

        m_device_id = NNDevice::findHiAiDevice();
        m_input_num = getInputCount();
        for (size_t i = 0; i < m_input_num; i++) {
            auto *tensorDesc = createInputTensorDesc(i);
            auto desc = NNTensorDesc(tensorDesc, true);
            NNTensor tensor(desc, m_device_id);
            m_input_tensors[i] = tensor.value();
            m_inputs.push_back(tensor);
        }

        m_output_num = getOutputCount();
        for (size_t i = 0; i < m_output_num; i++) {
            auto *tensorDesc = createOutputTensorDesc(i);
            auto desc = NNTensorDesc(tensorDesc, true);
            NNTensor tensor(desc, m_device_id);
            m_output_tensors[i] = tensor.value();
            m_outputs.push_back(tensor);
        }
        return true;
    }

    bool setOnRunDone(NN_OnRunDone onRunDone) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        auto code = OH_NNExecutor_SetOnRunDone(m_handle, onRunDone);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNExecutor_SetOnRunDone failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    bool setOnServiceDied(NN_OnServiceDied onServiceDied) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        auto code = OH_NNExecutor_SetOnServiceDied(m_handle, onServiceDied);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNExecutor_SetOnServiceDied failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    size_t getOutputCount() {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        size_t output_num = 0;
        auto code = OH_NNExecutor_GetOutputCount(m_handle, &output_num);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, 0, "OH_NNExecutor_GetOutputCount failed, code: %s", HiAIUtils::errStr(code));
        return output_num;
    }

    NNShape getOutputShape(size_t index) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        int32_t *shape = nullptr;
        uint32_t shape_length = 0;
        auto code = OH_NNExecutor_GetOutputShape(m_handle, index, &shape, &shape_length);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, NNShape(0), "OH_NNExecutor_GetOutputShape failed, code: %s", HiAIUtils::errStr(code));
        return {shape, shape_length};
    }

    size_t getInputCount() {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        size_t input_num = 0;
        auto code = OH_NNExecutor_GetInputCount(m_handle, &input_num);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, 0, "OH_NNExecutor_GetInputCount failed, code: %s", HiAIUtils::errStr(code));
        return input_num;
    }

    NNTensor& getInput(size_t index) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        _FATAL_IF(index >= m_input_num, "input index out of range: %d", index);
        return m_inputs[index];
    }

    NNTensor& getOutput(size_t index) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        _FATAL_IF(index >= m_output_num, "output index out of range: %d", index);
        return m_outputs[index];
    }

    bool runSync() {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        auto code = OH_NNExecutor_RunSync(m_handle, m_input_tensors, m_input_num, m_output_tensors, m_output_num);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNExecutor_RunSync failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    bool runAsync(int timeout, void *userDataa) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        auto code = OH_NNExecutor_RunAsync(m_handle, m_input_tensors, m_input_num, m_output_tensors, m_output_num, timeout, userDataa);
        _ERROR_RETURN_IF(code != OH_NN_SUCCESS, false, "OH_NNExecutor_RunAsync failed, code: %s", HiAIUtils::errStr(code));
        return true;
    }

    void release() {
        if (m_handle) {
            m_input_num = 0;
            memset(m_input_tensors, 0, sizeof(m_input_tensors));
            m_inputs.clear();
            m_output_num = 0;
            memset(m_output_tensors, 0, sizeof(m_output_tensors));
            m_outputs.clear();

            OH_NNExecutor_Destroy(&m_handle);
            m_handle = nullptr;
        }
    }

private:
    NN_TensorDesc * createInputTensorDesc(size_t index) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        NN_TensorDesc *desc = OH_NNExecutor_CreateInputTensorDesc(m_handle, index);
        _ERROR_IF(!desc, "OH_NNExecutor_CreateInputTensorDesc failed");
        return desc;
    }

    NN_TensorDesc * createOutputTensorDesc(size_t index) {
        _FATAL_IF(!m_handle, "OH_NNExecutor not create!");
        NN_TensorDesc *desc = OH_NNExecutor_CreateOutputTensorDesc(m_handle, index);
        _ERROR_IF(!desc, "OH_NNExecutor_CreateOutputTensorDesc failed");
        return desc;
    }

private:
    OH_NNExecutor *m_handle = nullptr;
    size_t m_device_id = 0;

    size_t m_input_num = 0;
    NN_Tensor *m_input_tensors[256] = {nullptr};
    std::vector<NNTensor> m_inputs;

    size_t m_output_num = 0;
    NN_Tensor *m_output_tensors[256] = {nullptr};
    std::vector<NNTensor> m_outputs;
};

NAMESPACE_END
