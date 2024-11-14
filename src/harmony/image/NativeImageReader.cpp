//
// Created on 2024/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "NativeImageReader.h"
#include "common/utils/CallbackMgr.h"

NAMESPACE_DEFAULT

static CallbackMgr<NativeImageReader, NativeImageListener> g_callback_mgr;

static void onImageReceiverCallback(OH_ImageReceiverNative *receiver) {
    auto callbacks = g_callback_mgr.findCallback(receiver);
    if (callbacks == nullptr) {
        _WARN("NativeImageReader::onImageReceiverCallback no callback found! %p", receiver);
        return;
    }
    for (auto &cb : *callbacks) {
        cb.second->onImageAvailable(cb.first);
    }
}

class NativeImageReaderOptions {
public:
    NativeImageReaderOptions() {
        Image_ErrorCode err = OH_ImageReceiverOptions_Create(&m_options);
        _FATAL_IF(err, "OH_ImageReceiverOptions_Create failed: %d", err)
    }
    ~NativeImageReaderOptions() {
        if (m_options) {
            OH_ImageReceiverOptions_Release(m_options);
            m_options = nullptr;
        }
    }

    Image_ErrorCode setSize(uint32_t width, uint32_t height) {
        _FATAL_IF(!m_options, "OH_ImageReceiverOptions == null while setSize()")

        Image_Size size = {.width = width, .height = height};
        Image_ErrorCode err = OH_ImageReceiverOptions_SetSize(m_options, size);
        _ERROR_IF(err, "OH_ImageReceiverOptions_SetSize failed: %d, size(%d, %d)", err, width, height)
        return err;
    }

    Image_ErrorCode setCapacity(int32_t capacity) {
        _FATAL_IF(!m_options, "OH_ImageReceiverOptions == null while setCapacity()")

        Image_ErrorCode err = OH_ImageReceiverOptions_SetCapacity(m_options, capacity);
        _ERROR_IF(err, "OH_ImageReceiverOptions_SetCapacity failed: %d, capacity: %d", err, capacity)
        return err;
    }

    inline OH_ImageReceiverOptions *ptr() { return m_options; }

private:
    OH_ImageReceiverOptions *m_options = nullptr;
};

int NativeImageReader::create(uint32_t width, uint32_t height, int32_t capacity) {
    if (m_native) {
        if (m_width == width && m_height == height && m_capacity == capacity) {
            _WARN("NativeImageReader already created(width=%d, height=%d, capacity=%d)", width, height, capacity);
            return Image_ErrorCode::IMAGE_SUCCESS;
        }

        if (m_native) {
            OH_ImageReceiverNative_Release(m_native);
            m_native = nullptr;
        }
    }

    _INFO("create native image reader(width=%d, height=%d, capacity=%d)", width, height, capacity);

    NativeImageReaderOptions options;
    options.setSize(width, height);
    options.setCapacity(capacity);

    Image_ErrorCode error = OH_ImageReceiverNative_Create(options.ptr(), &m_native);
    _ERROR_RETURN_IF(error, error, "OH_ImageReceiverNative_Create failed: %d", error);

    Image_Size imageSize;
    error = OH_ImageReceiverNative_GetSize(m_native, &imageSize);
    _ERROR_RETURN_IF(error, error, "OH_ImageReceiverNative_GetSize failed: %d", error)
    _ERROR_RETURN_IF(imageSize.width != width || imageSize.height != height, Image_ErrorCode::IMAGE_UNKNOWN_ERROR,
                     "ImageReceiverNative create failed! size mismatch")

    int checkCap;
    error = OH_ImageReceiverNative_GetCapacity(m_native, &checkCap);
    _ERROR_RETURN_IF(error, error, "OH_ImageReceiverNative_GetCapacity failed: %d", error)
    _ERROR_RETURN_IF(checkCap != capacity, Image_ErrorCode::IMAGE_UNKNOWN_ERROR,
                     "ImageReceiverNative create failed! capacity mismatch")

    error = OH_ImageReceiverNative_GetReceivingSurfaceId(m_native, &m_surface_id);
    _ERROR_RETURN_IF(error, error, "OH_ImageReceiverNative_GetReceivingSurfaceId failed: %d", error)

    m_width = width;
    m_height = height;
    m_capacity = capacity;
    memset(m_surface_id_str, 0, sizeof(m_surface_id_str));
    std::sprintf(m_surface_id_str, "%lu", m_surface_id);

    _INFO("create image receiver success: (%d x %d, cap: %d) surface id: %lu", width, height, capacity, m_surface_id);
    return error;
}

