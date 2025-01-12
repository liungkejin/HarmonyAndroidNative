//
// Created by LiangKeJin on 2025/1/2.
//

#pragma once

#include "Namespace.h"
#include "common/Log.h"
#include "common/Object.h"
#include "AITensor.h"

#include <cstddef>
#include <mindspore/types.h>
#include <mindspore/context.h>
#include <mindspore/status.h>

NAMESPACE_DEFAULT
    class AIDeviceInfo {
public:
    static NNRTDeviceDesc *getAllNNRTDeviceDescs(size_t &outNum) { return OH_AI_GetAllNNRTDeviceDescs(&outNum); }

    static NNRTDeviceDesc *getElementOfNNRTDeviceDescs(NNRTDeviceDesc *descs, size_t index) {
        return OH_AI_GetElementOfNNRTDeviceDescs(descs, index);
    }

public:

    ~AIDeviceInfo() {
        if (m_handle) {
            OH_AI_DeviceInfoDestroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
    bool valid() {
        return m_handle != nullptr;
    }

    OH_AI_DeviceInfoHandle value() const {
        return m_handle;
    }
    
    bool create(OH_AI_DeviceType type) {
        if (m_handle != nullptr) {
            OH_AI_DeviceInfoDestroy(&m_handle);
            m_handle = nullptr;
        }
        m_handle = OH_AI_DeviceInfoCreate(type);
        _WARN_IF(!m_handle, "create ai device info(%s) failed!", AIUtils::deviceTypeStr(type));
        return m_handle != nullptr;
    }

    void setProvider(const char *name) { OH_AI_DeviceInfoSetProvider(m_handle, name); }

    const char *getProvider() const { return OH_AI_DeviceInfoGetProvider(m_handle); }

    void setProviderDevice(const char *name) { OH_AI_DeviceInfoSetProviderDevice(m_handle, name); }

    const char *getProviderDevice() const { return OH_AI_DeviceInfoGetProviderDevice(m_handle); }

    OH_AI_DeviceType getDeviceType() const { return OH_AI_DeviceInfoGetDeviceType(m_handle); }

    void setF16Enable(bool enable) { OH_AI_DeviceInfoSetEnableFP16(m_handle, enable); }

    bool isF16Enable() const { return OH_AI_DeviceInfoGetEnableFP16(m_handle); }

    void setFrequency(int frequency) { OH_AI_DeviceInfoSetFrequency(m_handle, frequency); }

    int getFrequency() const { return OH_AI_DeviceInfoGetFrequency(m_handle); }

    // only valid for NNRT device
    void setDeviceId(size_t id) { OH_AI_DeviceInfoSetDeviceId(m_handle, id); }

    size_t getDeviceId() const { return OH_AI_DeviceInfoGetDeviceId(m_handle); }

    void setPerformanceMode(OH_AI_PerformanceMode mode) { OH_AI_DeviceInfoSetPerformanceMode(m_handle, mode); }

    OH_AI_PerformanceMode getPerformanceMode() const { return OH_AI_DeviceInfoGetPerformanceMode(m_handle); }

    // priority only valid for NNRT
    void setPriority(OH_AI_Priority priority) { OH_AI_DeviceInfoSetPriority(m_handle, priority); }

//    OH_AI_Priority getPriority() const { return OH_AI_DeviceInfoGetPriority(m_handle); }

    OH_AI_Status addExtension(const char *name, const char *value, size_t value_size) {
        return OH_AI_DeviceInfoAddExtension(m_handle, name, value, value_size);
    }
    
public:
    std::string toString() const {
        auto type = getDeviceType();
        std::stringstream ss;
        ss << "AIDeviceInfo {\n"
           << "  type: " << AIUtils::deviceTypeStr(type) << "\n"
           << "  provider: " << getProvider() << "\n"
           << "  provider device: " << getProviderDevice() << "\n"
           << "  f16: " << (isF16Enable() ? "enable" : "disable") << "\n";
        if (type == OH_AI_DEVICETYPE_NNRT) {
            ss << "  frequency: " << getFrequency() << "\n"
               << "  performance mode: " << AIUtils::performanceModeStr(getPerformanceMode()) << "\n";
//           << "  priority: " << AIUtils::priorityStr(getPriority()) << "\n"
        }
        ss << "}" << std::endl;
        return ss.str();
    }

private:
    OH_AI_DeviceInfoHandle m_handle = nullptr;
};

class AIContext : Object {
public:
    explicit AIContext() {
        m_handle = OH_AI_ContextCreate();
        _FATAL_IF(!m_handle, "OH_AI_ContextCreate failed");
    }

    AIContext(const AIContext &o) : Object(o), m_handle(o.m_handle) {}

    ~AIContext() {
        if (no_reference() && m_handle) {
            OH_AI_ContextDestroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
    const OH_AI_ContextHandle& value() const {
        return m_handle;
    }

    void setThreadNum(int num) { OH_AI_ContextSetThreadNum(m_handle, num); }

    int getThreadNum() { return OH_AI_ContextGetThreadNum(m_handle); }

    void setThreadAffinityMode(int mode) { OH_AI_ContextSetThreadAffinityMode(m_handle, mode); }

    int getThreadAffinityMode() { return OH_AI_ContextGetThreadAffinityMode(m_handle); }

    const int *getThreadAffinityCoreList(size_t &outSize) {
        return OH_AI_ContextGetThreadAffinityCoreList(m_handle, &outSize);
    }

    void setParallelEnable(bool enable) { OH_AI_ContextSetEnableParallel(m_handle, enable); }

    bool isParallelEnable() { return OH_AI_ContextGetEnableParallel(m_handle); }

    void addDeviceInfo(const AIDeviceInfo &info) {
        OH_AI_ContextAddDeviceInfo(m_handle, info.value());
    }
    
    std::string toString() {
        std::stringstream ss;
        ss << "AIContext{thread num: " << getThreadNum() << ", thread affinity mode: " << getThreadAffinityMode() << ", parallel enable: " << isParallelEnable() << "}";
        return ss.str();
    }

private:
    OH_AI_ContextHandle m_handle;
};

NAMESPACE_END
