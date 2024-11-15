//
// Created on 2024/8/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ImageUtils.h"
#include "common/Object.h"
#include "NPixelmap.h"
#include <cstdint>
#include <multimedia/image_framework/image/image_source_native.h>

NAMESPACE_DEFAULT

class NImageSource;

class NImageInfo : Object {
public:
    explicit NImageInfo(OH_ImageSourceNative *source, int index = 0) {
        Image_ErrorCode code = OH_ImageSourceInfo_Create(&m_info);
        _ERROR_IF(code != IMAGE_SUCCESS, "create image source info failed: %s", ImageUtils::errorString(code));
        code = OH_ImageSourceNative_GetImageInfo(source, index, m_info);
        _ERROR_IF(code != IMAGE_SUCCESS, "get image source info failed: %s", ImageUtils::errorString(code));
    }
    explicit NImageInfo(OH_ImageSource_Info *info) : m_info(info) {}
    
    NImageInfo(const NImageInfo &other) : m_info(other.m_info), Object(other) {};

    ~NImageInfo() {
        if (m_info && no_reference()) {
            OH_ImageSourceInfo_Release(m_info);
            m_info = nullptr;
        }
    }

    uint32_t width() const {
        if (!m_info) {
            return 0;
        }
        uint32_t w = 0;
        Image_ErrorCode code = OH_ImageSourceInfo_GetWidth(m_info, &w);
        _ERROR_IF(code != IMAGE_SUCCESS, "get image source width failed");
        return w;
    }

    uint32_t height() const {
        if (!m_info) {
            return 0;
        }
        uint32_t h = 0;
        Image_ErrorCode code = OH_ImageSourceInfo_GetHeight(m_info, &h);
        _ERROR_IF(code != IMAGE_SUCCESS, "get image source height failed");
        return h;
    }

    bool isHDR() const {
        if (!m_info) {
            return false;
        }
        bool isHDR = false;
        Image_ErrorCode code = OH_ImageSourceInfo_GetDynamicRange(m_info, &isHDR);
        _ERROR_IF(code != IMAGE_SUCCESS, "get image source HDR status failed");
        return isHDR;
    }
    
    std::string toString() const {
        if (!m_info) {
            return "NImageInfo invalid";
        }
        std::stringstream ss;
        ss << "{size(" << width() << "x" << height() << "), isHDR: " << (isHDR() ? "true" : "false") << "}";
        return ss.str();
    }

private:
    OH_ImageSource_Info *m_info = nullptr;
};


class NDecodingOptions : Object {
    friend class NImageSource;
public:
    NDecodingOptions() {
        Image_ErrorCode code = OH_DecodingOptions_Create(&m_options);
        _ERROR_IF(code != IMAGE_SUCCESS, "create decoding options failed: %s", ImageUtils::errorString(code));
    }
    
    NDecodingOptions(const NDecodingOptions &other) : m_options(other.m_options), Object(other) {};
    
    ~NDecodingOptions() {
        if (m_options && no_reference()) {
            OH_DecodingOptions_Release(m_options);
            m_options = nullptr;
        }
    }
    
    inline bool valid() const { return m_options != nullptr; }
    
    int32_t getPixelFormat() {
        _ERROR_RETURN_IF(!m_options, 0, "DecodingOptions invalid!");
        
        int32_t pixelFormat = 0;
        Image_ErrorCode code = OH_DecodingOptions_GetPixelFormat(m_options, &pixelFormat);
        _ERROR_IF(code != IMAGE_SUCCESS, "get pixel format failed: %s", ImageUtils::errorString(code));
        return pixelFormat;
    }
    
    void setPixelFormat(int32_t pixelFormat) {
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_ErrorCode code = OH_DecodingOptions_SetPixelFormat(m_options, pixelFormat);
        _ERROR_IF(code != IMAGE_SUCCESS, "set pixel format failed: %s", ImageUtils::errorString(code));
    }
    
    uint32_t getIndex() {
        _ERROR_RETURN_IF(!m_options, 0, "DecodingOptions invalid!");
        
        uint32_t index = 0;
        Image_ErrorCode code = OH_DecodingOptions_GetIndex(m_options, &index);
        _ERROR_IF(code != IMAGE_SUCCESS, "get index failed: %s", ImageUtils::errorString(code));
        return index;
    }
    
    void setIndex(uint32_t index) {
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_ErrorCode code = OH_DecodingOptions_SetIndex(m_options, index);
        _ERROR_IF(code != IMAGE_SUCCESS, "set index failed: %s", ImageUtils::errorString(code));
    }
    
    float getRotate() {
        _ERROR_RETURN_IF(!m_options, 0, "DecodingOptions invalid!");
        
        float rotate = 0;
        Image_ErrorCode code = OH_DecodingOptions_GetRotate(m_options, &rotate);
        _ERROR_IF(code != IMAGE_SUCCESS, "get rotate failed: %s", ImageUtils::errorString(code));
        return rotate;
    }
    
