cmake_minimum_required(VERSION 3.18.1)

set(PROJ_NAME "znative")
project(${PROJ_NAME} VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 17)

#
# 几个关键变量说明
# ZPLATFORM                 平台名称 (android | harmony | win32 | linux | macos)
#                           为了方便判断平台，可以使用 ${ZANDROID} | ${ZOHOS} | ${ZLOCAL} | ${ZWINDOWS} | ${ZMACOS} | ${ZLINUX} 这几个变量
# ZTARGET_ARCH               目标平台的架构 x86 | x64 | arm | arm64
# ZNATIVE_BUILD_STATIC        是否构建出静态库
# ZNATIVE_BUILD_LOCAL_SAMPLE  是否构建本地示例，默认启用
# ZNATIVE_ENABLE_OPENCV       是否启用 opencv 默认不启用
# ZNATIVE_ENABLE_GL           是否启用 EGL/OpenGL 默认启用
# ZNATIVE_ENABLE_DEBUG        是否启用 debug级别 日志输出, 默认 Debug 模式开启
#
#
# COMMON_LIBS_PATH          公共第三方库路径
# COMMON_INCLUDES           所有公共的 include 目录
# COMMON_SOURCES            所有公共的源文件
# COMMON_LIBS               所有公共的依赖库
# PLATFORM_LIBS_PATH        平台相关第三方库路径
# PLATFORM_INCLUDES         平台相关的 include 目录
# PLATFORM_SOURCES          平台相关的源文件
# PLATFORM_LIBS             平台相关的依赖库
#
# 输出的 target
# ${PROJ_NAME}-shared       共享库
# ${PROJ_NAME}-static       静态库
#
# 几个 CACHE 变量, 如果是使用 add_subdirectory 的方式引入的话，这些变量可以被引用工程所使用
# ZNATIVE_INCLUDES   所有的include目录
# ZNATIVE_SOURCES    所有的源文件
# ZNATIVE_LIBS       znative 和 所有的依赖库
# ZNATIVE_OPENCV_DIR 如果启用 opencv 的话，输出 opencv 的路径
# ZNATIVE_DEFINITIONS 所有的宏定义
#

option(ZNATIVE_BUILD_STATIC "Build static library" ON) # build static library
option(ZNATIVE_BUILD_LOCAL_SAMPLE "Build local sample" ON) # enable local sample
option(ZNATIVE_ENABLE_OPENCV "Enable opencv library" OFF) # use opencv library
option(ZNATIVE_ENABLE_GL "Enable EGL/OpenGL" ON) # enable OpenGL
option(ZNATIVE_ENABLE_LIBHV "Enable libhv" ON) # enable libhv

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    option(ZNATIVE_ENABLE_DEBUG "Enable debug" ON)
else()
    option(ZNATIVE_ENABLE_DEBUG "Enable debug" OFF)
endif()
if (${ZNATIVE_ENABLE_DEBUG})
    set(ALL_DEFINITIONS "-D__DEBUG__")
endif()
add_definitions(-DSTRICT_MODE=false)

if (${ZNATIVE_BUILD_STATIC})
    set(ZNATIVE_TARGET ${PROJ_NAME}-static)
    add_library(${ZNATIVE_TARGET} STATIC)
else()
    set(ZNATIVE_TARGET ${PROJ_NAME}-shared)
    add_library(${ZNATIVE_TARGET} SHARED)
endif()

set(ZNATIVE ON)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(ZANDROID ON)
    set(ZPLATFORM "android")
    set(ZTARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(ZOHOS ON)
    set(ZPLATFORM "harmony")
    set(ZTARGET_ARCH ${OHOS_ARCH})
else ()
    if (WIN32)
        set(ZPLATFORM "win32")
    elseif (APPLE)
        set(ZPLATFORM "macos")
    else ()
        set(ZPLATFORM "linux")
    endif ()

    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd" OR
            ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD" OR
            ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
            set(ZTARGET_ARCH "x64")
        else ()
            set(ZTARGET_ARCH "x86")
        endif ()
    elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
            set(ZTARGET_ARCH "arm64")
        else ()
            set(ZTARGET_ARCH "arm")
        endif ()
    else ()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ZTARGET_ARCH)
        else ()
            message(FATAL_ERROR "Unknown arch: ${CMAKE_SYSTEM_PROCESSOR}")
            set(ZTARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
        endif ()
    endif ()
endif ()

message(STATUS "option ZNATIVE_BUILD_STATIC: ${ZNATIVE_BUILD_STATIC}")
message(STATUS "option ZNATIVE_ENABLE_OPENCV: ${ZNATIVE_ENABLE_OPENCV}")
message(STATUS "option ZNATIVE_BUILD_LOCAL_SAMPLE: ${ZNATIVE_BUILD_LOCAL_SAMPLE}")
message(STATUS "option ZNATIVE_ENABLE_GL: ${ZNATIVE_ENABLE_GL}")

message(STATUS "Build status: platform: ${ZPLATFORM}, sys name: ${CMAKE_SYSTEM_NAME}, arch: ${ZTARGET_ARCH}, build type: ${CMAKE_BUILD_TYPE}, sys version: ${CMAKE_SYSTEM_VERSION}")

# 公共第三方库路径
set(COMMON_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/common)
set(COMMON_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/common)
# 平台相关第三方库路径
set(PLATFORM_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ZPLATFORM})
set(PLATFORM_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/${ZPLATFORM})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install/${ZPLATFORM}/${CMAKE_BUILD_TYPE})
set(ZNATIVE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/znative)
set(THIRDPARTY_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/thirdparty)
set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/libs/${ZTARGET_ARCH})

