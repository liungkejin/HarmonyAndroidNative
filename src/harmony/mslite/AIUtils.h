//
// Created on 2025/1/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include <mindspore/data_type.h>
#include <mindspore/format.h>
#include <mindspore/status.h>
#include <mindspore/types.h>

NAMESPACE_DEFAULT

class AIUtils {
public:
    static std::string statusStr(OH_AI_Status status) {
        switch (status) {
        case OH_AI_STATUS_SUCCESS :
            return "SUCCESS";
        // Core
        case OH_AI_STATUS_CORE_FAILED :
            return "OH_AI_STATUS_CORE_FAILED";

        // Lite  // Common error code, range: [-1, -100）
        case OH_AI_STATUS_LITE_ERROR :
            return "OH_AI_STATUS_LITE_ERROR";
        case OH_AI_STATUS_LITE_NULLPTR :
            return "OH_AI_STATUS_LITE_NULLPTR";
        case OH_AI_STATUS_LITE_PARAM_INVALID :
            return "OH_AI_STATUS_LITE_PARAM_INVALID";
        case OH_AI_STATUS_LITE_NO_CHANGE :
            return "OH_AI_STATUS_LITE_NO_CHANGE";
        case OH_AI_STATUS_LITE_SUCCESS_EXIT :
            return "OH_AI_STATUS_LITE_SUCCESS_EXIT";
        case OH_AI_STATUS_LITE_MEMORY_FAILED :
            return "OH_AI_STATUS_LITE_MEMORY_FAILED";
        case OH_AI_STATUS_LITE_NOT_SUPPORT :
            return "OH_AI_STATUS_LITE_NOT_SUPPORT";
        case OH_AI_STATUS_LITE_THREADPOOL_ERROR :
            return "OH_AI_STATUS_LITE_THREADPOOL_ERROR";
        case OH_AI_STATUS_LITE_UNINITIALIZED_OBJ :
            return "OH_AI_STATUS_LITE_UNINITIALIZED_OBJ";
        // Executor error code, range: [-100,-200)
        case OH_AI_STATUS_LITE_OUT_OF_TENSOR_RANGE :
            return "OH_AI_STATUS_LITE_OUT_OF_TENSOR_RANGE";
        case OH_AI_STATUS_LITE_INPUT_TENSOR_ERROR :
            return "OH_AI_STATUS_LITE_INPUT_TENSOR_ERROR";
        case OH_AI_STATUS_LITE_REENTRANT_ERROR :
            return "OH_AI_STATUS_LITE_REENTRANT_ERROR";
        case OH_AI_STATUS_LITE_GRAPH_FILE_ERROR :
            return "OH_AI_STATUS_LITE_GRAPH_FILE_ERROR";
        // Node error code, range: [-300,-400)
        case OH_AI_STATUS_LITE_NOT_FIND_OP :
            return "OH_AI_STATUS_LITE_NOT_FIND_OP";
        case OH_AI_STATUS_LITE_INVALID_OP_NAME :
            return "OH_AI_STATUS_LITE_INVALID_OP_NAME";
        case OH_AI_STATUS_LITE_INVALID_OP_ATTR :
            return "OH_AI_STATUS_LITE_INVALID_OP_ATTR";
        case OH_AI_STATUS_LITE_OP_EXECUTE_FAILURE :
            return "OH_AI_STATUS_LITE_OP_EXECUTE_FAILURE";
        // Tensor error code, range: [-400,-500)
        case OH_AI_STATUS_LITE_FORMAT_ERROR :
            return "OH_AI_STATUS_LITE_FORMAT_ERROR";
        // InferShape error code, range: [-500,-600)
        case OH_AI_STATUS_LITE_INFER_ERROR :
            return "OH_AI_STATUS_LITE_INFER_ERROR";
        case OH_AI_STATUS_LITE_INFER_INVALID :
            return "OH_AI_STATUS_LITE_INFER_INVALID";
        // User input param error code, range: [-600, 700)
        case OH_AI_STATUS_LITE_INPUT_PARAM_INVALID :
            return "OH_AI_STATUS_LITE_INPUT_PARAM_INVALID";
        }
        return "Unknown: " + std::to_string(status);
    }

    static std::string deviceTypeStr(OH_AI_DeviceType type) {
        switch (type) {
        case OH_AI_DEVICETYPE_CPU :
            return "OH_AI_DEVICETYPE_CPU";
        case OH_AI_DEVICETYPE_GPU :
            return "OH_AI_DEVICETYPE_GPU";
        case OH_AI_DEVICETYPE_KIRIN_NPU :
            return "OH_AI_DEVICETYPE_KIRIN_NPU";
        case OH_AI_DEVICETYPE_NNRT :
            return "OH_AI_DEVICETYPE_NNRT";
        case OH_AI_DEVICETYPE_INVALID :
        default :
            return "OH_AI_DEVICETYPE_INVALID";
        }
    }

