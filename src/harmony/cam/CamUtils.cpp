//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CamUtils.h"

NAMESPACE_DEFAULT

std::string CamUtils::errString(CamErrorCode e) {
    switch (e) {
        case CAMERA_OK :
            return "CAMERA_OK";
        case CAMERA_INVALID_ARGUMENT :
            return "CAMERA_INVALID_ARGUMENT";
        case CAMERA_OPERATION_NOT_ALLOWED :
            return "CAMERA_OPERATION_NOT_ALLOWED";
        case CAMERA_SESSION_NOT_CONFIG :
            return "CAMERA_SESSION_NOT_CONFIG";
        case CAMERA_SESSION_NOT_RUNNING :
            return "CAMERA_SESSION_NOT_RUNNING";
        case CAMERA_SESSION_CONFIG_LOCKED :
            return "CAMERA_SESSION_CONFIG_LOCKED";
        case CAMERA_DEVICE_SETTING_LOCKED :
            return "CAMERA_DEVICE_SETTING_LOCKED";
        case CAMERA_CONFLICT_CAMERA :
            return "CAMERA_CONFLICT_CAMERA";
        case CAMERA_DEVICE_DISABLED :
            return "CAMERA_DEVICE_DISABLED";
        case CAMERA_DEVICE_PREEMPTED :
            return "CAMERA_DEVICE_PREEMPTED";
        case CAMERA_SERVICE_FATAL_ERROR :
            return "CAMERA_SERVICE_FATAL_ERROR";
        default:
            return std::string("CAMERA_ERROR_CODE(") + std::to_string(e) + ")";
    }
}

std::string CamUtils::statusStr(Camera_Status s) {
    switch (s) {
        case CAMERA_STATUS_APPEAR :
            return "CAMERA_STATUS_APPEAR";
        case CAMERA_STATUS_DISAPPEAR :
            return "CAMERA_STATUS_DISAPPEAR";
        case CAMERA_STATUS_AVAILABLE :
            return "CAMERA_STATUS_AVAILABLE";
        case CAMERA_STATUS_UNAVAILABLE :
            return "CAMERA_STATUS_UNAVAILABLE";
    }

    return std::string("CAMERA_STATUS(") + std::to_string(s) + ")";
}

std::string CamUtils::positionStr(Camera_Position p) {
    switch (p) {
        case CAMERA_POSITION_UNSPECIFIED :
            return "CAMERA_POSITION_UNSPECIFIED";
        case CAMERA_POSITION_BACK :
            return "CAMERA_POSITION_BACK";
        case CAMERA_POSITION_FRONT :
            return "CAMERA_POSITION_FRONT";
    }

    return std::string("CAMERA_POSITION(") + std::to_string(p) + ")";
}

std::string CamUtils::typeStr(Camera_Type t) {
    switch (t) {
        case CAMERA_TYPE_DEFAULT :
            return "CAMERA_TYPE_DEFAULT";
        case CAMERA_TYPE_WIDE_ANGLE :
            return "CAMERA_TYPE_WIDE_ANGLE";
        case CAMERA_TYPE_ULTRA_WIDE :
            return "CAMERA_TYPE_ULTRA_WIDE";
        case CAMERA_TYPE_TELEPHOTO :
            return "CAMERA_TYPE_TELEPHOTO";
        case CAMERA_TYPE_TRUE_DEPTH :
            return "CAMERA_TYPE_TRUE_DEPTH";
    }
    return std::string("CAMERA_TYPE(") + std::to_string(t) + ")";
}

std::string CamUtils::connectionStr(Camera_Connection c) {
    switch (c) {
        case CAMERA_CONNECTION_BUILT_IN :
            return "CAMERA_CONNECTION_BUILT_IN";
        case CAMERA_CONNECTION_USB_PLUGIN :
            return "CAMERA_CONNECTION_USB_PLUGIN";
        case CAMERA_CONNECTION_REMOTE :
            return "CAMERA_CONNECTION_REMOTE";
    }
    return std::string("CAMERA_CONNECTION(") + std::to_string(c) + ")";
}

std::string CamUtils::formatStr(Camera_Format f) {
    switch (f) {
        case CAMERA_FORMAT_RGBA_8888 :
            return "CAMERA_FORMAT_RGBA_8888";
        case CAMERA_FORMAT_YUV_420_SP :
            return "CAMERA_FORMAT_YUV_420_SP";
        case CAMERA_FORMAT_JPEG :
            return "CAMERA_FORMAT_JPEG";
        case CAMERA_FORMAT_YCBCR_P010:
            return "CAMERA_FORMAT_YCBCR_P010";
        case CAMERA_FORMAT_YCRCB_P010:
            return "CAMERA_FORMAT_YCRCB_P010";
    }
    return std::string("CAMERA_FORMAT(") + std::to_string(f) + ")";
}

