//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "Namespace.h"
#include <ohcamera/camera.h>

NAMESPACE_DEFAULT

typedef int CamErrorCode;

#define WT_CAM_ERROR_UNKNOWN -1101
#define WT_CAM_ERROR_NULL -1102
#define WT_CAM_ERROR_WRANING -1103

class CamUtils {
public:
    static const char *errString(const CamErrorCode e);
    static const char *statusStr(const Camera_Status s);
    static const char *positionStr(const Camera_Position p);
    static const char *typeStr(const Camera_Type t);
    static const char *connectionStr(const Camera_Connection c);
    static const char *formatStr(const Camera_Format f);
    static const char *flashModeStr(const Camera_FlashMode f);
    static const char *exposureModeStr(const Camera_ExposureMode e);
    static const char *focusModeStr(const Camera_FocusMode f);
    static const char *focusStateStr(const Camera_FocusState f);
    static const char *videoStabModeStr(const Camera_VideoStabilizationMode m);
    static const char *imageRotationStr(const Camera_ImageRotation r);
    static const char *qualityLevelStr(const Camera_QualityLevel q);
    static const char *metadataObjectType(const Camera_MetadataObjectType t);
};

NAMESPACE_END
