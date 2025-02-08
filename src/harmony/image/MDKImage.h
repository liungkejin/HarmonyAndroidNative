//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <ZNamespace.h>
#include <common/Object.h>
#include <common/Log.h>
#include <multimedia/image_framework/image_mdk.h>

NAMESPACE_DEFAULT

typedef ImageNative MDKImageNative;
typedef struct OhosImageRect MDKImageRect;
typedef struct OhosImageSize MDKImageSize;
typedef struct OhosImageComponent MDKImageComponent;

// 封装 ImageNative
class MDKImage : Object {
public:
    explicit MDKImage(MDKImageNative *img) : m_native(img) {}

    MDKImage(napi_env env, napi_value value) {
        m_native = OH_Image_InitImageNative(env, value);
        _FATAL_IF(m_native == nullptr, "Failed to get ImageNative from %p", value);
    }

    ~MDKImage() {
        if (no_reference() && m_native) {
            OH_Image_Release(m_native);
            m_native = nullptr;
        }
    }

public:
    inline bool valid() const { return m_native != nullptr; }
    
    /**
     * 获取裁剪的矩形区域
     * @return rect 返回裁剪的矩形区域
     */
    MDKImageRect getClipRect() {
        _FATAL_IF(m_native == nullptr, "ImageNative is null while call getClipRect()")
        
        MDKImageRect rect;
        int32_t error = OH_Image_ClipRect(m_native, &rect);
        _ERROR_IF(error != 0, "OH_Image_ClipRect Failed: %d", error);
        return rect;
    }

    /**
     * 获取图片的大小
     * @param size 返回图片的大小
     * @return 错误码
     */
    MDKImageSize getSize() {
        _FATAL_IF(m_native == nullptr, "ImageNative is null while call getSize()")
        
        MDKImageSize size;
        int32_t error = OH_Image_Size(m_native, &size);
        _ERROR_IF(error != 0, "OH_Image_Size Failed: %d", error);
        return size;
    }

    /**
     * 获取图片的像素格式
     * @param format 返回像素格式
     * @return 错误码
     */
    int getFormat() {
        _FATAL_IF(m_native == nullptr, "ImageNative is null while call getFormat()")
        
        int32_t format;
        int32_t error = OH_Image_Format(m_native, &format);
        _ERROR_IF(error != 0, "OH_Image_Format Failed: %d", error);
        return format;
    }

    /**
     * 获取图片的像素数据
     * @param componentType
     * @param component 实际的像素数据
     * @return 错误码
     */
    MDKImageComponent getComponent(int componentType) {
        _FATAL_IF(m_native == nullptr, "ImageNative is null while call getComponent()")
        
        MDKImageComponent component;
        int32_t error = OH_Image_GetComponent(m_native, componentType, &component);
        _ERROR_IF(error != 0, "OH_Image_GetComponent Failed: %d", error);
        return component;
    }

private:
    // native层对象
    ImageNative *m_native = nullptr;
};

NAMESPACE_END
