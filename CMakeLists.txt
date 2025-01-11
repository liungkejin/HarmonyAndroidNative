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
    # 增加宏定义 __HARMONYOS__
    add_definitions(-D__HARMONYOS__)
else ()
    set(PLATFORM "local")
    add_definitions(-D__LOCAL_OS__)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE TARGET_ARCH)
    else ()
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd" OR
                ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD" OR
                ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
            if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
                set(TARGET_ARCH "x64")
            else ()
                set(TARGET_ARCH "x86")
            endif ()
        elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
            if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
                set(TARGET_ARCH "arm64")
            else ()
                set(TARGET_ARCH "armv7")
            endif ()
        else ()
            message(WARNING "Unknown arch: ${CMAKE_SYSTEM_PROCESSOR}")
            set(TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
        endif ()
    endif ()
endif ()

# 实现在 PLATFORM 为 local 时，默认 ZNATIVE_BUILD_SHARED_LIB 为 ON， 其他平台默认为 OFF
set(DEFAULT_ZNATIVE_BUILD_SHARED_LIB OFF)
if (${PLATFORM} STREQUAL "local")
    set(DEFAULT_ZNATIVE_BUILD_SHARED_LIB ON)
endif ()
option(ZNATIVE_BUILD_SHARED_LIB "Build shared library" ${DEFAULT_ZNATIVE_BUILD_SHARED_LIB})
option(ZNATIVE_OPENCV_ENABLE "Enable opencv library" ON) # use opencv library

message(STATUS "ZNATIVE_BUILD_SHARED_LIB: ${ZNATIVE_BUILD_SHARED_LIB}")
message(STATUS "ZNATIVE_OPENCV_ENABLE: ${ZNATIVE_OPENCV_ENABLE}")
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
    # cppfs
    ${COM_LIBS_PATH}/source/cppfs/include
)

# eventpp
add_subdirectory(${COM_LIBS_PATH}/eventpp-0.1.3)

# cppfs
set(BUILD_SHARED_LIBS OFF)
set(OPTION_BUILD_TESTS OFF)
add_subdirectory(${COM_LIBS_PATH}/cppfs-1.2.0)

set(THIRD_PARTY_LIBS eventpp cppfs)

# OpenCV
if (${ZNATIVE_OPENCV_ENABLE})
    message(STATUS "Use OpenCV")
    add_definitions(-D__OPENCV__)

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.3.0/sdk/native/jni)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
        set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.9.0/${OHOS_ARCH}/lib/cmake/opencv4)
    endif ()
    if (WIN32)
        # 这里配置自己的路径, 注意 opencv 必须使用 mingw 编译的.a，而且不能带.a 文件版本后缀，不是 cmake-gui 编译出来的.lib
        set(OpenCV_DIR "${PLATFORM_LIBS_PATH}/win32/opencv-4.9.0/x64/mingw/staticlib")
    endif ()
    find_package(OpenCV REQUIRED)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    set(ALL_INCLUDES ${ALL_INCLUDES} ${OpenCV_INCLUDE_DIRS})
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} ${OpenCV_LIBS})
endif ()

set(libyuv_path ${COM_LIBS_PATH}/libyuv-stable)
set(ALL_INCLUDES ${ALL_INCLUDES} ${libyuv_path}/include)
FILE(GLOB_RECURSE LIBYUV_SRCS ${libyuv_path}/source/*.cc)
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
#    MESSAGE(STATUS "hiai_foundation-lib: ${hiai_foundation-lib} in ${HMS_SDK_PATH}/native/sysroot/usr/lib")
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
            ${hiai_foundation-lib}
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

    if(WIN32)
        # 最低支持 win7 , cppfs 中定义了 _WIN32_WINNT 和 WINVER, 否则需要定义 -D_WIN32_WINNT=0x0601 -DWINVER=0x0601
        add_definitions(-D_AFXDLL)
        set(CMAKE_MFC_FLAG 2)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

        set(GLEW_DIR "${PLATFORM_LIBS_PATH}/win32/glew-2.1.0")
        add_subdirectory(${GLEW_DIR})
        set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} glew_s)
        set(ALL_INCLUDES ${ALL_INCLUDES} ${GLEW_DIR}/include)
    endif ()
endif ()

include_directories(${ALL_INCLUDES})

# install
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/${PLATFORM}/install)
install(FILES src/Namespace.h src/ZNative.h DESTINATION ${INSTALL_DIR}/include)
install(DIRECTORY src/common DESTINATION ${INSTALL_DIR}/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/${PLATFORM} DESTINATION ${INSTALL_DIR}/include/ FILES_MATCHING PATTERN "*.h")

## 注意这些变量 cache string 一旦存储之后就不会改动了，所以不能依赖这个变量
set(ZNATIVE_INCLUDES ${ALL_INCLUDES} CACHE STRING "all includes")
set(ZNATIVE_SOURCES ${COMMON_SOURCES} ${PLATFORM_SOURCES} CACHE STRING "all sources")
set(ZNATIVE_DEP_LIBS ${PLATFORM_LIBS} ${THIRD_PARTY_LIBS} CACHE STRING "all depend libs")
set(ZNATIVE_OPENCV_DIR ${OpenCV_DIR} CACHE STRING "opencv dir")

message(STATUS "dep libs: ${PLATFORM_LIBS} ${THIRD_PARTY_LIBS}")

if (${ZNATIVE_BUILD_SHARED_LIB})
    message(STATUS "build shared library")
    add_library(${PROJ_NAME} SHARED ${COMMON_SOURCES} ${PLATFORM_SOURCES})
else ()
    message(STATUS "build static library")
    add_library(${PROJ_NAME} STATIC ${COMMON_SOURCES} ${PLATFORM_SOURCES})
endif ()

install(TARGETS ${PROJ_NAME} DESTINATION ${INSTALL_DIR}/${CMAKE_BUILD_TYPE}/libs/${TARGET_ARCH})

target_link_libraries(${PROJ_NAME} PUBLIC ${PLATFORM_LIBS} ${THIRD_PARTY_LIBS})

if (${PLATFORM} STREQUAL "local")
    include_directories(
            ${PLATFORM_LIBS_PATH}/glfw-3.4/include
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9
            ${PLATFORM_LIBS_PATH}/imgui-1.90.9/backends
    )

    set(GLFW_INSTALL OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_BUILD_TESTS OFF)
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

    if (${WIN32})
        set(WIN32_SRC
                samples/local/win32/WinCamMgr.cpp
        )
    endif ()

    add_executable(local-sample
            ${IMGUI_SRC}
            ${WIN32_SRC}
            samples/local/main.cpp
            samples/local/gl/GLMain.cpp
            samples/local/gl/GLRenderer.cpp
            samples/local/test/TestZImage.cpp
    )
    target_link_libraries(local-sample PRIVATE
            glfw
            ${PROJ_NAME}
            ${PLATFORM_LIBS} ${THIRD_PARTY_LIBS}
    )
endif ()