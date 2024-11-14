//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CamUtils.h"

NAMESPACE_DEFAULT

static char __unknownErrStr[64] = {0};
static const char *unknownStr(const char *type, int code) {
    int len = sprintf(__unknownErrStr, "UNKNOWN_%s(%d)", type, code);
    __unknownErrStr[std::min(len, 63)] = '\0';
    return __unknownErrStr;
}

const char *CamUtils::errString(const CamErrorCode e) {
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
    }
    return unknownStr("CAMERA_ERROR_CODE", e);
}

const char *CamUtils::statusStr(const Camera_Status s) {
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

    return unknownStr("CAMERA_STATUS", s);
}

const char *CamUtils::positionStr(const Camera_Position p) {
    switch (p) {
    case CAMERA_POSITION_UNSPECIFIED :
        return "CAMERA_POSITION_UNSPECIFIED";
    case CAMERA_POSITION_BACK :
        return "CAMERA_POSITION_BACK";
    case CAMERA_POSITION_FRONT :
        return "CAMERA_POSITION_FRONT";
    }
    return unknownStr("CAMERA_POSITION", p);
}

const char *CamUtils::typeStr(const Camera_Type t) {
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
    return unknownStr("CAMERA_TYPE", t);
}

const char *CamUtils::connectionStr(const Camera_Connection c) {
    switch (c) {
    case CAMERA_CONNECTION_BUILT_IN :
        return "CAMERA_CONNECTION_BUILT_IN";
    case CAMERA_CONNECTION_USB_PLUGIN :
        return "CAMERA_CONNECTION_USB_PLUGIN";
    case CAMERA_CONNECTION_REMOTE :
        return "CAMERA_CONNECTION_REMOTE";
    }
    return unknownStr("CAMERA_CONNECTION", c);
}

const char *CamUtils::formatStr(const Camera_Format f) {
    switch (f) {
    case CAMERA_FORMAT_RGBA_8888 :
        return "CAMERA_FORMAT_RGBA_8888";
    case CAMERA_FORMAT_YUV_420_SP :
        return "CAMERA_FORMAT_YUV_420_SP";
    case CAMERA_FORMAT_JPEG :
        return "CAMERA_FORMAT_JPEG";
    }
    return unknownStr("CAMERA_FORMAT", f);
}

const char *CamUtils::flashModeStr(const Camera_FlashMode f) {
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

    return unknownStr("FLASH_MODE", f);
}

const char *CamUtils::exposureModeStr(const Camera_ExposureMode e) {
    switch (e) {
    case EXPOSURE_MODE_LOCKED :
        return "EXPOSURE_MODE_LOCKED";
    case EXPOSURE_MODE_AUTO :
        return "EXPOSURE_MODE_AUTO";
    case EXPOSURE_MODE_CONTINUOUS_AUTO :
        return "EXPOSURE_MODE_CONTINUOUS_AUTO";
    }
    return unknownStr("EXPOSURE_MODE", e);
}

const char *CamUtils::focusModeStr(const Camera_FocusMode f) {
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
    return unknownStr("FOCUS_MODE", f);
}

const char *CamUtils::focusStateStr(const Camera_FocusState f) {
    switch (f) {
    case FOCUS_STATE_SCAN :
        return "FOCUS_STATE_SCAN";
    case FOCUS_STATE_FOCUSED :
        return "FOCUS_STATE_FOCUSED";
    case FOCUS_STATE_UNFOCUSED :
        return "FOCUS_STATE_UNFOCUSED";
    }
    return unknownStr("FOCUS_STATE", f);
}

const char *CamUtils::videoStabModeStr(const Camera_VideoStabilizationMode m) {
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
    return unknownStr("STABILIZATION_MODE", m);
}

const char *CamUtils::imageRotationStr(const Camera_ImageRotation r) {
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
    return unknownStr("IMAGE_ROTATION", r);
}

const char *CamUtils::qualityLevelStr(const Camera_QualityLevel q) {
    switch (q) {
    case QUALITY_LEVEL_HIGH :
        return "QUALITY_LEVEL_HIGH";
    case QUALITY_LEVEL_MEDIUM :
        return "QUALITY_LEVEL_MEDIUM";
    case QUALITY_LEVEL_LOW :
        return "QUALITY_LEVEL_LOW";
    }
    return unknownStr("QUALITY_LEVEL", q);
}

const char *CamUtils::metadataObjectType(const Camera_MetadataObjectType t) {
    switch (t) {
    case FACE_DETECTION :
        return "FACE_DETECTION";
    }
    return unknownStr("METADATA_TYPE", t);
}

NAMESPACE_END