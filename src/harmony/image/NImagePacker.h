//
// Created on 2024/8/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Object.h"
#include "NImageSource.h"
#include "ImageUtils.h"
#include <multimedia/image_framework/image/image_packer_native.h>

NAMESPACE_DEFAULT

class NImagePacker;

/// compression options
class NPackingOptions : Object {
    friend class NImagePacker;

public:
    static NPackingOptions jpeg(uint32_t quality, bool packProperties = true) {
        NPackingOptions options;
        options.setMimeType("image/jpeg");
        options.setQuality(quality);
        options.setPackProperties(packProperties);
        options.setDesiredDynamicRange(IMAGE_PACKER_DYNAMIC_RANGE_AUTO);
        return options;
    }

public:
    NPackingOptions() {
        Image_ErrorCode code = OH_PackingOptions_Create(&m_options);
        _ERROR_IF(code != IMAGE_SUCCESS, "PackingOptions create failed: %s", ImageUtils::errorString(code));
    }

    NPackingOptions(const NPackingOptions &options) : m_options(options.m_options), Object(options) {}

    ~NPackingOptions() {
        if (m_options && no_reference()) {
            OH_PackingOptions_Release(m_options);
            m_options = nullptr;
        }
    }

    inline bool valid() { return m_options != nullptr; }

    std::string mimeType() const {
        _ERROR_RETURN_IF(!m_options, "", "PackingOptions invalid!");
        Image_MimeType type;
        Image_ErrorCode code = OH_PackingOptions_GetMimeType(m_options, &type);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, "", "Get MIME type failed: %s", ImageUtils::errorString(code));
        return std::string(type.data, type.size);
    }

    void setMimeType(const char *type) {
        _ERROR_RETURN_IF(!m_options, void(), "PackingOptions invalid!");
        Image_MimeType mimeType = {(char *)type, strlen(type)};
        Image_ErrorCode code = OH_PackingOptions_SetMimeType(m_options, &mimeType);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set MIME type failed: %s", ImageUtils::errorString(code));
    }

    uint32_t quality() const {
        _ERROR_RETURN_IF(!m_options, 0, "PackingOptions invalid!");
        uint32_t q = 0;
        Image_ErrorCode code = OH_PackingOptions_GetQuality(m_options, &q);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, 0, "Get quality failed: %s", ImageUtils::errorString(code));
        return q;
    }

    void setQuality(uint32_t quality) {
        _ERROR_RETURN_IF(!m_options, void(), "PackingOptions invalid!");
        Image_ErrorCode code = OH_PackingOptions_SetQuality(m_options, quality);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set quality failed: %s", ImageUtils::errorString(code));
    }

    bool packProperties() {
        _ERROR_RETURN_IF(!m_options, false, "PackingOptions invalid!");
        bool props = false;
        Image_ErrorCode code = OH_PackingOptions_GetNeedsPackProperties(m_options, &props);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, false, "Pack properties failed: %s", ImageUtils::errorString(code));
        return props;
    }

    void setPackProperties(bool props) {
        _ERROR_RETURN_IF(!m_options, void(), "PackingOptions invalid!");
        Image_ErrorCode code = OH_PackingOptions_SetNeedsPackProperties(m_options, props);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set pack properties failed: %s", ImageUtils::errorString(code));
    }

    int desiredDynamicRange() const {
        _ERROR_RETURN_IF(!m_options, 0, "PackingOptions invalid!");
        int range = 0;
        Image_ErrorCode code = OH_PackingOptions_GetDesiredDynamicRange(m_options, &range);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, 0, "Get desired dynamic range failed: %s",
                         ImageUtils::errorString(code));
        return range;
    }

    void setDesiredDynamicRange(int range) {
        _ERROR_RETURN_IF(!m_options, void(), "PackingOptions invalid!");
        Image_ErrorCode code = OH_PackingOptions_SetDesiredDynamicRange(m_options, range);
        _ERROR_IF(code != IMAGE_SUCCESS, "Set desired dynamic range failed: %s", ImageUtils::errorString(code));
    }

private:
    OH_PackingOptions *m_options = nullptr;
};

class NImagePacker : Object {
public:
    static int packToJpeg(const std::string &filepath, int quality, uint8_t *data, int width, int height,
                          int pixelFormat, int size = -1) {
        NPackingOptions packingOptions = NPackingOptions::jpeg(quality);
        return packToFile(filepath, packingOptions, data, width, height, pixelFormat, size);
    }