    void setRotate(float rotate) {
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_ErrorCode code = OH_DecodingOptions_SetRotate(m_options, rotate);
        _ERROR_IF(code != IMAGE_SUCCESS, "set rotate failed: %s", ImageUtils::errorString(code));
    }
    
    void getDesiredSize(uint32_t& width, uint32_t &height) {
        width = 0;
        height = 0;
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_Size size;
        Image_ErrorCode code = OH_DecodingOptions_GetDesiredSize(m_options, &size);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, void(), "get desired size failed: %s", ImageUtils::errorString(code));
        
        width = size.width;
        height = size.height;
    }
    
    void setDesiredSize(uint32_t width, uint32_t height) {
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_Size size = { width, height };
        Image_ErrorCode code = OH_DecodingOptions_SetDesiredSize(m_options, &size);
        _ERROR_IF(code != IMAGE_SUCCESS, "set desired size failed: %s", ImageUtils::errorString(code));
    }
    
    void getDesiredRegion(uint32_t &x, uint32_t &y, uint32_t &width, uint32_t &height) {
        x = y = width = height = 0;
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_Region region;
        Image_ErrorCode code = OH_DecodingOptions_GetDesiredRegion(m_options, &region);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, void(), "get desired region failed: %s", ImageUtils::errorString(code));
        
        x = region.x;
        y = region.y;
        width = region.width;
        height = region.height;
    }
    
    void setDesiredRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_Region region = { x, y, width, height };
        Image_ErrorCode code = OH_DecodingOptions_SetDesiredRegion(m_options, &region);
        _ERROR_IF(code != IMAGE_SUCCESS, "set desired region failed: %s", ImageUtils::errorString(code));
    }
    
    int32_t getDesiredDynamicRange() {
        _ERROR_RETURN_IF(!m_options, 0, "ImageSource invalid!");
        
        int32_t dynamicRange = 0;
        Image_ErrorCode code = OH_DecodingOptions_GetDesiredDynamicRange(m_options, &dynamicRange);
        _ERROR_IF(code != IMAGE_SUCCESS, "get desired dynamic range failed: %s", ImageUtils::errorString(code));
        return dynamicRange;
    }
    
    // 设置为AUTO会根据图片资源格式解码，如果图片资源为HDR资源则会解码为HDR的pixelmap。HDR 的格式是 RGBA_1010102
    void setDesiredDynamicRange(int32_t dynamicRange) {
        _ERROR_RETURN_IF(!m_options, void(), "DecodingOptions invalid!");
        
        Image_ErrorCode code = OH_DecodingOptions_SetDesiredDynamicRange(m_options, dynamicRange);
        _ERROR_IF(code != IMAGE_SUCCESS, "set desired dynamic range failed: %s", ImageUtils::errorString(code));
    }
    
private:
    OH_DecodingOptions *m_options = nullptr;
};

class NImagePacker;
class NImageSource : Object {
    friend class NImagePacker;
public:
    explicit NImageSource(char *uri) {
        Image_ErrorCode code = OH_ImageSourceNative_CreateFromUri(uri, strlen(uri), &m_source);
        _ERROR_IF(code != IMAGE_SUCCESS, "create image source(%s) failed: %s", uri, ImageUtils::errorString(code));
    }
    
    explicit NImageSource(const std::string &filepath) {
        m_file = std::fopen(filepath.c_str(), "r");
        if (m_file == nullptr) {
            _ERROR("open file(%s) failed", filepath.c_str());
            return;
        }
        Image_ErrorCode code = OH_ImageSourceNative_CreateFromFd(fileno(m_file), &m_source);
        _ERROR_IF(code != IMAGE_SUCCESS, "create image source from file failed: %s", ImageUtils::errorString(code));
    }

    explicit NImageSource(int32_t fd) {
        Image_ErrorCode code = OH_ImageSourceNative_CreateFromFd(fd, &m_source);
        _ERROR_IF(code != IMAGE_SUCCESS, "create image source from file descriptor failed: %s", ImageUtils::errorString(code));
    }
    
    NImageSource(uint8_t *data, size_t dataSize) {
        Image_ErrorCode code = OH_ImageSourceNative_CreateFromData(data, dataSize, &m_source);
        _ERROR_IF(code != IMAGE_SUCCESS, "create image source from memory failed: %s", ImageUtils::errorString(code));
    }

    explicit NImageSource(RawFileDescriptor *rawFile) {
        Image_ErrorCode code = OH_ImageSourceNative_CreateFromRawFile(rawFile, &m_source);
        _ERROR_IF(code != IMAGE_SUCCESS, "create image source from raw file descriptor failed: %s", ImageUtils::errorString(code));
    }

