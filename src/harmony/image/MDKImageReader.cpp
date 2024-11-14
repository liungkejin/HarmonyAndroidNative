//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "MDKImageReader.h"

NAMESPACE_DEFAULT

int MDKImageReader::attach(napi_env env, napi_value js_ir_obj) {
    _ERROR_RETURN_IF(!js_ir_obj, ERROR_CODE_NULLPTR, "MDKImageReader attach failed: js_ir_obj is null.");

    if (m_native) {
        release();
    }

    m_jni_env = env;
    m_native = OH_Image_Receiver_InitImageReceiverNative(m_jni_env, js_ir_obj);
    _ERROR_RETURN_IF(!m_native, ERROR_CODE_NULLPTR, "InitImageReceiverNative failed.");

    return IMAGE_RESULT_SUCCESS;
}

int MDKImageReader::create(napi_env env, MDKImageReaderConfig config) {
    if (m_native) {
        if (m_config.width == config.width && m_config.height == config.height && 
            m_config.format == config.format && m_config.capicity == config.capicity) {
            _WARN("ImageReader is already created and config is same.");
            return IMAGE_RESULT_SUCCESS;
        }
        _INFO("recreate ImageReader.");
        release();
    }

    _INFO("create ImageReader(%dx%d, format=%d, capacity=%d).",
            config.width, config.height, config.format, config.capicity);

    napi_value app_obj = nullptr;
    int code = OH_Image_Receiver_CreateImageReceiver(env, config, &app_obj);
    _ERROR_RETURN_IF(code != IMAGE_RESULT_SUCCESS, code, "OH_Image_Receiver_CreateImageReceiver failed: %d", code);
    
    code = attach(env, app_obj);
    m_config = config;
    return code;
}

const char *MDKImageReader::getSurfaceId() {
    _FATAL_IF(!m_native, "MDKImageReader not created while call getSurfaceId()")
    
    int code = OH_Image_Receiver_GetReceivingSurfaceId(m_native, m_surface_id, sizeof(m_surface_id));
    _ERROR_RETURN_IF(code != IMAGE_RESULT_SUCCESS, nullptr, "GetReceivingSurfaceId failed. err code = %d", code);
    
    return m_surface_id;
}

MDKImage MDKImageReader::readLatestImage() {
    _FATAL_IF(!m_native, "MDKImageReader not created while call readLatestImage()")
    
    napi_value js_image = nullptr;
    int code = OH_Image_Receiver_ReadLatestImage(m_native, &js_image);
    _ERROR_RETURN_IF(code != IMAGE_RESULT_SUCCESS, MDKImage(nullptr), "ReadLatestImage failed. err code = %d", code)
    
    if (js_image == nullptr) {
        _WARN("ReadLatestImage failed. js image is null");
        return MDKImage(nullptr);
    }
    
    return MDKImage(m_jni_env, js_image);
}

MDKImage MDKImageReader::readNextImage() {
    _FATAL_IF(!m_native, "MDKImageReader not created while call readNextImage()")

    napi_value js_image = nullptr;
    int code = OH_Image_Receiver_ReadNextImage(m_native, &js_image);
    _ERROR_RETURN_IF(code != IMAGE_RESULT_SUCCESS, MDKImage(nullptr), "ReadNextImage failed. err code = %d", code)

    return MDKImage(m_jni_env, js_image);
}

int MDKImageReader::setImageListener(MDKImageListener listener) {
    _FATAL_IF(!m_native, "MDKImageReader not created while call setImageListener()")

    int code = OH_Image_Receiver_On(m_native, listener);
    _ERROR_IF(code != IMAGE_RESULT_SUCCESS, "OH_Image_Receiver_On failed. err code = %d", code);
    
    return code;
}

MDKImageSize MDKImageReader::getSize() {
    _FATAL_IF(!m_native, "MDKImageReader not created while call getSize()")

    MDKImageSize size;
    int code = OH_Image_Receiver_GetSize(m_native, &size);
    _ERROR_IF(code != IMAGE_RESULT_SUCCESS, "OH_Image_Receiver_GetSize failed. err code = %d", code)

    return size;
}

int MDKImageReader::getFormat() {
    _FATAL_IF(!m_native, "MDKImageReader not created while call getFormat()")

    int format;
    int code = OH_Image_Receiver_GetFormat(m_native, &format);
    _ERROR_IF(code != IMAGE_RESULT_SUCCESS, "OH_Image_Receiver_GetFormat failed. err code = %d", code)
    
    return code;
}

int MDKImageReader::getCapacity() {
    _FATAL_IF(!m_native, "MDKImageReader not created while call getCapacity()")

    int capacity = 0;
    int code = OH_Image_Receiver_GetCapacity(m_native, &capacity);
    _ERROR_IF(code != IMAGE_RESULT_SUCCESS, "OH_Image_Receiver_GetCapacity failed. err code = %d", code)
    
    return code;
}

int MDKImageReader::release() {
    if (m_native) {
        int code = OH_Image_Receiver_Release(m_native);
        m_native = nullptr;
        _ERROR_RETURN_IF(code != IMAGE_RESULT_SUCCESS, code, "ImageReader release failed. err code = %d", code)
    }
    
    return IMAGE_RESULT_SUCCESS;
}


NAMESPACE_END