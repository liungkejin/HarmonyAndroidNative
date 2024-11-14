//
// Created on 2024/8/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Object.h"
#include "ImageUtils.h"
#include <cstdint>
#include <multimedia/image_framework/image/pixelmap_native.h>

NAMESPACE_DEFAULT

class NPixelmap;

/// initialization options
class NPixelmapInitOptions : Object {
    friend class NPixelmap;

public:
    NPixelmapInitOptions() {
        Image_ErrorCode code = OH_PixelmapInitializationOptions_Create(&m_options);
        _ERROR_IF(code != IMAGE_SUCCESS, "PixelmapInitialization create failed: %s", ImageUtils::errorString(code));
    }

    NPixelmapInitOptions(const NPixelmapInitOptions &other) : m_options(other.m_options), Object(other) {}

    ~NPixelmapInitOptions() {
        if (m_options && no_reference()) {
            OH_PixelmapInitializationOptions_Release(m_options);
            m_options = nullptr;
        }
    }

    inline bool valid() const { return m_options != nullptr; }

    uint32_t width() const {
        _ERROR_RETURN_IF(!m_options, 0, "PixelmapInitOptions invalid!");
        uint32_t w = 0;
        Image_ErrorCode code = OH_PixelmapInitializationOptions_GetWidth(m_options, &w);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get width failed: %s", ImageUtils::errorString(code));
        return w;
    }

    void setWidth(uint32_t width) {
        _ERROR_RETURN_IF(!m_options, void(), "PixelmapInitOptions invalid!");
        Image_ErrorCode code = OH_PixelmapInitializationOptions_SetWidth(m_options, width);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set width failed: %s", ImageUtils::errorString(code));
    }

    uint32_t height() const {
        _ERROR_RETURN_IF(!m_options, 0, "PixelmapInitOptions invalid!");
        uint32_t h = 0;
        Image_ErrorCode code = OH_PixelmapInitializationOptions_GetHeight(m_options, &h);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get height failed: %s", ImageUtils::errorString(code));
        return h;
    }

    void setHeight(uint32_t height) {
        _ERROR_RETURN_IF(!m_options, void(), "PixelmapInitOptions invalid!");
        Image_ErrorCode code = OH_PixelmapInitializationOptions_SetHeight(m_options, height);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set height failed: %s", ImageUtils::errorString(code));
    }

    int32_t pixelFormat() const {
        _ERROR_RETURN_IF(!m_options, 0, "PixelmapInitOptions invalid!");
        int32_t format = 0;
        Image_ErrorCode code = OH_PixelmapInitializationOptions_GetPixelFormat(m_options, &format);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get pixel format failed: %s", ImageUtils::errorString(code));
        return format;
    }

    void setPixelFormat(int32_t pixelFormat) {
        _ERROR_RETURN_IF(!m_options, void(), "PixelmapInitOptions invalid!");
        Image_ErrorCode code = OH_PixelmapInitializationOptions_SetPixelFormat(m_options, pixelFormat);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set pixel format failed: %s", ImageUtils::errorString(code));
    }

    int32_t srcPixelFormat() const {
        _ERROR_RETURN_IF(!m_options, 0, "PixelmapInitOptions invalid!");
        int32_t format = 0;
        Image_ErrorCode code = OH_PixelmapInitializationOptions_GetSrcPixelFormat(m_options, &format);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get source pixel format failed: %s", ImageUtils::errorString(code));
        return format;
    }

    void setSrcPixelFormat(int32_t srcPixelFormat) {
        _ERROR_RETURN_IF(!m_options, void(), "PixelmapInitOptions invalid!");
        Image_ErrorCode code = OH_PixelmapInitializationOptions_SetSrcPixelFormat(m_options, srcPixelFormat);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set source pixel format failed: %s", ImageUtils::errorString(code));
    }

//    int32_t rowStride() const {
//        _ERROR_RETURN_IF(!m_options, 0, "PixelmapInitOptions invalid!");
//        int32_t stride = 0;
//        Image_ErrorCode code = OH_PixelmapInitializationOptions_GetRowStride(m_options, &stride);
//        _ERROR_IF(code != IMAGE_SUCCESS, "Get row stride failed: %s", ImageUtils::errorString(code));
//        return stride;
//    }
//
//    void setRowStride(int32_t rowStride) {
//        _ERROR_RETURN_IF(!m_options, void(), "PixelmapInitOptions invalid!");
//        Image_ErrorCode code = OH_PixelmapInitializationOptions_SetRowStride(m_options, rowStride);
//        _ERROR_IF(code != IMAGE_SUCCESS, "Set row stride failed: %s", ImageUtils::errorString(code));
//    }

