//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "Namespace.h"
#include "common/Log.h"
#include <ohcamera/camera.h>

NAMESPACE_DEFAULT

typedef int CamErrorCode;

#define WT_CAM_ERROR_UNKNOWN (-1101)
#define WT_CAM_ERROR_NULL (-1102)
#define WT_CAM_ERROR_WRANING (-1103)

class CamUtils {
public:
    static std::string errString(CamErrorCode e);
    static std::string statusStr(Camera_Status s);
    static std::string positionStr(Camera_Position p);
    static std::string typeStr(int t);
    static std::string connectionStr(Camera_Connection c);
    static std::string formatStr(Camera_Format f);
    static std::string flashModeStr(Camera_FlashMode f);
    static std::string exposureModeStr(Camera_ExposureMode e);
    static std::string focusModeStr(Camera_FocusMode f);
    static std::string focusStateStr(Camera_FocusState f);
    static std::string videoStabModeStr(Camera_VideoStabilizationMode m);
    static std::string imageRotationStr(Camera_ImageRotation r);
    static std::string qualityLevelStr(Camera_QualityLevel q);
    static std::string metadataObjectType(Camera_MetadataObjectType t);
};

NAMESPACE_END