    static int packToFile(const std::string &filepath, NPackingOptions &packingOptions, uint8_t *data, int width,
                          int height, int pixelFormat, int size = -1) {
        NPixelmapInitOptions options;
        options.setPixelFormat(pixelFormat);
        options.setWidth(width);
        options.setHeight(height);
        if (size < 1) {
            size = ImageUtils::formatBytes(width, height, pixelFormat);
        }
        NPixelmap pixelmap(options);
        pixelmap.writePixels(data, size);
//        NPixelmapInfo info = pixelmap.info();
//        _INFO("pixelmap width: %d, height: %d, format: %d", info.width(), info.height(), info.pixelFormat());

        FILE *file = fopen(filepath.c_str(), "w+");
        if (file == nullptr) {
            _ERROR("Open file %s failed: %s", filepath.c_str(), strerror(errno));
            return -1;
        }
        int fd = fileno(file);
        NImagePacker packer;
        Image_ErrorCode error = packer.packToFile(packingOptions, pixelmap, fd);
        fclose(file);
        _ERROR_RETURN_IF(error, error, "packToFile(%s) failed: %s", filepath.c_str(), ImageUtils::errorString(error));
        return error;
    }

public:
    NImagePacker() {
        Image_ErrorCode code = OH_ImagePackerNative_Create(&m_native);
        _ERROR_IF(code != IMAGE_SUCCESS, "ImagePacker create failed: %s", ImageUtils::errorString(code));
    }

    NImagePacker(const NImagePacker &other) : m_native(other.m_native), Object(other) {}

    ~NImagePacker() {
        if (m_native && no_reference()) {
            OH_ImagePackerNative_Release(m_native);
            m_native = nullptr;
        }
    }

    inline bool valid() { return m_native != nullptr; }

    Image_ErrorCode packToData(NPackingOptions &options, NImageSource &source, uint8_t *outData, size_t *outSize) {
        _ERROR_RETURN_IF(!m_native || !options.valid() || !source.valid(), IMAGE_UNSUPPORTED_OPERATION,
                         "Invalid ImagePacker or PackingOptions or ImageSource!");
        Image_ErrorCode code = OH_ImagePackerNative_PackToDataFromImageSource(m_native, options.m_options,
                                                                              source.m_source, outData, outSize);
        _ERROR_IF(code != IMAGE_SUCCESS, "ImagePackerNative_PackToDataFromImageSource failed: %s",
                  ImageUtils::errorString(code));
        return code;
    }

    Image_ErrorCode packToFile(NPackingOptions &options, NImageSource &source, int32_t fd) {
        _ERROR_RETURN_IF(!m_native || !options.valid() || !source.valid(), IMAGE_UNSUPPORTED_OPERATION,
                         "Invalid ImagePacker or PackingOptions or ImageSource!");
        Image_ErrorCode code =
            OH_ImagePackerNative_PackToFileFromImageSource(m_native, options.m_options, source.m_source, fd);
        _ERROR_IF(code != IMAGE_SUCCESS, "ImagePackerNative_PackToFileFromImageSource failed: %s",
                  ImageUtils::errorString(code));
        return code;
    }

    Image_ErrorCode packToData(NPackingOptions &options, NPixelmap &pixelmap, uint8_t *outData, size_t *outSize) {
        _ERROR_RETURN_IF(!m_native || !options.valid() || !pixelmap.valid(), IMAGE_UNSUPPORTED_OPERATION,
                         "Invalid ImagePacker or PackingOptions or Pixelmap!");
        Image_ErrorCode code = OH_ImagePackerNative_PackToDataFromPixelmap(m_native, options.m_options,
                                                                           pixelmap.m_native, outData, outSize);
        _ERROR_IF(code != IMAGE_SUCCESS, "ImagePackerNative_PackToDataFromPixelmap failed: %s",
                  ImageUtils::errorString(code));
        return code;
    }

    Image_ErrorCode packToFile(NPackingOptions &options, NPixelmap &pixelmap, int32_t fd) {
        _ERROR_RETURN_IF(!m_native || !options.valid() || !pixelmap.valid(), IMAGE_UNSUPPORTED_OPERATION,
                         "Invalid ImagePacker or PackingOptions or Pixelmap!");
        Image_ErrorCode code =
            OH_ImagePackerNative_PackToFileFromPixelmap(m_native, options.m_options, pixelmap.m_native, fd);
        _ERROR_IF(code != IMAGE_SUCCESS, "OH_ImagePackerNative_PackToFileFromPixelmap failed: %s",
                  ImageUtils::errorString(code));
        return code;
    }

private:
    OH_ImagePackerNative *m_native = nullptr;
};

NAMESPACE_END
