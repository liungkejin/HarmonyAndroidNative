//
// Created on 2025/1/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Common.h"
#include <mindspore/status.h>

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
};

NAMESPACE_END
