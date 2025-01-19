# ZNativeConfig.cmake

set(ZSYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(ZSYSTEM_ARCH ${CMAKE_ANDROID_ARCH_ABI})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OHOS")
    set(ZSYSTEM_ARCH ${OHOS_ARCH})
else ()
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd" OR
            ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD" OR
            ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
        if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
            set(ZSYSTEM_ARCH "x64")
        else ()
            set(ZSYSTEM_ARCH "x86")
        endif ()
    elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
        if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
            set(ZSYSTEM_ARCH "arm64")
        else ()
            set(ZSYSTEM_ARCH "arm")
        endif ()
    else ()
        if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ZSYSTEM_ARCH)
        endif ()
    endif ()
endif ()

message(STATUS "ZSYSTEM_ARCH: ${ZSYSTEM_ARCH}")
message(STATUS "CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

get_filename_component(ZNative_CONFIG_PATH "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(ZNative_INSTALL_PATH "${OpenCV_CONFIG_PATH}/../../../" REALPATH)
message(STATUS "ZNative_CONFIG_PATH: ${ZNative_CONFIG_PATH}")
message(STATUS "ZNative_INSTALL_PATH: ${ZNative_INSTALL_PATH}")

# 查找库的头文件目录
find_path(ZNative_INCLUDE_DIRS
        NAMES znative
        PATHS ${ZNative_CONFIG_PATH}/include
)

# 查找库文件
find_library(ZNative_LIBRARIES
        NAMES znative-static
        PATHS ${ZNative_CONFIG_PATH}/libs/${ZSYSTEM_ARCH}
)

# 判断是否找到
if (${ZNative_LIBRARIES} STREQUAL "ZNative_LIBRARIES-NOTFOUND")
    set(IS_STATIC_ZNATIVE OFF)
    find_library(ZNative_LIBRARIES
            NAMES znative-shared
            PATHS ${ZNative_CONFIG_PATH}/libs/${ZSYSTEM_ARCH}
    )
else ()
    set(IS_STATIC_ZNATIVE ON)
    find_library(Cppfs_LIBRARIES
            NAMES cppfs
            PATHS ${ZNative_CONFIG_PATH}/libs/${ZSYSTEM_ARCH}
    )
    if (${Cppfs_LIBRARIES} STREQUAL "Cppfs_LIBRARIES-NOTFOUND")
        find_library(Cppfs_LIBRARIES
                NAMES cppfsd
                PATHS ${ZNative_CONFIG_PATH}/libs/${ZSYSTEM_ARCH}
        )
    endif ()

    set(ZNative_LIBRARIES
            ${ZNative_LIBRARIES}
            ${Cppfs_LIBRARIES}
    )
endif ()

set(ZNative_INCLUDE_DIRS
        ${ZNative_INCLUDE_DIRS}/znative
        ${ZNative_CONFIG_PATH}/include/thirdparty
)

message(STATUS "ZNative_INCLUDE_DIRS: ${ZNative_INCLUDE_DIRS}")

message(STATUS "ZNative_LIBRARIES: ${ZNative_LIBRARIES}")

# 检查是否找到头文件和库文件
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZNative
        DEFAULT_MSG
        ZNative_INCLUDE_DIRS
        ZNative_LIBRARIES
)

# 如果找到，将库添加到目标
if (ZNative_FOUND)
    if (NOT TARGET ZNative)
        if (${IS_STATIC_ZNATIVE})
            add_library(ZNative STATIC IMPORTED)
        else ()
            add_library(ZNative SHARED IMPORTED)
        endif ()
        set_target_properties(ZNative PROPERTIES
                IMPORTED_LOCATION "${ZNative_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${ZNative_INCLUDE_DIRS}"
        )
    endif ()
endif ()