    explicit NImageSource(OH_ImageSourceNative *native) : m_source(native) {}
    
    NImageSource(const NImageSource &other) : m_source(other.m_source), Object(other) {}
    
    ~NImageSource() {
        if (m_source && no_reference()) {
            OH_ImageSourceNative_Release(m_source);
            m_source = nullptr;
            if (m_file) {
                fclose(m_file);
                m_file = nullptr;
            }
        }
    }
    
    inline bool valid() {
        return m_source!= nullptr;
    }
    
    NImageInfo info(int index = 0) const {
        return NImageInfo(m_source, index);
    }
    
    std::string getProperty(const std::string &name) {
        _ERROR_RETURN_IF(!m_source, "", "Image source invalid!");
        
        Image_String key;
        key.data =(char *) name.c_str();
        key.size = name.size();
        
        Image_String value;
        Image_ErrorCode code = OH_ImageSourceNative_GetImageProperty(m_source, &key, &value);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, "", "get image source property failed: %s", ImageUtils::errorString(code));
        
        return {value.data, value.size};
    }
    
    bool setProperty(const std::string &name, const std::string &value) {
        _ERROR_RETURN_IF(!m_source, false, "Image source invalid!");
        
        Image_String key;
        key.data =(char *) name.c_str();
        key.size = name.size();
        
        Image_String val;
        val.data =(char *) value.c_str();
        val.size = value.size();
        
        Image_ErrorCode code = OH_ImageSourceNative_ModifyImageProperty(m_source, &key, &val);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, false, "set image source property failed: %s", ImageUtils::errorString(code));
        return true;
    }
    
    uint32_t frameCount() const {
        _ERROR_RETURN_IF(!m_source, 0, "Image source invalid!");
        
        uint32_t count = 0;
        Image_ErrorCode code = OH_ImageSourceNative_GetFrameCount(m_source, &count);
        _ERROR_IF(code != IMAGE_SUCCESS, "get frame count failed: %s", ImageUtils::errorString(code));
        return count;
    }
    
    NPixelmap* decodeFrame(int32_t index = 0) const {
        NDecodingOptions options;
        options.setIndex(index);
        return decodeFrame(options);
    }
    
    NPixelmap* decodeFrame(const NDecodingOptions& options) const {
        _ERROR_RETURN_IF(!m_source, nullptr, "Image source invalid!");
        OH_PixelmapNative *pixels = nullptr;
        Image_ErrorCode code = OH_ImageSourceNative_CreatePixelmap(m_source, options.m_options, &pixels);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, nullptr, "get frame failed: %s", ImageUtils::errorString(code));
        return new NPixelmap(pixels);
    }
    
    std::vector<NPixelmap> decodeFrameList(const NDecodingOptions& options) const {
        std::vector<NPixelmap> result;
        _ERROR_RETURN_IF(!m_source, result, "Image source invalid!");
        
        auto fcount = frameCount();
        auto **resVecPixMap = new OH_PixelmapNative*[fcount];
        size_t outSize = fcount;
        Image_ErrorCode code = OH_ImageSourceNative_CreatePixelmapList(m_source, options.m_options, resVecPixMap, outSize);
        _ERROR_RETURN_IF(code != IMAGE_SUCCESS, result, "get frame list failed: %s", ImageUtils::errorString(code));
        
        for (size_t i = 0; i < outSize; i++) {
            result.emplace_back(resVecPixMap[i]);
        }
        return result;
    }

    std::vector<int32_t> getDelayTimeList() {
        std::vector<int32_t> result;
        _ERROR_RETURN_IF(!m_source, result, "Image source invalid!");
        size_t size = frameCount();
        auto *list = new int32_t[size];
        Image_ErrorCode code = OH_ImageSourceNative_GetDelayTimeList(m_source, list, size);
        if (code == IMAGE_SUCCESS) {
            for (size_t i = 0; i < size; i++) {
                result.push_back(list[i]);
            }
        } else {
            _ERROR("get delay time list failed: %s", ImageUtils::errorString(code));
        }
        delete[] list;
        return result;
    }

    std::string toString() const {
        _ERROR_RETURN_IF(!m_source, "", "Image source invalid!");
        std::stringstream ss;
        auto fcount = frameCount();
        ss << "{frameCount: " << fcount << "\n";
        for (int i = 0; i < fcount; i++) {
            NImageInfo imgInfo = info(i);
            ss << "Frame[" << i << "]: " << imgInfo.toString() << "\n";
        }
        ss << "}";
        return ss.str();
    }

private:
    std::FILE *m_file = nullptr;
    OH_ImageSourceNative *m_source = nullptr;
};

NAMESPACE_END
