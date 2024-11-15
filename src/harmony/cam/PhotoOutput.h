//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "Namespace.h"
#include "common/Object.h"
#include "common/utils/BaseUtils.h"
#include "CamDevice.h"
#include "harmony/image/NIRImage.h"
#include "harmony/masset/MediaAsset.h"
#include <cstdint>
#include <ohcamera/photo_output.h>

NAMESPACE_DEFAULT

class PhotoOutput;
class CaptureSession;

class NativePhoto : public Object {
public:
    NativePhoto(OH_PhotoNative *native, bool own) : m_native(native), m_own(own) {}
    NativePhoto(const NativePhoto& o) : m_native(o.m_native), m_own(o.m_own), Object(o) {}
    ~NativePhoto() {
        if (no_reference() && m_native && m_own) {
            OH_PhotoNative_Release(m_native);
            m_native = nullptr;
            
            DELETE_TO_NULL(m_image);
        }
    }
    
public:
    NIRImage& getImage() {
        if (m_image == nullptr) {
            OH_ImageNative *image = nullptr;
            CamErrorCode error = OH_PhotoNative_GetMainImage(m_native, &image);
            _ERROR_IF(error, "GetMainImage failed: %s", CamUtils::errString(error));
            m_image = new NIRImage(image);
        }
        return *m_image;
    }

private:
    bool m_own;
    OH_PhotoNative *m_native;
    NIRImage *m_image = nullptr;
};

class PhotoCallback {
public:
    virtual void onFrameStart(PhotoOutput &output) {}
    
    virtual void onFrameShutter(PhotoOutput &output, Camera_FrameShutterInfo *info) {}
    
    virtual void onFrameEnd(PhotoOutput &output, int32_t frameCount) {}
    
    virtual void onError(PhotoOutput &output, Camera_ErrorCode err) {}
    
    //////
    
    virtual void onCaptureReady(PhotoOutput &output) {}
    
    virtual void onEstimatedCaptureDuration(PhotoOutput &output, int64_t duration) {}
    
    virtual void onPhotoAvailable(PhotoOutput &output, NativePhoto& photo) {}
    
    virtual void onPhotoAssetAvailable(PhotoOutput &output, MediaAsset *asset) {}
};

typedef Camera_PhotoCaptureSetting PhotoCaptureSetting;

class PhotoOutput {
    friend class CaptureSession;

public:
    PhotoOutput(Camera_PhotoOutput *p, const CamProfile *profile) : m_output(p), m_profile(*profile) {}
    PhotoOutput(const PhotoOutput &other) : m_output(other.m_output), m_profile(other.m_profile) {}
    ~PhotoOutput() { release(); }
    
    inline const CamProfile& profile() const { return m_profile;}

    CamErrorCode setCallback(PhotoCallback *callback);
    
    void setShutterCallbackEnable(bool enable);
    
    void setCaptureReadyCallbackEnable(bool enable);
    
    void setEstimatedDurationCallbackEnable(bool enable);
    
    void setNativePhotoCallbackEnable(bool enable);
    
    void setPhotoAssetCallbackEnable(bool enable);

    CamErrorCode capture(const PhotoCaptureSetting *settings = nullptr);

    bool isMirrorSupported();

private:
    CamErrorCode release();

private:
    CamProfile m_profile;
    Camera_PhotoOutput *m_output;
};

NAMESPACE_END
