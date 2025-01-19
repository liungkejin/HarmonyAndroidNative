//
// Created by LiangKeJin on 2024/12/7.
//

#pragma once

#include <camera/NdkCameraError.h>
#include <camera/NdkCameraDevice.h>
#include "ZNamespace.h"
#include "common/Object.h"

NAMESPACE_DEFAULT

typedef camera_status_t CamStatus;
typedef ACameraDevice_request_template CamTemplate;

class CamUtils {
public:
    static std::string errString(int32_t status) {
        switch (status) {
            case ACAMERA_OK:
                return "ACAMERA_OK";
            case ACAMERA_ERROR_UNKNOWN:
                return "ACAMERA_ERROR_UNKNOWN";
            case ACAMERA_ERROR_INVALID_PARAMETER:
                return "ACAMERA_ERROR_INVALID_PARAMETER";
            case ACAMERA_ERROR_CAMERA_DISCONNECTED:
                return "ACAMERA_ERROR_CAMERA_DISCONNECTED";
            case ACAMERA_ERROR_NOT_ENOUGH_MEMORY:
                return "ACAMERA_ERROR_NOT_ENOUGH_MEMORY";
            case ACAMERA_ERROR_METADATA_NOT_FOUND:
                return "ACAMERA_ERROR_METADATA_NOT_FOUND";
            case ACAMERA_ERROR_CAMERA_DEVICE:
                return "ACAMERA_ERROR_CAMERA_DEVICE";
            case ACAMERA_ERROR_CAMERA_SERVICE:
                return "ACAMERA_ERROR_CAMERA_SERVICE";
            case ACAMERA_ERROR_SESSION_CLOSED:
                return "ACAMERA_ERROR_SESSION_CLOSED";
            case ACAMERA_ERROR_INVALID_OPERATION:
                return "ACAMERA_ERROR_INVALID_OPERATION";
            case ACAMERA_ERROR_STREAM_CONFIGURE_FAIL:
                return "ACAMERA_ERROR_STREAM_CONFIGURE_FAIL";
            case ACAMERA_ERROR_CAMERA_IN_USE:
                return "ACAMERA_ERROR_CAMERA_IN_USE";
            case ACAMERA_ERROR_MAX_CAMERA_IN_USE:
                return "ACAMERA_ERROR_MAX_CAMERAS_IN_USE";
            case ACAMERA_ERROR_CAMERA_DISABLED:
                return "ACAMERA_ERROR_CAMERA_DISABLED";
            case ACAMERA_ERROR_PERMISSION_DENIED:
                return "ACAMERA_ERROR_PERMISSION_DENIED";
            case ACAMERA_ERROR_UNSUPPORTED_OPERATION:
                return "ACAMERA_ERROR_UNSUPPORTED_OPERATION";
            default:
                return std::string("UNKNOWN(") + std::to_string(status) + ")";
        }
    }

    static std::string tagString(int32_t tag);

    static std::string colorCorrectionModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX:
                return "ACAMERA_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX";
            case ACAMERA_COLOR_CORRECTION_MODE_FAST:
                return "ACAMERA_COLOR_CORRECTION_MODE_FAST";
            case ACAMERA_COLOR_CORRECTION_MODE_HIGH_QUALITY:
                return "ACAMERA_COLOR_CORRECTION_MODE_HIGH_QUALITY";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string colorCorrectionAberrationModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_COLOR_CORRECTION_ABERRATION_MODE_OFF:
                return "ACAMERA_COLOR_CORRECTION_ABERRATION_MODE_OFF";
            case ACAMERA_COLOR_CORRECTION_ABERRATION_MODE_FAST:
                return "ACAMERA_COLOR_CORRECTION_ABERRATION_MODE_FAST";
            case ACAMERA_COLOR_CORRECTION_ABERRATION_MODE_HIGH_QUALITY:
                return "ACAMERA_COLOR_CORRECTION_ABERRATION_MODE_HIGH_QUALITY";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string aeAntibandingModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_AE_ANTIBANDING_MODE_OFF:
                return "ACAMERA_CONTROL_AE_ANTIBANDING_MODE_OFF";
            case ACAMERA_CONTROL_AE_ANTIBANDING_MODE_AUTO:
                return "ACAMERA_CONTROL_AE_ANTIBANDING_MODE_AUTO";
            case ACAMERA_CONTROL_AE_ANTIBANDING_MODE_50HZ:
                return "ACAMERA_CONTROL_AE_ANTIBANDING_MODE_50HZ";
            case ACAMERA_CONTROL_AE_ANTIBANDING_MODE_60HZ:
                return "ACAMERA_CONTROL_AE_ANTIBANDING_MODE_60HZ";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string aeLockString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_AE_LOCK_OFF:
                return "ACAMERA_CONTROL_AE_LOCK_OFF";
            case ACAMERA_CONTROL_AE_LOCK_ON:
                return "ACAMERA_CONTROL_AE_LOCK_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string aeModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_AE_MODE_OFF:
                return "ACAMERA_CONTROL_AE_MODE_OFF";
            case ACAMERA_CONTROL_AE_MODE_ON:
                return "ACAMERA_CONTROL_AE_MODE_ON";
            case ACAMERA_CONTROL_AE_MODE_ON_AUTO_FLASH:
                return "ACAMERA_CONTROL_AE_MODE_ON_AUTO_FLASH";
            case ACAMERA_CONTROL_AE_MODE_ON_ALWAYS_FLASH:
                return "ACAMERA_CONTROL_AE_MODE_ON_ALWAYS_FLASH";
            case ACAMERA_CONTROL_AE_MODE_ON_AUTO_FLASH_REDEYE:
                return "ACAMERA_CONTROL_AE_MODE_ON_AUTO_FLASH_REDEYE";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string aePrecaptureTriggerString(int32_t trigger) {
        switch (trigger) {
            case ACAMERA_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE:
                return "ACAMERA_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE";
            case ACAMERA_CONTROL_AE_PRECAPTURE_TRIGGER_START:
                return "ACAMERA_CONTROL_AE_PRECAPTURE_TRIGGER_START";
            case ACAMERA_CONTROL_AE_PRECAPTURE_TRIGGER_CANCEL:
                return "ACAMERA_CONTROL_AE_PRECAPTURE_TRIGGER_CANCEL";
            default:
                return std::string("UNKNOWN(") + std::to_string(trigger) + ")";
        }
    }

