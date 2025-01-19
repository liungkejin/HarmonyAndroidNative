cmake_minimum_required(VERSION 3.18.1)

set(PROJ_NAME "znative")
project(${PROJ_NAME} VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 17)

#
# 几个关键变量说明
# PLATFORM                  平台名称 android | harmony | local (local 表示桌面平台，具体的平台可以是 win32 | linux | macos)
#                           为了方便判断平台，可以使用 ${ZANDROID} | ${ZOHOS} | ${ZLOCAL} | ${ZWINDOWS} | ${ZMACOS} | ${ZLINUX} 这几个变量
# TARGET_ARCH               目标平台的架构 x86 | x64 | arm | arm64
# ZNATIVE_OPENCV_ENABLE     是否启用 opencv 默认启用
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
# 几个 CACHE 变量, 如果是使用 add_subdirectory 的方式引入的话，这些变量可以引用工程所使用
# ZNATIVE_INCLUDES   所有的include目录
# ZNATIVE_SOURCES    所有的源文件
# ZNATIVE_DEP_LIBS   所有的依赖库
# ZNATIVE_OPENCV_DIR 如果启用 opencv 的话，输出 opencv 的路径
#

set(TARGET_SHARED ${PROJ_NAME}-shared)
set(TARGET_STATIC ${PROJ_NAME}-static)
add_library(${TARGET_SHARED} SHARED)
add_library(${TARGET_STATIC} STATIC)

# debug 时增加宏定义 __DEBUG__
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-D__DEBUG__)
endif()

add_definitions(-DSTRICT_MODE=false)

option(ZNATIVE_OPENCV_ENABLE "Enable opencv library" ON) # use opencv library

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(ZANDROID ON)
    set(PLATFORM "android")
    set(TARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(ZOHOS ON)
    set(PLATFORM "harmony")
    set(TARGET_ARCH ${OHOS_ARCH})
else ()
    set(ZLOCAL ON)
    set(PLATFORM "local")
    add_definitions(-D__LOCAL_OS__)
    if (WIN32)
        set(ZWINDOWS ON)
    elseif (APPLE)
        set(ZMACOS ON)
    else ()
        set(ZLINUX ON)
    endif ()

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
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE TARGET_ARCH)
        else ()
            message(FATAL_ERROR "Unknown arch: ${CMAKE_SYSTEM_PROCESSOR}")
            set(TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
        endif ()
    endif ()
endif ()

message(STATUS "ZNATIVE_OPENCV_ENABLE: ${ZNATIVE_OPENCV_ENABLE}")
message(STATUS "Build status: platform: ${PLATFORM}, sys name: ${CMAKE_SYSTEM_NAME}, arch: ${TARGET_ARCH}, build type: ${CMAKE_BUILD_TYPE}, sys version: ${CMAKE_SYSTEM_VERSION}")

# 公共第三方库路径
set(COMMON_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/common)
# 平台相关第三方库路径
set(PLATFORM_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/${PLATFORM})

# eventpp
add_subdirectory(${COMMON_LIBS_PATH}/eventpp-0.1.3)

# cppfs
set(BUILD_SHARED_LIBS OFF)
set(OPTION_BUILD_TESTS OFF)
add_subdirectory(${COMMON_LIBS_PATH}/cppfs-1.2.0)

set(COMMON_LIBS eventpp cppfs)
set(COMMON_INCLUDES
        # tinyformat
        ${COMMON_LIBS_PATH}/tinyformat
        # json
        ${COMMON_LIBS_PATH}/json
        # eventpp
        ${COMMON_LIBS_PATH}/eventpp-0.1.3/include
        # cppfs
        ${COMMON_LIBS_PATH}/source/cppfs/include
)

if (${ZANDROID})
    include(cmake/Android.cmake)
elseif (${ZOHOS})
    include(cmake/Harmonyos.cmake)
else ()
    include(cmake/Local.cmake)
endif ()

# OpenCV
if (${ZNATIVE_OPENCV_ENABLE})
    message(STATUS "Use OpenCV")
    add_definitions(-D__OPENCV__)

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
        ${CMAKE_SOURCE_DIR}/src/ZNative.cpp
        ${CMAKE_SOURCE_DIR}/src/common/Log.cpp
        ${CMAKE_SOURCE_DIR}/src/common/utils/YuvUtils.cpp
        ${CMAKE_SOURCE_DIR}/src/common/utils/FileUtils.cpp
        ${CMAKE_SOURCE_DIR}/src/common/egl/EGLCtx.cpp
        ${CMAKE_SOURCE_DIR}/src/common/egl/GLEngine.cpp
        ${CMAKE_SOURCE_DIR}/src/common/gles/GLUtil.cpp
        ${CMAKE_SOURCE_DIR}/src/common/media/ZMedia.cpp
)
set(COMMON_INCLUDES
        ${COMMON_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${LIBYUV_PATH}/include
)

include_directories(${COMMON_INCLUDES} ${PLATFORM_INCLUDES})
message(STATUS "COMMON_INCLUDES: ${COMMON_INCLUDES}")
message(STATUS "PLATFORM_INCLUDES: ${PLATFORM_INCLUDES}")

target_sources(${TARGET_SHARED} PRIVATE ${COMMON_SOURCES} ${PLATFORM_SOURCES})
target_sources(${TARGET_STATIC} PRIVATE ${COMMON_SOURCES} ${PLATFORM_SOURCES})

target_link_libraries(${TARGET_SHARED} PRIVATE ${PLATFORM_LIBS} ${COMMON_LIBS})
target_link_libraries(${TARGET_STATIC} PRIVATE ${PLATFORM_LIBS} ${COMMON_LIBS})

message(STATUS "COMMON_LIBS: ${COMMON_LIBS}")
message(STATUS "PLATFORM_LIBS: ${PLATFORM_LIBS}")

## 注意这些变量 cache string 一旦存储之后就不会改动了，所以不能依赖这个变量
set(ZNATIVE_INCLUDES ${COMMON_INCLUDES} ${PLATFORM_INCLUDES} CACHE STRING "all includes")
set(ZNATIVE_SOURCES ${COMMON_SOURCES} ${PLATFORM_SOURCES} CACHE STRING "all sources")
set(ZNATIVE_DEP_LIBS ${PLATFORM_LIBS} ${COMMON_LIBS} CACHE STRING "all depend libs")
set(ZNATIVE_OPENCV_DIR ${OpenCV_DIR} CACHE STRING "opencv dir")

# TODO install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${PLATFORM})
install(FILES src/Namespace.h src/ZNative.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(DIRECTORY src/common DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/${PLATFORM} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ FILES_MATCHING PATTERN "*.h")

install(TARGETS ${TARGET_SHARED} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/libs/${TARGET_ARCH})
install(TARGETS ${TARGET_STATIC} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/libs/${TARGET_ARCH})

if (${ZLOCAL})
    include(cmake/Local-sample.cmake)
endif ()
