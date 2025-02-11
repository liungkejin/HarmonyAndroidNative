set(LOCAL_SAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/samples/local)
set(SAMPLE_LIBS_DIR ${LOCAL_SAMPLE_DIR}/libs)
set(SAMPLE_SRC_DIR ${LOCAL_SAMPLE_DIR}/src)

set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(${SAMPLE_LIBS_DIR}/glfw-3.4)
set(SAMPLE_LIBS glfw)

if (WIN32)
    include(${LOCAL_SAMPLE_DIR}/Windows-sample.cmake)
endif ()

if (NOT ${ZNATIVE_OPENCV_ENABLE})
    find_package(OpenCV REQUIRED)
    set(SAMPLE_INCLUDES ${SAMPLE_INCLUDES} ${OpenCV_INCLUDE_DIRS})
    set(SAMPLE_LIBS ${SAMPLE_LIBS} ${OpenCV_LIBS})
endif ()

include_directories(
        ${SAMPLE_LIBS_DIR}/glfw-3.4/include
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/backends
        ${SAMPLE_INCLUDES}
)

add_definitions(-DLOCAL_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets")

set(IMGUI_SRC
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/imgui.cpp
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/imgui_demo.cpp
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/imgui_draw.cpp
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/imgui_tables.cpp
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/imgui_widgets.cpp
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/backends/imgui_impl_glfw.cpp
        ${SAMPLE_LIBS_DIR}/imgui-1.90.9/backends/imgui_impl_opengl3.cpp
)

set(SAMPLE_SOURCES
        ${SAMPLE_SOURCES}
        ${SAMPLE_SRC_DIR}/main.cpp
        ${SAMPLE_SRC_DIR}/MainApp.cpp
        ${SAMPLE_SRC_DIR}/MainWindow.cpp
        ${SAMPLE_SRC_DIR}/gl/GLTestWindow.cpp
        ${SAMPLE_SRC_DIR}/test/TestZImage.cpp
)

add_executable(local-sample
        ${IMGUI_SRC}
        ${SAMPLE_SOURCES}
)

target_link_libraries(local-sample PRIVATE
        ${COMMON_LIBS}
        ${PLATFORM_LIBS}
        ${TARGET_STATIC}
        ${SAMPLE_LIBS}
)