    static std::string afModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_AF_MODE_OFF:
                return "ACAMERA_CONTROL_AF_MODE_OFF";
            case ACAMERA_CONTROL_AF_MODE_AUTO:
                return "ACAMERA_CONTROL_AF_MODE_AUTO";
            case ACAMERA_CONTROL_AF_MODE_MACRO:
                return "ACAMERA_CONTROL_AF_MODE_MACRO";
            case ACAMERA_CONTROL_AF_MODE_CONTINUOUS_VIDEO:
                return "ACAMERA_CONTROL_AF_MODE_CONTINUOUS_VIDEO";
            case ACAMERA_CONTROL_AF_MODE_CONTINUOUS_PICTURE:
                return "ACAMERA_CONTROL_AF_MODE_CONTINUOUS_PICTURE";
            case ACAMERA_CONTROL_AF_MODE_EDOF:
                return "ACAMERA_CONTROL_AF_MODE_EDOF";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string afTriggerString(int32_t trigger) {
        switch (trigger) {
            case ACAMERA_CONTROL_AF_TRIGGER_IDLE:
                return "ACAMERA_CONTROL_AF_TRIGGER_IDLE";
            case ACAMERA_CONTROL_AF_TRIGGER_START:
                return "ACAMERA_CONTROL_AF_TRIGGER_START";
            case ACAMERA_CONTROL_AF_TRIGGER_CANCEL:
                return "ACAMERA_CONTROL_AF_TRIGGER_CANCEL";
            default:
                return std::string("UNKNOWN(") + std::to_string(trigger) + ")";
        }
    }

    static std::string awbLockString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_AWB_LOCK_OFF:
                return "ACAMERA_CONTROL_AWB_LOCK_OFF";
            case ACAMERA_CONTROL_AWB_LOCK_ON:
                return "ACAMERA_CONTROL_AWB_LOCK_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string awbModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_AWB_MODE_OFF:
                return "ACAMERA_CONTROL_AWB_MODE_OFF";
            case ACAMERA_CONTROL_AWB_MODE_AUTO:
                return "ACAMERA_CONTROL_AWB_MODE_AUTO";
            case ACAMERA_CONTROL_AWB_MODE_INCANDESCENT:
                return "ACAMERA_CONTROL_AWB_MODE_INCANDESCENT";
            case ACAMERA_CONTROL_AWB_MODE_FLUORESCENT:
                return "ACAMERA_CONTROL_AWB_MODE_FLUORESCENT";
            case ACAMERA_CONTROL_AWB_MODE_WARM_FLUORESCENT:
                return "ACAMERA_CONTROL_AWB_MODE_WARM_FLUORESCENT";
            case ACAMERA_CONTROL_AWB_MODE_DAYLIGHT:
                return "ACAMERA_CONTROL_AWB_MODE_DAYLIGHT";
            case ACAMERA_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT:
                return "ACAMERA_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT";
            case ACAMERA_CONTROL_AWB_MODE_TWILIGHT:
                return "ACAMERA_CONTROL_AWB_MODE_TWILIGHT";
            case ACAMERA_CONTROL_AWB_MODE_SHADE:
                return "ACAMERA_CONTROL_AWB_MODE_SHADE";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string captureIntentString(int32_t intent) {
        switch (intent) {
            case ACAMERA_CONTROL_CAPTURE_INTENT_CUSTOM:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_CUSTOM";
            case ACAMERA_CONTROL_CAPTURE_INTENT_PREVIEW:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_PREVIEW";
            case ACAMERA_CONTROL_CAPTURE_INTENT_STILL_CAPTURE:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_STILL_CAPTURE";
            case ACAMERA_CONTROL_CAPTURE_INTENT_VIDEO_RECORD:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_VIDEO_RECORD";
            case ACAMERA_CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT";
            case ACAMERA_CONTROL_CAPTURE_INTENT_ZERO_SHUTTER_LAG:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_ZERO_SHUTTER_LAG";
            case ACAMERA_CONTROL_CAPTURE_INTENT_MANUAL:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_MANUAL";
            case ACAMERA_CONTROL_CAPTURE_INTENT_MOTION_TRACKING:
                return "ACAMERA_CONTROL_CAPTURE_INTENT_MOTION_TRACKING";
            default:
                return std::string("UNKNOWN(") + std::to_string(intent) + ")";
        }
    }

