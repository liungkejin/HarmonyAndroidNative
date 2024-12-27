cmake_minimum_required(VERSION 3.18.1)

set(PROJ_NAME "znative")
project(${PROJ_NAME} VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_SHARED_LIB "Build shared library" ON)
option(OPENCV_ENABLE "Use OpenCV" OFF)
option(LIBYUV_ENABLE "Use libyuv" ON)

# debug 时增加宏定义 __DEBUG__
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-D__DEBUG__)
endif()

add_definitions(-DSTRICT_MODE=false)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(PLATFORM "android")
    set(TARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(PLATFORM "harmony")
    set(TARGET_ARCH ${OHOS_ARCH})
    # 增加宏定义 __HARMONY_OS__
    add_definitions(-D__HARMONY_OS__)
else ()
    set(PLATFORM "local")
    add_definitions(-D__LOCAL_OS__)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE TARGET_ARCH)
    endif ()
endif ()

message(STATUS "Build status: platform: ${PLATFORM}, sys name: ${CMAKE_SYSTEM_NAME}, arch: ${TARGET_ARCH}, build type: ${CMAKE_BUILD_TYPE}, sys version: ${CMAKE_SYSTEM_VERSION}")

# 公共第三方库路径
set(COM_LIBS_PATH ${CMAKE_SOURCE_DIR}/libs/common)
# 平台相关第三方库路径
set(PLATFORM_LIBS_PATH ${CMAKE_SOURCE_DIR}/libs/${PLATFORM})

# message(STATUS "Platform libs path: ${PLATFORM_LIBS_PATH}")

# tinyformat
include_directories(${COM_LIBS_PATH}/tinyformat)

# eventpp
add_subdirectory(${COM_LIBS_PATH}/eventpp-0.1.3)

# json
include_directories(${COM_LIBS_PATH}/json)

# glm
# add_subdirectory(${COM_LIBS_PATH}/glm)

set(THIRD_PARTY_LIBS eventpp)

# OpenCV
if (${OPENCV_ENABLE})
    message(STATUS "Use OpenCV")

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.3.0/sdk/native/jni)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.9.0/${OHOS_ARCH}/lib/cmake/opencv4)
    endif ()
    find_package(OpenCV REQUIRED)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} ${OpenCV_LIBS})
endif ()

# libyuv
if (${LIBYUV_ENABLE})
    message(STATUS "Use libyuv")

    set(libyuv_path ${COM_LIBS_PATH}/libyuv-stable)
    include_directories(${libyuv_path}/include)
    FILE(GLOB_RECURSE LIBYUV_SRCS ${libyuv_path}/source/*.cc)
endif ()

include_directories(src)
set(COMMON_SOURCES
        ${LIBYUV_SRCS}
        src/ZNative.cpp
        src/common/Log.cpp
        src/common/utils/YuvUtils.cpp
        src/common/gles/GLUtil.cpp
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(PLATFORM_SOURCES
            # 对于 local 平台不需要
            src/common/gles/EGLCtx.cpp
            src/common/gles/GLEngine.cpp
            src/android/AppContext.cpp
            src/android/utils/JNIUtils.cpp
            src/android/surface/NativeWindow.cpp
            src/android/cam/CamManager.cpp
            src/android/cam/CamUtils.cpp
            src/android/cam/CamCharacteristics.cpp
            src/android/cam/CamDevice.cpp
            src/android/cam/CaptureSession.cpp
            src/android/cam/CaptureRequest.cpp
            src/android/image/ImageReader.cpp
    )
    set(PLATFORM_LIBS
            android
            log
            z
            jnigraphics
            GLESv2
            GLESv3
            EGL
            camera2ndk
            mediandk
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(PLATFORM_SOURCES
            # 对于 local 平台不需要
            src/common/gles/EGLCtx.cpp
            src/common/gles/GLEngine.cpp
            src/harmony/Harmony.cpp
            src/harmony/AppContext.cpp
            src/harmony/utils/NapiUtils.cpp
            src/harmony/xcomp/XComponent.cpp
            src/harmony/xcomp/XCompGLBinder.cpp
            src/harmony/cam/CamDevice.cpp
            src/harmony/cam/CamInput.cpp
            src/harmony/cam/CamManager.cpp
            src/harmony/cam/CamUtils.cpp
            src/harmony/cam/CaptureSession.cpp
            src/harmony/cam/MetadataOutput.cpp
            src/harmony/cam/PhotoOutput.cpp
            src/harmony/cam/PreviewOutput.cpp
            src/harmony/cam/VideoOutput.cpp
            src/harmony/image/MDKImageReader.cpp
            src/harmony/image/NativeImageReader.cpp
            src/harmony/masset/MediaAssetMgr.cpp
            src/harmony/media/audio/read/AudioReader.cpp
            src/harmony/media/audio/AudioEncoder.cpp
            src/harmony/media/video/VideoEncoder.cpp
            src/harmony/sensor/SensorGravity.cpp
            src/harmony/sensor/SensorMgr.cpp
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

            ace_ndk.z
            uv
    )
else ()
    find_package(OpenGL REQUIRED)
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} OpenGL::GL)
endif ()

# install
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/${PLATFORM}/install)
install(FILES src/Namespace.h src/ZNative.h DESTINATION ${INSTALL_DIR}/include)
install(DIRECTORY src/common DESTINATION ${INSTALL_DIR}/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/${PLATFORM} DESTINATION ${INSTALL_DIR}/include/ FILES_MATCHING PATTERN "*.h")

if (${BUILD_SHARED_LIB})
    add_library(${PROJ_NAME} SHARED
            ${COMMON_SOURCES}
            ${PLATFORM_SOURCES}
    )
else ()
    add_library(${PROJ_NAME} STATIC
            ${COMMON_SOURCES}
            ${PLATFORM_SOURCES}
    )
endif ()

install(TARGETS ${PROJ_NAME} DESTINATION ${INSTALL_DIR}/${CMAKE_BUILD_TYPE}/libs/${TARGET_ARCH})

target_link_libraries(${PROJ_NAME} PUBLIC
        ${PLATFORM_LIBS}
        ${THIRD_PARTY_LIBS}
)

if (${PLATFORM} STREQUAL "local")
    include_directories(
            ${PLATFORM_LIBS_PATH}/glfw-3.4/include
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/backends
    )

    set(GLFW_INSTALL OFF)
    set(GLFW_BUILD_DOCS OFF)
    add_subdirectory(${PLATFORM_LIBS_PATH}/glfw-3.4)

    set(IMGUI_SRC
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/imgui.cpp
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/imgui_demo.cpp
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/imgui_draw.cpp
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/imgui_tables.cpp
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/imgui_widgets.cpp
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/backends/imgui_impl_glfw.cpp
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/backends/imgui_impl_opengl3.cpp
    )

    add_executable(local-sample
            ${IMGUI_SRC}
            samples/local/main.cpp
            samples/local/gl/GLMain.cpp
            samples/local/gl/GLRenderer.cpp
    )
    target_link_libraries(local-sample PUBLIC
            glfw
            ${PROJ_NAME}
    )
endif ()