    static std::string performanceModeStr(OH_AI_PerformanceMode mode) {
        switch (mode) {
        /** No performance mode preference */
        case OH_AI_PERFORMANCE_NONE :
            return "OH_AI_PERFORMANCE_NONE";
        /** Low power consumption mode*/
        case OH_AI_PERFORMANCE_LOW :
            return "OH_AI_PERFORMANCE_LOW";
        /** Medium performance mode */
        case OH_AI_PERFORMANCE_MEDIUM :
            return "OH_AI_PERFORMANCE_MEDIUM";
        /** High performance mode */
        case OH_AI_PERFORMANCE_HIGH :
            return "OH_AI_PERFORMANCE_HIGH";
        /** Ultimate performance mode */
        case OH_AI_PERFORMANCE_EXTREME :
            return "OH_AI_PERFORMANCE_EXTREME";
        }
        return "UNKNOWN_PERFORMANCE_MODE: " + std::to_string(mode);
    }

    static std::string priorityStr(OH_AI_Priority priority) {
        switch (priority) {
        /** No priority preference */
        case OH_AI_PRIORITY_NONE :
            return "OH_AI_PRIORITY_NONE";
        /** Low priority */
        case OH_AI_PRIORITY_LOW :
            return "OH_AI_PRIORITY_LOW";
        /** Medium priority */
        case OH_AI_PRIORITY_MEDIUM :
            return "OH_AI_PRIORITY_MEDIUM";
        /** High priority */
        case OH_AI_PRIORITY_HIGH :
            return "OH_AI_PRIORITY_HIGH";
        }
        return "UNKNOWN_PRIORITY: " + std::to_string(priority);
    }

    static std::string dataTypeStr(OH_AI_DataType type) {
        switch (type) {
            case OH_AI_DATATYPE_OBJECTTYPE_STRING:
                return "STRING";
            case OH_AI_DATATYPE_OBJECTTYPE_LIST:
                return "LIST";
            case OH_AI_DATATYPE_OBJECTTYPE_TUPLE:
                return "TUPLE";
            case OH_AI_DATATYPE_OBJECTTYPE_TENSOR:
                return "TENSOR";
            case OH_AI_DATATYPE_NUMBERTYPE_BOOL:
                return "BOOL";
            case OH_AI_DATATYPE_NUMBERTYPE_INT8:
                return "INT8";
            case OH_AI_DATATYPE_NUMBERTYPE_INT16:
                return "INT16";
            case OH_AI_DATATYPE_NUMBERTYPE_INT32:
                return "INT32";
            case OH_AI_DATATYPE_NUMBERTYPE_INT64:
                return "INT64";
            case OH_AI_DATATYPE_NUMBERTYPE_UINT8:
                return "UINT8";
            case OH_AI_DATATYPE_NUMBERTYPE_UINT16:
                return "UINT16";
            case OH_AI_DATATYPE_NUMBERTYPE_UINT32:
                return "UINT32";
            case OH_AI_DATATYPE_NUMBERTYPE_UINT64:
                return "UINT64";
            case OH_AI_DATATYPE_NUMBERTYPE_FLOAT16:
                return "FLOAT16";
            case OH_AI_DATATYPE_NUMBERTYPE_FLOAT32:
                return "FLOAT32";
            case OH_AI_DATATYPE_NUMBERTYPE_FLOAT64:
                return "FLOAT64";
            default:
                return "UNKNOWN_DATA_TYPE: " + std::to_string(type);
        }
    }

    static std::string formatStr(OH_AI_Format format) {
        switch (format) {
            case OH_AI_FORMAT_NCHW:
                return "NCHW";
            case OH_AI_FORMAT_NHWC:
                return "NHWC";
            case OH_AI_FORMAT_NHWC4:
                return "NHWC4";
            case OH_AI_FORMAT_HWKC:
                return "HWKC";
            case OH_AI_FORMAT_HWCK:
                return "HWCK";
            case OH_AI_FORMAT_KCHW:
                return "KCHW";
            case OH_AI_FORMAT_CKHW:
                return "CKHW";
            case OH_AI_FORMAT_KHWC:
                return "KHWC";
            case OH_AI_FORMAT_CHWK:
                return "CHWK";
            case OH_AI_FORMAT_HW:
                return "HW";
            case OH_AI_FORMAT_HW4:
                return "HW4";
            case OH_AI_FORMAT_NC:
                return "NC";
            case OH_AI_FORMAT_NC4:
                return "NC4";
            case OH_AI_FORMAT_NC4HW4:
                return "NC4HW4";
            case OH_AI_FORMAT_NCDHW:
                return "NCDHW";
            case OH_AI_FORMAT_NWC:
                return "NWC";
            case OH_AI_FORMAT_NCW:
                return "NCW";
            default:
                return "UNKNOWN_FORMAT: " + std::to_string(format);
        }
    }
};

NAMESPACE_END