    static std::string effectModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_EFFECT_MODE_OFF:
                return "ACAMERA_CONTROL_EFFECT_MODE_OFF";
            case ACAMERA_CONTROL_EFFECT_MODE_MONO:
                return "ACAMERA_CONTROL_EFFECT_MODE_MONO";
            case ACAMERA_CONTROL_EFFECT_MODE_NEGATIVE:
                return "ACAMERA_CONTROL_EFFECT_MODE_NEGATIVE";
            case ACAMERA_CONTROL_EFFECT_MODE_SOLARIZE:
                return "ACAMERA_CONTROL_EFFECT_MODE_SOLARIZE";
            case ACAMERA_CONTROL_EFFECT_MODE_SEPIA:
                return "ACAMERA_CONTROL_EFFECT_MODE_SEPIA";
            case ACAMERA_CONTROL_EFFECT_MODE_POSTERIZE:
                return "ACAMERA_CONTROL_EFFECT_MODE_POSTERIZE";
            case ACAMERA_CONTROL_EFFECT_MODE_WHITEBOARD:
                return "ACAMERA_CONTROL_EFFECT_MODE_WHITEBOARD";
            case ACAMERA_CONTROL_EFFECT_MODE_BLACKBOARD:
                return "ACAMERA_CONTROL_EFFECT_MODE_BLACKBOARD";
            case ACAMERA_CONTROL_EFFECT_MODE_AQUA:
                return "ACAMERA_CONTROL_EFFECT_MODE_AQUA";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string controlModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_MODE_OFF:
                return "ACAMERA_CONTROL_MODE_OFF";
            case ACAMERA_CONTROL_MODE_AUTO:
                return "ACAMERA_CONTROL_MODE_AUTO";
            case ACAMERA_CONTROL_MODE_USE_SCENE_MODE:
                return "ACAMERA_CONTROL_MODE_USE_SCENE_MODE";
            case ACAMERA_CONTROL_MODE_OFF_KEEP_STATE:
                return "ACAMERA_CONTROL_MODE_OFF_KEEP_STATE";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string sceneModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_SCENE_MODE_DISABLED:
                return "ACAMERA_CONTROL_SCENE_MODE_DISABLED";
            case ACAMERA_CONTROL_SCENE_MODE_FACE_PRIORITY:
                return "ACAMERA_CONTROL_SCENE_MODE_FACE_PRIORITY";
            case ACAMERA_CONTROL_SCENE_MODE_ACTION:
                return "ACAMERA_CONTROL_SCENE_MODE_ACTION";
            case ACAMERA_CONTROL_SCENE_MODE_PORTRAIT:
                return "ACAMERA_CONTROL_SCENE_MODE_PORTRAIT";
            case ACAMERA_CONTROL_SCENE_MODE_LANDSCAPE:
                return "ACAMERA_CONTROL_SCENE_MODE_LANDSCAPE";
            case ACAMERA_CONTROL_SCENE_MODE_NIGHT:
                return "ACAMERA_CONTROL_SCENE_MODE_NIGHT";
            case ACAMERA_CONTROL_SCENE_MODE_NIGHT_PORTRAIT:
                return "ACAMERA_CONTROL_SCENE_MODE_NIGHT_PORTRAIT";
            case ACAMERA_CONTROL_SCENE_MODE_THEATRE:
                return "ACAMERA_CONTROL_SCENE_MODE_THEATRE";
            case ACAMERA_CONTROL_SCENE_MODE_BEACH:
                return "ACAMERA_CONTROL_SCENE_MODE_BEACH";
            case ACAMERA_CONTROL_SCENE_MODE_SNOW:
                return "ACAMERA_CONTROL_SCENE_MODE_SNOW";
            case ACAMERA_CONTROL_SCENE_MODE_SUNSET:
                return "ACAMERA_CONTROL_SCENE_MODE_SUNSET";
            case ACAMERA_CONTROL_SCENE_MODE_STEADYPHOTO:
                return "ACAMERA_CONTROL_SCENE_MODE_STEADYPHOTO";
            case ACAMERA_CONTROL_SCENE_MODE_FIREWORKS:
                return "ACAMERA_CONTROL_SCENE_MODE_FIREWORKS";
            case ACAMERA_CONTROL_SCENE_MODE_SPORTS:
                return "ACAMERA_CONTROL_SCENE_MODE_SPORTS";
            case ACAMERA_CONTROL_SCENE_MODE_PARTY:
                return "ACAMERA_CONTROL_SCENE_MODE_PARTY";
            case ACAMERA_CONTROL_SCENE_MODE_CANDLELIGHT:
                return "ACAMERA_CONTROL_SCENE_MODE_CANDLELIGHT";
            case ACAMERA_CONTROL_SCENE_MODE_BARCODE:
                return "ACAMERA_CONTROL_SCENE_MODE_BARCODE";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string videoStabilizationModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_OFF:
                return "ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_OFF";
            case ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_ON:
                return "ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_ON";
#if __ANDROID_API__ >= __ANDROID_API_Q__
            case ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_PREVIEW_STABILIZATION:
                return "ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_PREVIEW_STABILIZATION";
#endif
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string aeStateString(int32_t state) {
        switch (state) {
            case ACAMERA_CONTROL_AE_STATE_INACTIVE:
                return "ACAMERA_CONTROL_AE_STATE_INACTIVE";
            case ACAMERA_CONTROL_AE_STATE_SEARCHING:
                return "ACAMERA_CONTROL_AE_STATE_SEARCHING";
            case ACAMERA_CONTROL_AE_STATE_CONVERGED:
                return "ACAMERA_CONTROL_AE_STATE_CONVERGED";
            case ACAMERA_CONTROL_AE_STATE_LOCKED:
                return "ACAMERA_CONTROL_AE_STATE_LOCKED";
            case ACAMERA_CONTROL_AE_STATE_FLASH_REQUIRED:
                return "ACAMERA_CONTROL_AE_STATE_FLASH_REQUIRED";
            case ACAMERA_CONTROL_AE_STATE_PRECAPTURE:
                return "ACAMERA_CONTROL_AE_STATE_PRECAPTURE";
            default:
                return std::string("UNKNOWN(") + std::to_string(state) + ")";
        }
    }

