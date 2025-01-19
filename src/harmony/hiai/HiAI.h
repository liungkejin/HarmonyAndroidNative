//
// Created by bigheadson on 2025/1/7.
//

#pragma once

#include "ZNamespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include "neural_network_runtime/neural_network_core.h"
#include "hiai_foundation/hiai_aipp_param.h"
#include "hiai_foundation/hiai_tensor.h"

NAMESPACE_DEFAULT

class NNShape : Object {
public:
    explicit NNShape(size_t length) {
        if (length > 0) {
            m_shape = new int32_t[length];
            m_length = length;
        }
    }

    NNShape(const int32_t *shape, size_t length) {
        m_shape = new int32_t[length];
        memcpy(m_shape, shape, length * sizeof(int32_t));
        m_length = length;
    }

    NNShape(const NNShape &o) : Object(o), m_shape(o.m_shape), m_length(o.m_length) {}

    ~NNShape() {
        if (m_shape && no_reference()) {
            delete[] m_shape;
            m_shape = nullptr;
        }
    }

public:
    const int32_t *shape() const { return m_shape; }

    size_t length() const { return m_length; }

    int32_t operator[](size_t index) const {
        if (index >= m_length) {
            _ERROR("index out of range: %d", index);
            return 0;
        }
        return m_shape[index];
    }

    void set(int32_t value, size_t index) {
        if (index >= m_length) {
            _ERROR("index out of range: %d", index);
            return;
        }
        m_shape[index] = value;
    }
    
    std::string toString() {
        std::stringstream ss;
        ss << "NNShape(" << m_length << ")[";
        for (int i = 0; i < m_length && i < 5; ++i) {
            ss << m_shape[i];
            if (i < m_length-1 && i < 4) {
                ss << ",";
            }
        }
        if (m_length > 4) {
            ss << ", ... ";
        }
        ss << "]";
        return ss.str();
    }

private:
    int32_t *m_shape = nullptr;
    size_t m_length = 0;
};

class HiAIUtils {
public:
    // OH_NN_ReturnCode to string
    static std::string errStr(int code) {
        switch (code) {
            case OH_NN_SUCCESS:
                return "OH_NN_SUCCESS";
            case OH_NN_FAILED:
                return "OH_NN_FAILED";
            case OH_NN_INVALID_PARAMETER:
                return "OH_NN_INVALID_PARAMETER";
            case OH_NN_MEMORY_ERROR:
                return "OH_NN_MEMORY_ERROR";
            case OH_NN_OPERATION_FORBIDDEN:
                return "OH_NN_OPERATION_FORBIDDEN";
            case OH_NN_NULL_PTR:
                return "OH_NN_NULL_PTR";
            case OH_NN_INVALID_FILE:
                return "OH_NN_INVALID_FILE";
            case OH_NN_UNAVALIDABLE_DEVICE:
                return "OH_NN_UNAVALIDABLE_DEVICE";
            case OH_NN_INVALID_PATH:
                return "OH_NN_INVALID_PATH";
            case OH_NN_TIMEOUT:
                return "OH_NN_TIMEOUT";
            case OH_NN_UNSUPPORTED:
                return "OH_NN_UNSUPPORTED";
            case OH_NN_CONNECTION_EXCEPTION:
                return "OH_NN_CONNECTION_EXCEPTION";
            case OH_NN_SAVE_CACHE_EXCEPTION:
                return "OH_NN_SAVE_CACHE_EXCEPTION";
            case OH_NN_DYNAMIC_SHAPE:
                return "OH_NN_DYNAMIC_SHAPE";
            case OH_NN_UNAVAILABLE_DEVICE:
                return "OH_NN_UNAVAILABLE_DEVICE";
            default:
                return "UnknownErr: " + std::to_string(code);
        }
    }