    int32_t alphaType() const {
        _ERROR_RETURN_IF(!m_options, 0, "PixelmapInitOptions invalid!");
        int32_t type = 0;
        Image_ErrorCode code = OH_PixelmapInitializationOptions_GetAlphaType(m_options, &type);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get alpha type failed: %s", ImageUtils::errorString(code));
        return type;
    }

    void setAlphaType(int32_t alphaType) {
        _ERROR_RETURN_IF(!m_options, void(), "PixelmapInitOptions invalid!");
        Image_ErrorCode code = OH_PixelmapInitializationOptions_SetAlphaType(m_options, alphaType);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set alpha type failed: %s", ImageUtils::errorString(code));
    }
    
public:
    std::string toString() const {
        std::stringstream ss;
        ss << "{ size(" << width() << " x " << height() << "), "; 
        ss << "pixelFormat: " << ImageUtils::formatString(pixelFormat()) << ", ";
        ss << "srcPixelFormat: " << ImageUtils::formatString(srcPixelFormat()) << ", ";
        ss << "alphaType: " << alphaType() << " }";
        return ss.str();
    }

private:
    OH_Pixelmap_InitializationOptions *m_options = nullptr;
};

/// image info
class NPixelmapInfo : Object {
    friend class NPixelmap;

public:
    NPixelmapInfo() {}

    NPixelmapInfo(OH_Pixelmap_ImageInfo *info) : m_info(info) {}

    NPixelmapInfo(const NPixelmapInfo &info) : m_info(info.m_info), Object(info) {}

    ~NPixelmapInfo() {
        if (m_info && no_reference()) {
            OH_PixelmapImageInfo_Release(m_info);
            m_info = nullptr;
        }
    }

    inline bool valid() const { return m_info != nullptr; }

    uint32_t width() const {
        _ERROR_RETURN_IF(!m_info, 0, "PixelmapInfo invalid!");
        uint32_t w = 0;
        Image_ErrorCode code = OH_PixelmapImageInfo_GetWidth(m_info, &w);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get width failed: %s", ImageUtils::errorString(code));
        return w;
    }

    uint32_t height() const {
        _ERROR_RETURN_IF(!m_info, 0, "PixelmapInfo invalid!");
        uint32_t h = 0;
        Image_ErrorCode code = OH_PixelmapImageInfo_GetHeight(m_info, &h);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get height failed: %s", ImageUtils::errorString(code));
        return h;
    }

    uint32_t rowStride() const {
        _ERROR_RETURN_IF(!m_info, 0, "PixelmapInfo invalid!");
        uint32_t stride = 0;
        Image_ErrorCode code = OH_PixelmapImageInfo_GetRowStride(m_info, &stride);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get row stride failed: %s", ImageUtils::errorString(code));
        return stride;
    }

    int32_t pixelFormat() const {
        _ERROR_RETURN_IF(!m_info, 0, "PixelmapInfo invalid!");
        int32_t format = 0;
        Image_ErrorCode code = OH_PixelmapImageInfo_GetPixelFormat(m_info, &format);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get pixel format failed: %s", ImageUtils::errorString(code));
        return format;
    }

    int32_t alphaType() const {
        _ERROR_RETURN_IF(!m_info, 0, "PixelmapInfo invalid!");
        int32_t type = 0;
        Image_ErrorCode code = OH_PixelmapImageInfo_GetAlphaType(m_info, &type);
        _ERROR_IF(code != IMAGE_SUCCESS, "Get alpha type failed: %s", ImageUtils::errorString(code));
        return type;
    }

    bool isHDR() const {
        _ERROR_RETURN_IF(!m_info, false, "PixelmapInfo invalid!");
        bool isHDR = false;
        Image_ErrorCode code = OH_PixelmapImageInfo_GetDynamicRange(m_info, &isHDR);
        _ERROR_IF(code != IMAGE_SUCCESS, "Is HDR failed: %s", ImageUtils::errorString(code));
        return isHDR;
    }
    
public:
    std::string toString() const {
        std::stringstream ss;
        ss << "{ size(" << width() << " x " << height() << "), "; 
        ss << "rowStride: " << rowStride() << ", ";
        ss << "pixelFormat: " << ImageUtils::formatString(pixelFormat()) << ", ";
        ss << "alphaType: " << alphaType() << ", ";
        ss << "isHDR: " << (isHDR()? "true" : "false") << " }";
        return ss.str();
    }

private:
    OH_Pixelmap_ImageInfo *m_info = nullptr;
};

/// Pixel Map
class NImagePacker;
class NPixelmap : Object {
    friend class NImagePacker;
public:
    NPixelmap(const NPixelmapInitOptions options) {
        Image_ErrorCode code = OH_PixelmapNative_CreateEmptyPixelmap(options.m_options, &m_native);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap create failed: %s, options: %s", ImageUtils::errorString(code), options.toString());
    }