    static std::string afStateString(int32_t state) {
        switch (state) {
            case ACAMERA_CONTROL_AF_STATE_INACTIVE:
                return "ACAMERA_CONTROL_AF_STATE_INACTIVE";
            case ACAMERA_CONTROL_AF_STATE_PASSIVE_SCAN:
                return "ACAMERA_CONTROL_AF_STATE_PASSIVE_SCAN";
            case ACAMERA_CONTROL_AF_STATE_PASSIVE_FOCUSED:
                return "ACAMERA_CONTROL_AF_STATE_PASSIVE_FOCUSED";
            case ACAMERA_CONTROL_AF_STATE_ACTIVE_SCAN:
                return "ACAMERA_CONTROL_AF_STATE_ACTIVE_SCAN";
            case ACAMERA_CONTROL_AF_STATE_FOCUSED_LOCKED:
                return "ACAMERA_CONTROL_AF_STATE_FOCUSED_LOCKED";
            case ACAMERA_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
                return "ACAMERA_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED";
            case ACAMERA_CONTROL_AF_STATE_PASSIVE_UNFOCUSED:
                return "ACAMERA_CONTROL_AF_STATE_PASSIVE_UNFOCUSED";
            default:
                return std::string("UNKNOWN(") + std::to_string(state) + ")";
        }
    }

    static std::string awbStateString(int32_t state) {
        switch (state) {
            case ACAMERA_CONTROL_AWB_STATE_INACTIVE:
                return "ACAMERA_CONTROL_AWB_STATE_INACTIVE";
            case ACAMERA_CONTROL_AWB_STATE_SEARCHING:
                return "ACAMERA_CONTROL_AWB_STATE_SEARCHING";
            case ACAMERA_CONTROL_AWB_STATE_CONVERGED:
                return "ACAMERA_CONTROL_AWB_STATE_CONVERGED";
            case ACAMERA_CONTROL_AWB_STATE_LOCKED:
                return "ACAMERA_CONTROL_AWB_STATE_LOCKED";
            default:
                return std::string("UNKNOWN(") + std::to_string(state) + ")";
        }
    }

    static std::string aeLockAvailableString(int32_t available) {
        switch (available) {
            case ACAMERA_CONTROL_AE_LOCK_AVAILABLE_FALSE:
                return "ACAMERA_CONTROL_AE_LOCK_AVAILABLE_FALSE";
            case ACAMERA_CONTROL_AE_LOCK_AVAILABLE_TRUE:
                return "ACAMERA_CONTROL_AE_LOCK_AVAILABLE_TRUE";
            default:
                return std::string("UNKNOWN(") + std::to_string(available) + ")";
        }
    }

    static std::string awbLockAvailableString(int32_t available) {
        switch (available) {
            case ACAMERA_CONTROL_AWB_LOCK_AVAILABLE_FALSE:
                return "ACAMERA_CONTROL_AWB_LOCK_AVAILABLE_FALSE";
            case ACAMERA_CONTROL_AWB_LOCK_AVAILABLE_TRUE:
                return "ACAMERA_CONTROL_AWB_LOCK_AVAILABLE_TRUE";
            default:
                return std::string("UNKNOWN(") + std::to_string(available) + ")";
        }
    }

    static std::string enableZSLString(int32_t enable) {
        switch (enable) {
            case ACAMERA_CONTROL_ENABLE_ZSL_FALSE:
                return "ACAMERA_CONTROL_ENABLE_ZSL_FALSE";
            case ACAMERA_CONTROL_ENABLE_ZSL_TRUE:
                return "ACAMERA_CONTROL_ENABLE_ZSL_TRUE";
            default:
                return std::string("UNKNOWN(") + std::to_string(enable) + ")";
        }
    }

