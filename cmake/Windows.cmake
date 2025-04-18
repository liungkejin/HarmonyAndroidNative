option(ZNATIVE_WIN32_DS_CAMERA_ENABLE "Enable directshow camera library" OFF) # use directshow camera library
option(ZNATIVE_WIN32_LIB_DS_CAPTURE_ENABLE "Enable libdshowcapture library" ON) # use libdshowcapture library

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd" OR
        ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD" OR
        ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
        add_definitions(-D_AMD64_)
    else ()
        add_definitions(-D_X86_)
    endif ()
elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
        add_definitions(-D_ARM64_)
    else ()
        add_definitions(-D_ARM_)
    endif ()
endif ()

# Windows 上使用 Visual Studio 工具链时，编译的 opencv 静态库时，注意将 BUILD_WITH_STATIC_CRT 置为 OFF
# 同时需要编译两份，一份是 debug 版本，一份是 release 版本
if (MSVC)
    if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(OpenCV_DIR "${PLATFORM_LIBS_PATH}/opencv-4.10.0/vc17-debug/x64/vc17/staticlib")
    else ()
        set(OpenCV_DIR "${PLATFORM_LIBS_PATH}/opencv-4.10.0/vc17-release/x64/vc17/staticlib")
    endif ()
else ()
    # MINGW
    set(OpenCV_DIR "${PLATFORM_LIBS_PATH}/opencv-4.10.0/mingw-release/x64/mingw/staticlib")
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

if (${ZNATIVE_ENABLE_GL})
    set(GLEW_DIR "${PLATFORM_LIBS_PATH}/glew-2.1.0")
    add_subdirectory(${GLEW_DIR})

    set(PLATFORM_LIBS
            ${PLATFORM_LIBS}
            glew_s
    )
    set(PLATFORM_INCLUDES
            ${PLATFORM_INCLUDES}
            ${GLEW_DIR}/include
    )
endif ()

if (${ZNATIVE_WIN32_DS_CAMERA_ENABLE})
    set(DS_CAMERA_DIR "${PLATFORM_LIBS_PATH}/directshow_camera")
    add_subdirectory(${DS_CAMERA_DIR})
    set(PLATFORM_LIBS
            ${PLATFORM_LIBS}
            directshow_camera
    )
    set(PLATFORM_INCLUDES
            ${PLATFORM_INCLUDES}
            ${DS_CAMERA_DIR}/src/directshow_camera/src
    )
endif ()

if (${ZNATIVE_WIN32_LIB_DS_CAPTURE_ENABLE})
    set(LIB_DS_CAPTURE_DIR "${PLATFORM_LIBS_PATH}/libdshowcapture")
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(${LIB_DS_CAPTURE_DIR})

    set(PLATFORM_LIBS
            ${PLATFORM_LIBS}
            libdshowcapture
    )
    set(PLATFORM_INCLUDES
            ${PLATFORM_INCLUDES}
            ${LIB_DS_CAPTURE_DIR}
    )
    set(PLATFORM_SOURCES
            ${PLATFORM_SOURCES}
            ${PLATFORM_SRC_PATH}/dshow/DSCaptureMgr.cpp
    )
endif ()

set(PLATFORM_SOURCES
        ${PLATFORM_SOURCES}
        ${PLATFORM_SRC_PATH}/dshow/DeviceEnumerator.cpp
        ${PLATFORM_SRC_PATH}/cam/CamDevice.cpp
)
set(PLATFORM_LIBS
        ${PLATFORM_LIBS}
        quartz
        strmiids
)