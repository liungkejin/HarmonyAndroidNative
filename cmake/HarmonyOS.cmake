
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
        ${CMAKE_SOURCE_DIR}/src/harmony
)

set(PLATFORM_SOURCES
        ${CMAKE_SOURCE_DIR}/src/harmony/Harmony.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/AppContext.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/utils/NapiUtils.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/xcomp/XComponent.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/xcomp/XCompGLBinder.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/CamDevice.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/CamInput.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/CamManager.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/CamUtils.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/CaptureSession.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/MetadataOutput.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/PhotoOutput.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/PreviewOutput.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/cam/VideoOutput.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/image/MDKImageReader.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/image/NativeImageReader.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/masset/MediaAssetMgr.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/media/audio/read/AudioReader.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/media/audio/AudioEncoder.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/media/video/VideoEncoder.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/sensor/SensorGravity.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/sensor/SensorMgr.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/mslite/MSLitePredictor.cpp
        ${CMAKE_SOURCE_DIR}/src/harmony/hiai/HiAI.cpp
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

        libEGL.so
        libGLESv2.so
        libGLESv3.so
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