    static std::string afSceneChangeString(int32_t change) {
        switch (change) {
            case ACAMERA_CONTROL_AF_SCENE_CHANGE_NOT_DETECTED:
                return "ACAMERA_CONTROL_AF_SCENE_CHANGE_NOT_DETECTED";
            case ACAMERA_CONTROL_AF_SCENE_CHANGE_DETECTED:
                return "ACAMERA_CONTROL_AF_SCENE_CHANGE_DETECTED";
            default:
                return std::string("UNKNOWN(") + std::to_string(change) + ")";
        }
    }

    static std::string extendedSceneModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_CONTROL_EXTENDED_SCENE_MODE_DISABLED:
                return "ACAMERA_CONTROL_EXTENDED_SCENE_MODE_DISABLED";
            case ACAMERA_CONTROL_EXTENDED_SCENE_MODE_BOKEH_STILL_CAPTURE:
                return "ACAMERA_CONTROL_EXTENDED_SCENE_MODE_BOKEH_STILL_CAPTURE";
            case ACAMERA_CONTROL_EXTENDED_SCENE_MODE_BOKEH_CONTINUOUS:
                return "ACAMERA_CONTROL_EXTENDED_SCENE_MODE_BOKEH_CONTINUOUS";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string edgeModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_EDGE_MODE_OFF:
                return "ACAMERA_EDGE_MODE_OFF";
            case ACAMERA_EDGE_MODE_FAST:
                return "ACAMERA_EDGE_MODE_FAST";
            case ACAMERA_EDGE_MODE_HIGH_QUALITY:
                return "ACAMERA_EDGE_MODE_HIGH_QUALITY";
            case ACAMERA_EDGE_MODE_ZERO_SHUTTER_LAG:
                return "ACAMERA_EDGE_MODE_ZERO_SHUTTER_LAG";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string flashModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_FLASH_MODE_OFF:
                return "ACAMERA_FLASH_MODE_OFF";
            case ACAMERA_FLASH_MODE_SINGLE:
                return "ACAMERA_FLASH_MODE_SINGLE";
            case ACAMERA_FLASH_MODE_TORCH:
                return "ACAMERA_FLASH_MODE_TORCH";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string flashStateString(int32_t state) {
        switch (state) {
            case ACAMERA_FLASH_STATE_UNAVAILABLE:
                return "ACAMERA_FLASH_STATE_UNAVAILABLE";
            case ACAMERA_FLASH_STATE_CHARGING:
                return "ACAMERA_FLASH_STATE_CHARGING";
            case ACAMERA_FLASH_STATE_READY:
                return "ACAMERA_FLASH_STATE_READY";
            case ACAMERA_FLASH_STATE_FIRED:
                return "ACAMERA_FLASH_STATE_FIRED";
            case ACAMERA_FLASH_STATE_PARTIAL:
                return "ACAMERA_FLASH_STATE_PARTIAL";
            default:
                return std::string("UNKNOWN(") + std::to_string(state) + ")";
        }
    }

    static std::string flashInfoAvailableString(int32_t available) {
        switch (available) {
            case ACAMERA_FLASH_INFO_AVAILABLE_FALSE:
                return "ACAMERA_FLASH_INFO_AVAILABLE_FALSE";
            case ACAMERA_FLASH_INFO_AVAILABLE_TRUE:
                return "ACAMERA_FLASH_INFO_AVAILABLE_TRUE";
            default:
                return std::string("UNKNOWN(") + std::to_string(available) + ")";
        }
    }