NNativeWindow *NativeImageReader::surface() {
    _ERROR_RETURN_IF(!m_native, nullptr, "NativeImageReader not create")
    
    if (m_window == nullptr) {
        m_window = NNativeWindow::createFromSurfaceId(m_surface_id);
        //m_window->setSize(m_width, m_height);
    }
    return m_window;
}

uint64_t NativeImageReader::surfaceId() {
    //    _FATAL_IF(!m_native, "NativeImageReader not created while call surfaceId()")
    //
    //    uint64_t id = 0;
    //    Image_ErrorCode error = OH_ImageReceiverNative_GetReceivingSurfaceId(m_native, &id);
    //    _ERROR_IF(error, "OH_ImageReceiverNative_GetReceivingSurfaceId failed: %d", error)

    return m_surface_id;
}

const char *NativeImageReader::surfaceIdString() {
    //    uint64_t id = surfaceId();
    //    std::sprintf(m_surface_id_str, "%lu", id);
    return m_surface_id_str;
}

NIRImage NativeImageReader::readLastImage() {
    _FATAL_IF(!m_native, "NativeImageReader not create")

    OH_ImageNative *image = nullptr;
    Image_ErrorCode error = OH_ImageReceiverNative_ReadLatestImage(m_native, &image);
    _ERROR_RETURN_IF(error, NIRImage(), "OH_ImageReceiverNative_ReadLatestImage failed: %d", error)

    _ERROR_RETURN_IF(!image, NIRImage(), "OH_ImageReceiverNative_ReadLatestImage failed: image == nullptr")

    return NIRImage(image);
}

NIRImage NativeImageReader::readNextImage() {
    _FATAL_IF(!m_native, "NativeImageReader not create")

    OH_ImageNative *image = nullptr;
    Image_ErrorCode error = OH_ImageReceiverNative_ReadNextImage(m_native, &image);
    _ERROR_RETURN_IF(error, NIRImage(), "OH_ImageReceiverNative_ReadNextImage failed: %d", error)

    _ERROR_RETURN_IF(!image, NIRImage(), "OH_ImageReceiverNative_ReadNextImage failed: image == nullptr")

    return NIRImage(image);
}

int NativeImageReader::setImageListener(NativeImageListener *listener) {
    _FATAL_IF(!m_native, "NativeImageReader not create")

    Image_ErrorCode error;
    if (listener) {
        if (!g_callback_mgr.hasAnyCallback(m_native)) {
            error = OH_ImageReceiverNative_On(m_native, onImageReceiverCallback);
            _ERROR_RETURN_IF(error, error, "OH_ImageReceiverNative_On failed: %d", error)
        } else {
            g_callback_mgr.clearCallback(m_native);
        }
        g_callback_mgr.addCallback(m_native, *this, listener);
    } else {
        g_callback_mgr.clearCallback(m_native);
        error = OH_ImageReceiverNative_Off(m_native);
        _ERROR_IF(error, "OH_ImageReceiverNative_Off failed: %d", error)
    }
    return error;
}

void NativeImageReader::release() {
    if (m_native) {
        if (g_callback_mgr.hasAnyCallback(m_native)) {
            g_callback_mgr.clearCallback(m_native);
            OH_ImageReceiverNative_Off(m_native);
        }
        Image_ErrorCode code = OH_ImageReceiverNative_Release(m_native);
        if (code) {
            _WARN("OH_ImageReceiverNative_Release failed: %d", code);
        } else {
            _INFO("OH_ImageReceiverNative_Release release success");
        }
        m_native = nullptr;
    }
    m_width = 0;
    m_height = 0;
    m_capacity = 0;
    memset(m_surface_id_str, 0, sizeof(m_surface_id_str));
    m_surface_id = 0;
    DELETE_TO_NULL(m_window);
}

NAMESPACE_END