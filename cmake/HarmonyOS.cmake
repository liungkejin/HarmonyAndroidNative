
# 增加宏定义 __HARMONYOS__
add_definitions(-D__HARMONYOS__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")

set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.9.0/${OHOS_ARCH}/lib/cmake/opencv4)

# 拿到 CMAKE_TOOLCHAIN_FILE 的文件夹路径
get_filename_component(CMAKE_TOOLCHAIN_DIR ${CMAKE_TOOLCHAIN_FILE} DIRECTORY)
set(HMS_SDK_PATH ${CMAKE_TOOLCHAIN_DIR}/../../../../hms)

list(APPEND CMAKE_FIND_ROOT_PATH ${HMS_SDK_PATH}/native/sysroot/usr)

find_library(hiai_foundation-lib hiai_foundation)
MESSAGE(STATUS "hiai_foundation-lib: ${hiai_foundation-lib} in ${HMS_SDK_PATH}/native/sysroot/usr/lib")

set(PLATFORM_INCLUDES
        ${HMS_SDK_PATH}/native/sysroot/usr/include
        ${PLATFORM_SRC_PATH}
)

set(PLATFORM_SOURCES
        ${PLATFORM_SRC_PATH}/Harmony.cpp
        ${PLATFORM_SRC_PATH}/AppContext.cpp
        ${PLATFORM_SRC_PATH}/utils/NapiUtils.cpp
        ${PLATFORM_SRC_PATH}/cam/CamDevice.cpp
        ${PLATFORM_SRC_PATH}/cam/CamInput.cpp
        ${PLATFORM_SRC_PATH}/cam/CamManager.cpp
        ${PLATFORM_SRC_PATH}/cam/CamUtils.cpp
        ${PLATFORM_SRC_PATH}/cam/CaptureSession.cpp
        ${PLATFORM_SRC_PATH}/cam/MetadataOutput.cpp
        ${PLATFORM_SRC_PATH}/cam/PhotoOutput.cpp
        ${PLATFORM_SRC_PATH}/cam/PreviewOutput.cpp
        ${PLATFORM_SRC_PATH}/cam/VideoOutput.cpp
        ${PLATFORM_SRC_PATH}/image/MDKImageReader.cpp
        ${PLATFORM_SRC_PATH}/image/NativeImageReader.cpp
        ${PLATFORM_SRC_PATH}/masset/MediaAssetMgr.cpp
        ${PLATFORM_SRC_PATH}/media/audio/read/AudioReader.cpp
        ${PLATFORM_SRC_PATH}/media/audio/AudioEncoder.cpp
        ${PLATFORM_SRC_PATH}/media/video/VideoEncoder.cpp
        ${PLATFORM_SRC_PATH}/sensor/SensorGravity.cpp
        ${PLATFORM_SRC_PATH}/sensor/SensorAccelerometer.cpp
        ${PLATFORM_SRC_PATH}/sensor/SensorMgr.cpp
        ${PLATFORM_SRC_PATH}/mslite/MSLitePredictor.cpp
        ${PLATFORM_SRC_PATH}/hiai/HiAI.cpp
)

set(PLATFORM_LIBS
        libhilog_ndk.z.so
        libace_napi.z.so
        # mdk image receiver
        libimage_ndk.z.so
        libimage_receiver_ndk.z.so
        libohcamera.so
        libnative_buffer.so
        # native image receciver
        libimage_receiver.so
        libohimage.so
        libnative_image.so
        libimage_source.so
        libpixelmap.so
        libimage_packer.so
        libmedia_asset_manager.so

        libnative_window.so
        libnative_buffer.so

        # media
        libnative_media_codecbase.so
        libnative_media_core.so
        libnative_media_venc.so
        libnative_media_acodec.so
        libnative_media_avmuxer.so
        libnative_media_avdemuxer.so
        libnative_media_avsource.so
        libohaudio.so

        libace_napi.z.so
        libdeviceinfo_ndk.z.so

        librawfile.z.so
        libohresmgr.so
        libbundle_ndk.z.so
        libohsensor.so
        libpixelmap_ndk.z.so

        mindspore_lite_ndk
        ${hiai_foundation-lib}
        libneural_network_core.so

        ace_ndk.z
        uv
)

if (${ZNATIVE_GL_ENABLE})
    set(PLATFORM_SOURCES
            ${PLATFORM_SOURCES}
            ${PLATFORM_SRC_PATH}/xcomp/XComponent.cpp
            ${PLATFORM_SRC_PATH}/xcomp/XCompGLBinder.cpp
    )
    set(PLATFORM_LIBS
            ${PLATFORM_LIBS}
            libEGL.so
            libGLESv2.so
            libGLESv3.so
    )
endif ()