    static std::string hotPixelModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_HOT_PIXEL_MODE_OFF:
                return "ACAMERA_HOT_PIXEL_MODE_OFF";
            case ACAMERA_HOT_PIXEL_MODE_FAST:
                return "ACAMERA_HOT_PIXEL_MODE_FAST";
            case ACAMERA_HOT_PIXEL_MODE_HIGH_QUALITY:
                return "ACAMERA_HOT_PIXEL_MODE_HIGH_QUALITY";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string lensOpticalStabilizationModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_LENS_OPTICAL_STABILIZATION_MODE_OFF:
                return "ACAMERA_LENS_OPTICAL_STABILIZATION_MODE_OFF";
            case ACAMERA_LENS_OPTICAL_STABILIZATION_MODE_ON:
                return "ACAMERA_LENS_OPTICAL_STABILIZATION_MODE_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string lensFacingString(int32_t facing) {
        switch (facing) {
            case ACAMERA_LENS_FACING_FRONT:
                return "ACAMERA_LENS_FACING_FRONT";
            case ACAMERA_LENS_FACING_BACK:
                return "ACAMERA_LENS_FACING_BACK";
            case ACAMERA_LENS_FACING_EXTERNAL:
                return "ACAMERA_LENS_FACING_EXTERNAL";
            default:
                return std::string("UNKNOWN(") + std::to_string(facing) + ")";
        }
    }

    static std::string lensStateString(int32_t state) {
        switch (state) {
            case ACAMERA_LENS_STATE_STATIONARY:
                return "ACAMERA_LENS_STATE_STATIONARY";
            case ACAMERA_LENS_STATE_MOVING:
                return "ACAMERA_LENS_STATE_MOVING";
            default:
                return std::string("UNKNOWN(") + std::to_string(state) + ")";
        }
    }

    static std::string lensPoseReferenceString(int32_t reference) {
        switch (reference) {
            case ACAMERA_LENS_POSE_REFERENCE_PRIMARY_CAMERA:
                return "ACAMERA_LENS_POSE_REFERENCE_PRIMARY_CAMERA";
            case ACAMERA_LENS_POSE_REFERENCE_GYROSCOPE:
                return "ACAMERA_LENS_POSE_REFERENCE_GYROSCOPE";
            case ACAMERA_LENS_POSE_REFERENCE_UNDEFINED:
                return "ACAMERA_LENS_POSE_REFERENCE_UNDEFINED";
#if __ANDROID_API__ >= __ANDROID_API_Q__
            case ACAMERA_LENS_POSE_REFERENCE_AUTOMOTIVE:
                return "ACAMERA_LENS_POSE_REFERENCE_AUTOMOTIVE";
#endif
            default:
                return std::string("UNKNOWN(") + std::to_string(reference) + ")";
        }
    }

    static std::string lensInfoFocusDistanceCalibrationString(int32_t calibration) {
        switch (calibration) {
            case ACAMERA_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_UNCALIBRATED:
                return "ACAMERA_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_UNCALIBRATED";
            case ACAMERA_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_APPROXIMATE:
                return "ACAMERA_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_APPROXIMATE";
            case ACAMERA_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_CALIBRATED:
                return "ACAMERA_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_CALIBRATED";
            default:
                return std::string("UNKNOWN(") + std::to_string(calibration) + ")";
        }
    }

    static std::string noiseReductionModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_NOISE_REDUCTION_MODE_OFF:
                return "ACAMERA_NOISE_REDUCTION_MODE_OFF";
            case ACAMERA_NOISE_REDUCTION_MODE_FAST:
                return "ACAMERA_NOISE_REDUCTION_MODE_FAST";
            case ACAMERA_NOISE_REDUCTION_MODE_HIGH_QUALITY:
                return "ACAMERA_NOISE_REDUCTION_MODE_HIGH_QUALITY";
            case ACAMERA_NOISE_REDUCTION_MODE_MINIMAL:
                return "ACAMERA_NOISE_REDUCTION_MODE_MINIMAL";
            case ACAMERA_NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG:
                return "ACAMERA_NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string requestAvailableCapabilitiesString(int32_t capabilities) {
        switch (capabilities) {
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_POST_PROCESSING:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_POST_PROCESSING";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_RAW:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_RAW";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_BURST_CAPTURE:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_BURST_CAPTURE";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MOTION_TRACKING:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MOTION_TRACKING";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_LOGICAL_MULTI_CAMERA:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_LOGICAL_MULTI_CAMERA";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MONOCHROME:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_MONOCHROME";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_SECURE_IMAGE_DATA:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_SECURE_IMAGE_DATA";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_SYSTEM_CAMERA:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_SYSTEM_CAMERA";
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_ULTRA_HIGH_RESOLUTION_SENSOR:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_ULTRA_HIGH_RESOLUTION_SENSOR";
#if __ANDROID_API__ >= __ANDROID_API_Q__
            case ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_STREAM_USE_CASE:
                return "ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_STREAM_USE_CASE";
#endif
            default:
                return std::string("UNKNOWN(") + std::to_string(capabilities) + ")";
        }
    }

    static std::string requestAvailableDynamicRangeProfilesMapString(int32_t profiles) {
        switch (profiles) {
#if __ANDROID_API__ >= __ANDROID_API_Q__
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_STANDARD:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_STANDARD";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_HLG10:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_HLG10";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_HDR10:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_HDR10";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_HDR10_PLUS:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_HDR10_PLUS";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_REF:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_REF";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_REF_PO:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_REF_PO";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_OEM:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_OEM";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_OEM_PO:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_10B_HDR_OEM_PO";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_REF:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_REF";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_REF_PO:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_REF_PO";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_OEM:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_OEM";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_OEM_PO:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_DOLBY_VISION_8B_HDR_OEM_PO";
            case ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_MAX:
                return "ACAMERA_REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES_MAP_MAX";
#endif
            default:
                return std::string("UNKNOWN(") + std::to_string(profiles) + ")";
        }
    }

    static std::string streamConfigurationMapString(int32_t configuration) {
        switch (configuration) {
            case ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT";
            case ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_INPUT:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_INPUT";
            default:
                return std::string("UNKNOWN(") + std::to_string(configuration) + ")";
        }
    }

    static std::string scalerCroppingTypeString(int32_t type) {
        switch (type) {
            case ACAMERA_SCALER_CROPPING_TYPE_CENTER_ONLY:
                return "ACAMERA_SCALER_CROPPING_TYPE_CENTER_ONLY";
            case ACAMERA_SCALER_CROPPING_TYPE_FREEFORM:
                return "ACAMERA_SCALER_CROPPING_TYPE_FREEFORM";
            default:
                return std::string("UNKNOWN(") + std::to_string(type) + ")";
        }
    }

    static std::string scalerRecommendedStreamConfigurationsMapString(int32_t configuration) {
        switch (configuration) {
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_PREVIEW:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_PREVIEW";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_RECORD:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_RECORD";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_VIDEO_SNAPSHOT:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_VIDEO_SNAPSHOT";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_SNAPSHOT:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_SNAPSHOT";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_ZSL:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_ZSL";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_RAW:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_RAW";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_LOW_LATENCY_SNAPSHOT:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_LOW_LATENCY_SNAPSHOT";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_PUBLIC_END:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_PUBLIC_END";
#if __ANDROID_API__ >= __ANDROID_API_Q__
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_10BIT_OUTPUT:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_10BIT_OUTPUT";
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_PUBLIC_END_3_8:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_PUBLIC_END_3_8";
#endif
            case ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_VENDOR_START:
                return "ACAMERA_SCALER_AVAILABLE_RECOMMENDED_STREAM_CONFIGURATIONS_VENDOR_START";
            default:
                return std::string("UNKNOWN(") + std::to_string(configuration) + ")";
        }
    }

    static std::string scalarRotateAndCropString(int32_t rotate) {
        switch (rotate) {
            case ACAMERA_SCALER_ROTATE_AND_CROP_NONE:
                return "ACAMERA_SCALER_ROTATE_AND_CROP_NONE";
            case ACAMERA_SCALER_ROTATE_AND_CROP_90:
                return "ACAMERA_SCALER_ROTATE_AND_CROP_90";
            case ACAMERA_SCALER_ROTATE_AND_CROP_180:
                return "ACAMERA_SCALER_ROTATE_AND_CROP_180";
            case ACAMERA_SCALER_ROTATE_AND_CROP_270:
                return "ACAMERA_SCALER_ROTATE_AND_CROP_270";
            default:
                return std::string("UNKNOWN(") + std::to_string(rotate) + ")";
        }
    }

    static std::string scalerPhysicalCameraMultiResolutionStreamConfigurationString(int32_t configuration) {
        switch (configuration) {
            case ACAMERA_SCALER_PHYSICAL_CAMERA_MULTI_RESOLUTION_STREAM_CONFIGURATIONS_OUTPUT:
                return "ACAMERA_SCALER_PHYSICAL_CAMERA_MULTI_RESOLUTION_STREAM_CONFIGURATIONS_OUTPUT";
            case ACAMERA_SCALER_PHYSICAL_CAMERA_MULTI_RESOLUTION_STREAM_CONFIGURATIONS_INPUT:
                return "ACAMERA_SCALER_PHYSICAL_CAMERA_MULTI_RESOLUTION_STREAM_CONFIGURATIONS_INPUT";
            default:
                return std::string("UNKNOWN(") + std::to_string(configuration) + ")";
        }
    }

    static std::string scalerAvailableStreamUseCaseString(int32_t useCase) {
        switch (useCase) {
#if __ANDROID_API__ >= __ANDROID_API_Q__
            case ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_DEFAULT:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_DEFAULT";
            case ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_PREVIEW:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_PREVIEW";
            case ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_STILL_CAPTURE:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_STILL_CAPTURE";
            case ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_VIDEO_RECORD:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_VIDEO_RECORD";
            case ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_PREVIEW_VIDEO_STILL:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_PREVIEW_VIDEO_STILL";
            case ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_VIDEO_CALL:
                return "ACAMERA_SCALER_AVAILABLE_STREAM_USE_CASES_VIDEO_CALL";
#endif

            default:
                return std::string("UNKNOWN(") + std::to_string(useCase) + ")";
        }
    }

    static std::string sensorInfoColorFilterArrangementString(int32_t arrangement) {
        switch (arrangement) {
            case ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGGB:
                return "ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGGB";
            case ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GRBG:
                return "ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GRBG";
            case ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GBRG:
                return "ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GBRG";
            case ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_BGGR:
                return "ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_BGGR";
            case ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGB:
                return "ACAMERA_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGB";
            default:
                return std::string("UNKNOWN(") + std::to_string(arrangement) + ")";
        }
    }

    static std::string sensorInfoTimestampSourceString(int32_t source) {
        switch (source) {
            case ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN:
                return "ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN";
            case ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME:
                return "ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME";
            default:
                return std::string("UNKNOWN(") + std::to_string(source) + ")";
        }
    }

    static std::string sensorInfoLensShadingAppliedString(int32_t applied) {
        switch (applied) {
            case ACAMERA_SENSOR_INFO_LENS_SHADING_APPLIED_FALSE:
                return "ACAMERA_SENSOR_INFO_LENS_SHADING_APPLIED_FALSE";
            case ACAMERA_SENSOR_INFO_LENS_SHADING_APPLIED_TRUE:
                return "ACAMERA_SENSOR_INFO_LENS_SHADING_APPLIED_TRUE";
            default:
                return std::string("UNKNOWN(") + std::to_string(applied) + ")";
        }
    }

    static std::string shadingModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_SHADING_MODE_OFF:
                return "ACAMERA_SHADING_MODE_OFF";
            case ACAMERA_SHADING_MODE_FAST:
                return "ACAMERA_SHADING_MODE_FAST";
            case ACAMERA_SHADING_MODE_HIGH_QUALITY:
                return "ACAMERA_SHADING_MODE_HIGH_QUALITY";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string statisticsFaceDetectModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_STATISTICS_FACE_DETECT_MODE_OFF:
                return "ACAMERA_STATISTICS_FACE_DETECT_MODE_OFF";
            case ACAMERA_STATISTICS_FACE_DETECT_MODE_SIMPLE:
                return "ACAMERA_STATISTICS_FACE_DETECT_MODE_SIMPLE";
            case ACAMERA_STATISTICS_FACE_DETECT_MODE_FULL:
                return "ACAMERA_STATISTICS_FACE_DETECT_MODE_FULL";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string statisticsHotPixelMapModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_STATISTICS_HOT_PIXEL_MAP_MODE_OFF:
                return "ACAMERA_STATISTICS_HOT_PIXEL_MAP_MODE_OFF";
            case ACAMERA_STATISTICS_HOT_PIXEL_MAP_MODE_ON:
                return "ACAMERA_STATISTICS_HOT_PIXEL_MAP_MODE_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string statisticsLensShadingMapModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_OFF:
                return "ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_OFF";
            case ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_ON:
                return "ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string statisticsSceneFlickerString(int32_t flicker) {
        switch (flicker) {
            case ACAMERA_STATISTICS_SCENE_FLICKER_NONE:
                return "ACAMERA_STATISTICS_SCENE_FLICKER_NONE";
            case ACAMERA_STATISTICS_SCENE_FLICKER_50HZ:
                return "ACAMERA_STATISTICS_SCENE_FLICKER_50HZ";
            case ACAMERA_STATISTICS_SCENE_FLICKER_60HZ:
                return "ACAMERA_STATISTICS_SCENE_FLICKER_60HZ";
            default:
                return std::string("UNKNOWN(") + std::to_string(flicker) + ")";
        }
    }

    static std::string lensShadingMapModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_OFF:
                return "ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_OFF";
            case ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_ON:
                return "ACAMERA_STATISTICS_LENS_SHADING_MAP_MODE_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string statisticsOISDataMode(int32_t mode) {
        switch (mode) {
            case ACAMERA_STATISTICS_OIS_DATA_MODE_OFF:
                return "ACAMERA_STATISTICS_OIS_DATA_MODE_OFF";
            case ACAMERA_STATISTICS_OIS_DATA_MODE_ON:
                return "ACAMERA_STATISTICS_OIS_DATA_MODE_ON";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string tonemapMode(int32_t mode) {
        switch (mode) {
            case ACAMERA_TONEMAP_MODE_CONTRAST_CURVE:
                return "ACAMERA_TONEMAP_MODE_CONTRAST_CURVE";
            case ACAMERA_TONEMAP_MODE_FAST:
                return "ACAMERA_TONEMAP_MODE_FAST";
            case ACAMERA_TONEMAP_MODE_HIGH_QUALITY:
                return "ACAMERA_TONEMAP_MODE_HIGH_QUALITY";
            case ACAMERA_TONEMAP_MODE_GAMMA_VALUE:
                return "ACAMERA_TONEMAP_MODE_GAMMA_VALUE";
            case ACAMERA_TONEMAP_MODE_PRESET_CURVE:
                return "ACAMERA_TONEMAP_MODE_PRESET_CURVE";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string tonemapPresetCurveMode(int32_t mode) {
        switch (mode) {
            case ACAMERA_TONEMAP_PRESET_CURVE_SRGB:
                return "ACAMERA_TONEMAP_PRESET_CURVE_SRGB";
            case ACAMERA_TONEMAP_PRESET_CURVE_REC709:
                return "ACAMERA_TONEMAP_PRESET_CURVE_REC709";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }

    static std::string supportedHardwareLevelString(int32_t level) {
        switch (level) {
            case ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED:
                return "ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED";
            case ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_FULL:
                return "ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_FULL";
            case ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY:
                return "ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY";
            case ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_3:
                return "ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL_3";
            default:
                return std::string("UNKNOWN(") + std::to_string(level) + ")";
        }
    }

    static std::string distortionCorrectionModeString(int32_t mode) {
        switch (mode) {
            case ACAMERA_DISTORTION_CORRECTION_MODE_OFF:
                return "ACAMERA_DISTORTION_CORRECTION_MODE_OFF";
            case ACAMERA_DISTORTION_CORRECTION_MODE_FAST:
                return "ACAMERA_DISTORTION_CORRECTION_MODE_FAST";
            case ACAMERA_DISTORTION_CORRECTION_MODE_HIGH_QUALITY:
                return "ACAMERA_DISTORTION_CORRECTION_MODE_HIGH_QUALITY";
            default:
                return std::string("UNKNOWN(") + std::to_string(mode) + ")";
        }
    }
};

NAMESPACE_END
