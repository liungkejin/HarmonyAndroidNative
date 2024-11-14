//
// Created on 2024/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "NIRImage.h"
#include "NNativeWindow.h"
#include <Namespace.h>
#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/image_receiver_native.h>

NAMESPACE_DEFAULT

class NativeImageReader;

class NativeImageListener {
public:
    virtual void onImageAvailable(NativeImageReader &ir) = 0;
};

class NativeImageReader {
public:
    NativeImageReader() = default;
    ~NativeImageReader() {
        release();
    }

    int create(uint32_t width, uint32_t height, int32_t capacity);

    inline int width() const { return m_width; }
    
    inline int height() const { return m_height; }
    
    inline int capacity() const { return m_capacity; }
    
    NNativeWindow *surface();

    uint64_t surfaceId();
    
    const char *surfaceIdString();

    NIRImage readLastImage();

    NIRImage readNextImage();

    int setImageListener(NativeImageListener *listener);
    
    void release();
    
private:
    OH_ImageReceiverNative *m_native = nullptr;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    int32_t m_capacity = 0;
    uint64_t m_surface_id = 0;
    char m_surface_id_str[65] = {0};
    
    NNativeWindow *m_window = nullptr;
};

NAMESPACE_END