    NPixelmap(uint8_t *data, size_t size, const NPixelmapInitOptions options) {
        Image_ErrorCode code = OH_PixelmapNative_CreatePixelmap(data, size, options.m_options, &m_native);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap create from data failed: %s, options: %s", ImageUtils::errorString(code), options.toString());
    }

    NPixelmap(OH_PixelmapNative *native) : m_native(native) {}
    
    NPixelmap(const NPixelmap &other) : m_native(other.m_native), Object(other) {}

    ~NPixelmap() {
        if (m_native && no_reference()) {
            OH_PixelmapNative_Release(m_native);
            m_native = nullptr;
        }
    }

    bool valid() const { return m_native != nullptr; }
    
    Image_ErrorCode readPixels(uint8_t *output, size_t &outSize) {
        _ERROR_RETURN_IF(!m_native, IMAGE_UNKNOWN_ERROR, "Pixelmap invalid!");
        Image_ErrorCode code = OH_PixelmapNative_ReadPixels(m_native, output, &outSize);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap read pixels failed: %s", ImageUtils::errorString(code));
        return code;
    }
    
    Image_ErrorCode writePixels(uint8_t *input, size_t size) {
        _ERROR_RETURN_IF(!m_native, IMAGE_UNKNOWN_ERROR, "Pixelmap invalid!");
        Image_ErrorCode code = OH_PixelmapNative_WritePixels(m_native, input, size);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap write pixels failed: %s", ImageUtils::errorString(code));
        return code;
    }
    
    NPixelmapInfo info() const {
        _ERROR_RETURN_IF(!m_native, NPixelmapInfo(), "Pixelmap invalid!");
        
        OH_Pixelmap_ImageInfo *imageInfo = nullptr;
        Image_ErrorCode code = OH_PixelmapImageInfo_Create(&imageInfo);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, NPixelmapInfo(), "PixelmapInfo create failed: %s", ImageUtils::errorString(code));
        
        code = OH_PixelmapNative_GetImageInfo(m_native, imageInfo);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap get image info failed: %s", ImageUtils::errorString(code));
        return NPixelmapInfo(imageInfo);
    }
    
    void toSdr() {
        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
        
        Image_ErrorCode code = OH_PixelmapNative_ToSdr(m_native);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap to SDR failed: %s", ImageUtils::errorString(code));
    }
    
    void setOpacity(float opacity) {
        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
        
        Image_ErrorCode code = OH_PixelmapNative_Opacity(m_native, opacity);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap set opacity failed: %s", ImageUtils::errorString(code));
    }
    
    void scale(float sx, float sy) {
        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
        
        Image_ErrorCode code = OH_PixelmapNative_Scale(m_native, sx, sy);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap scale failed: %s", ImageUtils::errorString(code));
    }
    
//    void scale(float sx, float sy, OH_PixelmapNative_AntiAliasingLevel level) {
//        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
//        
//        Image_ErrorCode code = OH_PixelmapNative_ScaleWithAntiAliasing(m_native, sx, sy, level);
//        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap scale with antialiasing level failed: %s", ImageUtils::errorString(code));
//    }

    void translate(float x, float y) {
        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
        
        Image_ErrorCode code = OH_PixelmapNative_Translate(m_native, x, y);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap translate failed: %s", ImageUtils::errorString(code));
    }
    
    void flip(bool h, bool v) {
        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
        
        Image_ErrorCode code = OH_PixelmapNative_Flip(m_native, h, v);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap flip failed: %s", ImageUtils::errorString(code));
    }
    
    void crop(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
        _ERROR_RETURN_IF(!m_native, void(), "Pixelmap invalid!");
        Image_Region region = {
            .x = x,
            .y = y,
            .width = w,
            .height = h,
        };
        Image_ErrorCode code = OH_PixelmapNative_Crop(m_native, &region);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap crop failed: %s", ImageUtils::errorString(code));
    }
    
    void convertToAlphaFormat(NPixelmap dst, bool isPremul) {
        _ERROR_RETURN_IF(!m_native ||!dst.m_native, void(), "Pixelmaps invalid!");
        
        Image_ErrorCode code = OH_PixelmapNative_ConvertAlphaFormat(m_native, dst.m_native, isPremul);
        _ERROR_IF(code != IMAGE_SUCCESS, "Pixelmap convert to alpha format failed: %s", ImageUtils::errorString(code));
    }

private:
    OH_PixelmapNative *m_native = nullptr;
};

NAMESPACE_END