set(CMAKE_INSTALL_INCLUDEDIR ${THIRDPARTY_INCLUDE_INSTALL_DIR})
set(CMAKE_INSTALL_LIBDIR ${LIBRARY_INSTALL_DIR})

# eventpp
add_subdirectory(${COMMON_LIBS_PATH}/eventpp-0.1.3)
set(COMMON_LIBS eventpp)
set(COMMON_INCLUDES
        # tinyformat
        ${COMMON_LIBS_PATH}/tinyformat
        # json
        ${COMMON_LIBS_PATH}/json
        # eventpp
        ${COMMON_LIBS_PATH}/eventpp-0.1.3/include
)

# libhv
if (${ZNATIVE_ENABLE_LIBHV})
    add_subdirectory(${COMMON_LIBS_PATH}/libhv-1.3.3)
    set(COMMON_LIBS ${COMMON_LIBS} ${HV_LIBRARIES})
endif ()

if (ZANDROID)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Android.cmake)
elseif (ZOHOS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/HarmonyOS.cmake)
else ()
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Local.cmake)
endif ()

# OpenCV
if (${ZNATIVE_ENABLE_OPENCV})
    message(STATUS "Use OpenCV")
    set(ALL_DEFINITIONS ${ALL_DEFINITIONS} "-D__ZNATIVE_WITH_OPENCV__")

    find_package(OpenCV REQUIRED)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    set(COMMON_LIBS ${COMMON_LIBS} ${OpenCV_LIBS})
    set(COMMON_INCLUDES
            ${COMMON_INCLUDES}
            ${OpenCV_INCLUDE_DIRS}
    )
endif ()

set(LIBYUV_PATH ${COMMON_LIBS_PATH}/libyuv-stable)
FILE(GLOB_RECURSE LIBYUV_SRCS ${LIBYUV_PATH}/source/*.cc)
set(COMMON_SOURCES
        ${LIBYUV_SRCS}
        ${COMMON_SRC_PATH}/../ZNative.cpp
        ${COMMON_SRC_PATH}/Log.cpp
        ${COMMON_SRC_PATH}/utils/YuvUtils.cpp
        ${COMMON_SRC_PATH}/utils/FileUtils.cpp
        ${COMMON_SRC_PATH}/media/ZMedia.cpp
        ${COMMON_SRC_PATH}/net/TCPServer.cpp
)
set(COMMON_INCLUDES
        ${COMMON_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${LIBYUV_PATH}/include
)

if (${ZNATIVE_ENABLE_GL})
    set(COMMON_SOURCES
            ${COMMON_SOURCES}
            ${COMMON_SRC_PATH}/egl/EGLCtx.cpp
            ${COMMON_SRC_PATH}/egl/GLEngine.cpp
            ${COMMON_SRC_PATH}/gles/GLUtil.cpp
    )
endif ()

add_definitions(${ALL_DEFINITIONS})

include_directories(${COMMON_INCLUDES} ${PLATFORM_INCLUDES})
message(STATUS "COMMON_INCLUDES: ${COMMON_INCLUDES}")
message(STATUS "PLATFORM_INCLUDES: ${PLATFORM_INCLUDES}")

target_sources(${ZNATIVE_TARGET} PRIVATE ${COMMON_SOURCES} ${PLATFORM_SOURCES})
target_link_libraries(${ZNATIVE_TARGET} PRIVATE ${PLATFORM_LIBS} ${COMMON_LIBS})

message(STATUS "COMMON_LIBS: ${COMMON_LIBS}")
message(STATUS "PLATFORM_LIBS: ${PLATFORM_LIBS}")

# install
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Install.cmake)

## 注意这些变量 cache string 一旦存储之后就不会改动了，所以不能依赖这个变量
set(ZNATIVE_INCLUDES ${COMMON_INCLUDES} ${PLATFORM_INCLUDES} CACHE INTERNAL "all includes")
set(ZNATIVE_SOURCES ${COMMON_SOURCES} ${PLATFORM_SOURCES} CACHE INTERNAL "all sources")
set(ZNATIVE_LIBS ${ZNATIVE_TARGET} ${PLATFORM_LIBS} ${COMMON_LIBS} CACHE INTERNAL "all libs")
set(ZNATIVE_OPENCV_DIR ${OpenCV_DIR} CACHE INTERNAL "opencv dir")
set(ZNATIVE_DEFINITIONS ${ALL_DEFINITIONS} CACHE INTERNAL "all definitions")

if (${ZNATIVE_BUILD_LOCAL_SAMPLE} AND
        (${ZPLATFORM} MATCHES "win32" OR ${ZPLATFORM} MATCHES "macos" OR ${ZPLATFORM} MATCHES "linux"))
    message(STATUS "Build local sample")
    include(${CMAKE_CURRENT_SOURCE_DIR}/samples/local/Local-sample.cmake)
endif ()
