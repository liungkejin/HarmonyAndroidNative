cmake_minimum_required(VERSION 3.18.1)

set(PROJ_NAME "znative")
project(${PROJ_NAME} VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 14)

#
# 定义的几个 CACHE 变量
# ZNATIVE_INCLUDES  所有的include目录
# ZNATIVE_SOURCES   所有的源文件
# ZNATIVE_DEP_LIBS  所有的依赖库
# ZNATIVE_OPENCV_DIR 如果启用 opencv 的话，输出 opencv 的路径
#

option(ZNATIVE_BUILD_SHARED_LIB "Build shared library" ON) # Build shared library
option(ZNATIVE_OPENCV_ENABLE "Enable opencv library" ON) # use opencv library
option(ZNATIVE_LIBYUV_ENABLE "Enable libyuv" ON) # use libyuv

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
set(COM_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/common)
# 平台相关第三方库路径
set(PLATFORM_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/${PLATFORM})

# message(STATUS "Platform libs path: ${PLATFORM_LIBS_PATH}")
set(ALL_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # tinyformat
    ${COM_LIBS_PATH}/tinyformat
    # json
    ${COM_LIBS_PATH}/json
    # eventpp
    ${COM_LIBS_PATH}/eventpp-0.1.3/include
)

# eventpp
add_subdirectory(${COM_LIBS_PATH}/eventpp-0.1.3)

set(THIRD_PARTY_LIBS eventpp)

# OpenCV
if (${ZNATIVE_OPENCV_ENABLE})
    message(STATUS "Use OpenCV")
    add_definitions(-D__OPENCV__)

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.3.0/sdk/native/jni)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.9.0/${OHOS_ARCH}/lib/cmake/opencv4)
    endif ()
    find_package(OpenCV REQUIRED)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    set(ALL_INCLUDES ${ALL_INCLUDES} ${OpenCV_INCLUDE_DIRS})
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} ${OpenCV_LIBS})
endif ()

# libyuv
if (${ZNATIVE_LIBYUV_ENABLE})
    message(STATUS "Use libyuv")

    set(libyuv_path ${COM_LIBS_PATH}/libyuv-stable)
    set(ALL_INCLUDES ${ALL_INCLUDES} ${libyuv_path}/include)
    FILE(GLOB_RECURSE LIBYUV_SRCS ${libyuv_path}/source/*.cc)
endif ()

set(COMMON_SOURCES
        ${LIBYUV_SRCS}
        src/ZNative.cpp
        src/common/Log.cpp
        src/common/utils/YuvUtils.cpp
        src/common/gles/GLUtil.cpp
        src/common/media/ZMedia.cpp
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
    # 拿到 CMAKE_TOOLCHAIN_FILE 的文件夹路径
    get_filename_component(CMAKE_TOOLCHAIN_DIR ${CMAKE_TOOLCHAIN_FILE} DIRECTORY)
    set(HMS_SDK_PATH ${CMAKE_TOOLCHAIN_DIR}/../../../../hms)
    set(ALL_INCLUDES ${ALL_INCLUDES} ${HMS_SDK_PATH}/native/sysroot/usr/include)
    list(APPEND CMAKE_FIND_ROOT_PATH ${HMS_SDK_PATH}/native/sysroot/usr)

    find_library(hiai_foundation-lib hiai_foundation)
    MESSAGE(STATUS "hiai_foundation-lib: ${hiai_foundation-lib} in ${HMS_SDK_PATH}/native/sysroot/usr/lib")
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
            src/harmony/mslite/MSLitePredictor.cpp
            src/harmony/hiai/HiAI.cpp
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
            libhiai_foundation.so
            libneural_network_core.so

            ace_ndk.z
            uv
    )
else ()
    set(PLATFORM_SOURCES
            src/local/AppContext.cpp
    )
    find_package(OpenGL REQUIRED)
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} OpenGL::GL)
endif ()

include_directories(${ALL_INCLUDES})

# install
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/${PLATFORM}/install)
install(FILES src/Namespace.h src/ZNative.h DESTINATION ${INSTALL_DIR}/include)
install(DIRECTORY src/common DESTINATION ${INSTALL_DIR}/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/${PLATFORM} DESTINATION ${INSTALL_DIR}/include/ FILES_MATCHING PATTERN "*.h")

set(ZNATIVE_INCLUDES ${ALL_INCLUDES} CACHE STRING "all includes")
set(ZNATIVE_SOURCES ${COMMON_SOURCES} ${PLATFORM_SOURCES} CACHE STRING "all sources")
set(ZNATIVE_DEP_LIBS ${PLATFORM_LIBS} ${THIRD_PARTY_LIBS} CACHE STRING "all depend libs")
set(ZNATIVE_OPENCV_DIR ${OpenCV_DIR} CACHE STRING "opencv dir")

if (${ZNATIVE_BUILD_SHARED_LIB})
    add_library(${PROJ_NAME} SHARED ${ZNATIVE_SOURCES})
else ()
    add_library(${PROJ_NAME} STATIC ${ZNATIVE_SOURCES})
endif ()

install(TARGETS ${PROJ_NAME} DESTINATION ${INSTALL_DIR}/${CMAKE_BUILD_TYPE}/libs/${TARGET_ARCH})

target_link_libraries(${PROJ_NAME} PUBLIC ${ZNATIVE_DEP_LIBS})

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
            samples/local/test/TestZImage.cpp
    )
    target_link_libraries(local-sample PUBLIC
            glfw
            ${PROJ_NAME}
    )
endif ()