std::string CamUtils::flashModeStr(Camera_FlashMode f) {
    switch (f) {
        case FLASH_MODE_CLOSE :
            return "FLASH_MODE_CLOSE";
        case FLASH_MODE_OPEN :
            return "FLASH_MODE_OPEN";
        case FLASH_MODE_AUTO :
            return "FLASH_MODE_AUTO";
        case FLASH_MODE_ALWAYS_OPEN :
            return "FLASH_MODE_ALWAYS_OPEN";
    }

    return std::string("FLASH_MODE(") + std::to_string(f) + ")";
}

std::string CamUtils::exposureModeStr(Camera_ExposureMode e) {
    switch (e) {
        case EXPOSURE_MODE_LOCKED :
            return "EXPOSURE_MODE_LOCKED";
        case EXPOSURE_MODE_AUTO :
            return "EXPOSURE_MODE_AUTO";
        case EXPOSURE_MODE_CONTINUOUS_AUTO :
            return "EXPOSURE_MODE_CONTINUOUS_AUTO";
    }

    return std::string("EXPOSURE_MODE(") + std::to_string(e) + ")";
}

std::string CamUtils::focusModeStr(Camera_FocusMode f) {
    switch (f) {
        case FOCUS_MODE_MANUAL :
            return "FOCUS_MODE_MANUAL";
        case FOCUS_MODE_CONTINUOUS_AUTO :
            return "FOCUS_MODE_CONTINUOUS_AUTO";
        case FOCUS_MODE_AUTO :
            return "FOCUS_MODE_AUTO";
        case FOCUS_MODE_LOCKED :
            return "FOCUS_MODE_LOCKED";
    }

    return std::string("FOCUS_MODE(") + std::to_string(f) + ")";
}

std::string CamUtils::focusStateStr(Camera_FocusState f) {
    switch (f) {
        case FOCUS_STATE_SCAN :
            return "FOCUS_STATE_SCAN";
        case FOCUS_STATE_FOCUSED :
            return "FOCUS_STATE_FOCUSED";
        case FOCUS_STATE_UNFOCUSED :
            return "FOCUS_STATE_UNFOCUSED";
    }

    return std::string("FOCUS_STATE(") + std::to_string(f) + ")";
}

std::string CamUtils::videoStabModeStr(Camera_VideoStabilizationMode m) {
    switch (m) {
        case STABILIZATION_MODE_OFF :
            return "STABILIZATION_MODE_OFF";
        case STABILIZATION_MODE_LOW :
            return "STABILIZATION_MODE_LOW";
        case STABILIZATION_MODE_MIDDLE :
            return "STABILIZATION_MODE_MIDDLE";
        case STABILIZATION_MODE_HIGH :
            return "STABILIZATION_MODE_HIGH";
        case STABILIZATION_MODE_AUTO :
            return "STABILIZATION_MODE_AUTO";
    }

    return std::string("STABILIZATION_MODE(") + std::to_string(m) + ")";
}

std::string CamUtils::imageRotationStr(Camera_ImageRotation r) {
    switch (r) {
        case IAMGE_ROTATION_0 :
            return "IMAGE_ROTATION_0";
        case IAMGE_ROTATION_90 :
            return "IMAGE_ROTATION_90";
        case IAMGE_ROTATION_180 :
            return "IMAGE_ROTATION_180";
        case IAMGE_ROTATION_270 :
            return "IMAGE_ROTATION_270";
    }

    return std::string("IMAGE_ROTATION(") + std::to_string(r) + ")";
}

std::string CamUtils::qualityLevelStr(Camera_QualityLevel q) {
    switch (q) {
        case QUALITY_LEVEL_HIGH :
            return "QUALITY_LEVEL_HIGH";
        case QUALITY_LEVEL_MEDIUM :
            return "QUALITY_LEVEL_MEDIUM";
        case QUALITY_LEVEL_LOW :
            return "QUALITY_LEVEL_LOW";
    }

    return std::string("QUALITY_LEVEL(") + std::to_string(q) + ")";
}

std::string CamUtils::metadataObjectType(Camera_MetadataObjectType t) {
    switch (t) {
        case FACE_DETECTION :
            return "FACE_DETECTION";
    }

    return std::string("METADATA_OBJECT_TYPE(") + std::to_string(t) + ")";
}

NAMESPACE_END