cmake_minimum_required(VERSION 3.18.1)

set(PROJ_NAME "znative")
project(${PROJ_NAME})

set(CMAKE_CXX_STANDARD 14)

option(BUILD_SHARED_LIB "Build shared library" ON)
option(OPENCV_ENABLE "Use OpenCV" OFF)
option(LIBYUV_ENABLE "Use libyuv" ON)

# debug 时增加宏定义 __DEBUG__
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-D__DEBUG__)
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(PLATFORM "android")
    set(TARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI})
    # Android 的宏定义在 toolchain.cmake 中就定义了为 __ANDROID__
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(PLATFORM "harmony")
    set(TARGET_ARCH ${OHOS_ARCH})
    # 增加宏定义 __HARMONY_OS__
    add_definitions(-D__HARMONY_OS__)
else ()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif ()

message(STATUS "Build status: platform: ${PLATFORM}, arch: ${TARGET_ARCH}, build type: ${CMAKE_BUILD_TYPE}")

set(COM_LIBS_PATH ${CMAKE_SOURCE_DIR}/libs/common)
set(PLATFORM_LIBS_PATH ${CMAKE_SOURCE_DIR}/libs/${PLATFORM})

# tinyformat
include_directories(${COM_LIBS_PATH}/tinyformat)

# eventpp
add_subdirectory(${COM_LIBS_PATH}/eventpp-0.1.3)

# glm
# add_subdirectory(${COM_LIBS_PATH}/glm)

set(THIRD_PARTY_LIBS eventpp)

# OpenCV
if (${OPENCV_ENABLE})
    message(STATUS "Use OpenCV")

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.3.0/sdk/native/jni)
    else ()
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.9.0/${OHOS_ARCH}/lib/cmake/opencv4)
    endif ()
    find_package(OpenCV REQUIRED)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} ${OpenCV_LIBS})
endif ()

# libyuv
if (${USE_LIBYUV})
    message(STATUS "Use libyuv")

    set(libyuv_path ${PLATFORM_LIBS_PATH}/libyuv)
    include_directories(${libyuv_path}/include)
    link_directories(${libyuv_path}/lib/${TARGET_ARCH})
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} yuv)
endif ()

include_directories(src)
set(COMMON_SOURCES
        src/ZNative.cpp
        src/common/utils/Log.cpp
        src/common/utils/YuvUtils.cpp
        src/common/gles/GLUtil.cpp
        src/common/gles/EGLCtx.cpp
        src/common/gles/GLEngine.cpp
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(PLATFORM_SOURCES
            src/android/AppContext.cpp
            src/android/utils/JNIUtils.cpp
            src/android/surface/NativeWindow.cpp
    )
    set(PLATFORM_LIBS
            android
            log
            z
            jnigraphics
            GLESv2
            GLESv3
            EGL
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(PLATFORM_SOURCES
            src/harmony/AppContext.cpp
            src/harmony/utils/NapiUtils.cpp
            src/harmony/xcomp/XComponent.cpp
            src/harmony/xcomp/XCompGLBinder.cpp
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

            ace_ndk.z
            uv
    )
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
    # TODO 静态库还需要安装依赖库
endif ()

install(TARGETS ${PROJ_NAME} DESTINATION ${INSTALL_DIR}/${CMAKE_BUILD_TYPE}/libs/${TARGET_ARCH})

target_link_libraries(${PROJ_NAME} PUBLIC
        ${PLATFORM_LIBS}
        ${THIRD_PARTY_LIBS}
)