    /**
    OH_NN_PERFORMANCE_NONE = 0,
    OH_NN_PERFORMANCE_LOW = 1,
    OH_NN_PERFORMANCE_MEDIUM = 2,
    OH_NN_PERFORMANCE_HIGH = 3,
    OH_NN_PERFORMANCE_EXTREME = 4
    */
    static std::string performanceModeStr(int mode) {
        switch (mode) {
            case OH_NN_PERFORMANCE_NONE:
                return "OH_NN_PERFORMANCE_NONE";
            case OH_NN_PERFORMANCE_LOW:
                return "OH_NN_PERFORMANCE_LOW";
            case OH_NN_PERFORMANCE_MEDIUM:
                return "OH_NN_PERFORMANCE_MEDIUM";
            case OH_NN_PERFORMANCE_HIGH:
                return "OH_NN_PERFORMANCE_HIGH";
            case OH_NN_PERFORMANCE_EXTREME:
                return "OH_NN_PERFORMANCE_EXTREME";
            default:
                return "UNKNOWN_PERFORMANCE_MODE: " + std::to_string(mode);
        }
    }

    static std::string priorityStr(int priority) {
        switch (priority) {
            case OH_NN_PRIORITY_NONE:
                return "OH_NN_PRIORITY_NONE";
            case OH_NN_PRIORITY_LOW:
                return "OH_NN_PRIORITY_LOW";
            case OH_NN_PRIORITY_MEDIUM:
                return "OH_NN_PRIORITY_MEDIUM";
            case OH_NN_PRIORITY_HIGH:
                return "OH_NN_PRIORITY_HIGH";
            default:
                return "UNKNOWN_PRIORITY: " + std::to_string(priority);
        }
    }

    /**
    OH_NN_FORMAT_NONE = 0,
    OH_NN_FORMAT_NCHW = 1,
    OH_NN_FORMAT_NHWC = 2,
    OH_NN_FORMAT_ND = 3
    */
    static std::string formatStr(int format) {
        switch (format) {
            case OH_NN_FORMAT_NONE:
                return "OH_NN_FORMAT_NONE";
            case OH_NN_FORMAT_NCHW:
                return "OH_NN_FORMAT_NCHW";
            case OH_NN_FORMAT_NHWC:
                return "OH_NN_FORMAT_NHWC";
            case OH_NN_FORMAT_ND:
                return "OH_NN_FORMAT_ND";
            default:
                return "UNKNOWN_FORMAT: " + std::to_string(format);
        }
    }

    static std::string deviceTypeStr(int type) {
        switch (type) {
            case OH_NN_OTHERS:
                return "OH_NN_OTHERS";
            case OH_NN_CPU:
                return "OH_NN_CPU";
            case OH_NN_GPU:
                return "OH_NN_GPU";
            case OH_NN_ACCELERATOR:
                return "OH_NN_ACCELERATOR";
            default:
                return "UNKNOWN_DEVICE_TYPE: " + std::to_string(type);
        }
    }

    static std::string dataTypeStr(int type) {
        switch (type) {
            case OH_NN_UNKNOWN:
                return "OH_NN_UNKNOWN";
            case OH_NN_BOOL:
                return "OH_NN_BOOL";
            case OH_NN_INT8:
                return "OH_NN_INT8";
            case OH_NN_INT16:
                return "OH_NN_INT16";
            case OH_NN_INT32:
                return "OH_NN_INT32";
            case OH_NN_INT64:
                return "OH_NN_INT64";
            case OH_NN_UINT8:
                return "OH_NN_UINT8";
            case OH_NN_UINT16:
                return "OH_NN_UINT16";
            case OH_NN_UINT32:
                return "OH_NN_UINT32";
            case OH_NN_UINT64:
                return "OH_NN_UINT64";
            case OH_NN_FLOAT16:
                return "OH_NN_FLOAT16";
            case OH_NN_FLOAT32:
                return "OH_NN_FLOAT32";
            case OH_NN_FLOAT64:
                return "OH_NN_FLOAT64";
            default:
                return "UNKNOWN_DATA_TYPE: " + std::to_string(type);
        }
    }
};

NAMESPACE_END