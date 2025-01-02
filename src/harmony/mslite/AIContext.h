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
#include <mindspore/model.h>
#include <mindspore/context.h>
#include <mindspore/status.h>
#include <mindspore/tensor.h>

#include "Namespace.h"
#include "common/Object.h"

NAMESPACE_DEFAULT

class AIDeviceInfo : Object {
public:
    static NNRTDeviceDesc *getAllNNRTDeviceDescs(size_t &outNum) { return OH_AI_GetAllNNRTDeviceDescs(&outNum); }

    static NNRTDeviceDesc *getElementOfNNRTDeviceDescs(NNRTDeviceDesc *descs, size_t index) {
        return OH_AI_GetElementOfNNRTDeviceDescs(descs, index);
    }

public:
    explicit AIDeviceInfo(OH_AI_DeviceType type) {
        m_handle = OH_AI_DeviceInfoCreate(type);
        _FATAL_IF(!m_handle, "OH_AI_DeviceInfoCreate failed");
    }

    AIDeviceInfo(const AIDeviceInfo &o) : m_handle(o.m_handle), Object(o) {}

    ~AIDeviceInfo() {
        if (no_reference() && m_handle) {
            OH_AI_DeviceInfoDestroy(&m_handle);
            m_handle = nullptr;
        }
    }

public:
    OH_AI_DeviceInfoHandle value() const {
        return m_handle;
    }

    void setProvider(const char *name) { OH_AI_DeviceInfoSetProvider(m_handle, name); }

    const char *getProvider() { return OH_AI_DeviceInfoGetProvider(m_handle); }

    void setProviderDevice(const char *name) { OH_AI_DeviceInfoSetProviderDevice(m_handle, name); }

    const char *getProviderDevice() { return OH_AI_DeviceInfoGetProviderDevice(m_handle); }

    OH_AI_DeviceType getDeviceType() { return OH_AI_DeviceInfoGetDeviceType(m_handle); }

    void setF16Enable(bool enable) { OH_AI_DeviceInfoSetEnableFP16(m_handle, enable); }

    bool isF16Enable() { return OH_AI_DeviceInfoGetEnableFP16(m_handle); }

    void setFrequency(int frequency) { OH_AI_DeviceInfoSetFrequency(m_handle, frequency); }

    int getFrequency() { return OH_AI_DeviceInfoGetFrequency(m_handle); }

    void setDeviceId(size_t id) { OH_AI_DeviceInfoSetDeviceId(m_handle, id); }

    size_t getDeviceId() { return OH_AI_DeviceInfoGetDeviceId(m_handle); }

    void setPerformanceMode(OH_AI_PerformanceMode mode) { OH_AI_DeviceInfoSetPerformanceMode(m_handle, mode); }

    OH_AI_PerformanceMode getPerformanceMode() { return OH_AI_DeviceInfoGetPerformanceMode(m_handle); }

    void setPriority(OH_AI_Priority priority) { OH_AI_DeviceInfoSetPriority(m_handle, priority); }

    OH_AI_Priority getPriority() { return OH_AI_DeviceInfoGetPriority(m_handle); }

    OH_AI_Status addExtension(const char *name, const char *value, size_t value_size) {
        return OH_AI_DeviceInfoAddExtension(m_handle, name, value, value_size);
    }

private:
    OH_AI_DeviceInfoHandle m_handle;
};

class AIContext : Object {
public:
    explicit AIContext() {
        m_handle = OH_AI_ContextCreate();
        _FATAL_IF(!m_handle, "OH_AI_ContextCreate failed");
    }

    AIContext(const AIContext &o) : m_handle(o.m_handle), Object(o) {}

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

    void addDeviceInfo(const AIDeviceInfo &info) { OH_AI_ContextAddDeviceInfo(m_handle, info.value()); }

private:
    OH_AI_ContextHandle m_handle;
};

NAMESPACE_END
