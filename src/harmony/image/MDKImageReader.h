//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "Namespace.h"
#include <multimedia/image_framework/image_receiver_mdk.h>
#include "MDKImage.h"

NAMESPACE_DEFAULT

#define ERROR_CODE_NULLPTR (-12345)

class MDKImageReader;

typedef struct OhosImageReceiverInfo MDKImageReaderConfig;
typedef OH_Image_Receiver_On_Callback MDKImageListener;

// 封装 multimedia/image_framework/image_receiver_mdk.h 接口
class MDKImageReader {
public:
    MDKImageReader() = default;
    ~MDKImageReader() {}

public:
    /**
     * 创建 ImageReader
     * @param config 配置 width, height, format, capacity
     * @return 0 表示成功，其他表示失败
     */
    int create(napi_env env, MDKImageReaderConfig config);

    /**
     * 从上传传
     * @param js_ir_obj
     * @return 0 表示成功，其他表示失败
     */
    int attach(napi_env env, napi_value js_ir_obj);
    
    /**
     * 判断当前 ImageReader 是否有效
     */
    inline bool valid() const { return m_native != nullptr;}

    /**
     * 获取当前 ImageReader 的 Surface id
     * @return 如果成功，返回 Surface id，失败返回 nullptr
     */
    const char *getSurfaceId();

    /**
     * 读取最新一张图片数据,
     * 即如果有队列里面有多张图片数据，只返回最新的一张图片数据，其他的会被丢弃
     * 注意主动释放掉返回的 Image 对象
     * @return 失败返回 nullptr，成功返回 Image 对象
     */
    MDKImage readLatestImage();

    /**
     * 按顺序读取下一张图片数据
     * 注意主动释放掉返回的 Image 对象
     * @return 失败返回 nullptr，成功返回 Image 对象
     */
    MDKImage readNextImage();

    /**
     * 设置 Image 可用时的回调函数
     * @param listener ImageListener
     * @return 0 表示成功，其他表示失败
     */
    int setImageListener(MDKImageListener listener);

    /**
     * 获取图片大小
     * @return ImageSize 输出的大小
     */
    MDKImageSize getSize();

    /**
     * 获取读取的图片格式
     * @return format 图片格式
     */
    int getFormat();

    /**
     * 获取 ImageReader 的缓冲队列容量
     * @return capacity 缓冲队列容量
     */
    int getCapacity();

    /**
     * 释放 ImageReceiverNative
     * @return 0 表示成功，其他表示失败
     */
    int release();

private:
    napi_env m_jni_env = nullptr;
    MDKImageReaderConfig m_config = {};
    
    // native层对象
    ImageReceiverNative *m_native = nullptr;
    char m_surface_id[128] = {0};
};

NAMESPACE_END
