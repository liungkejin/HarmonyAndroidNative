
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(${PLATFORM_LIBS_PATH}/glfw-3.4)

# PLATFORM_SAMPLE_SOURCES is defined in cmake/Windows-sample.cmake
# PLATFORM_SAMPLE_INCLUDES is defined in cmake/Windows-sample.cmake

if (${WIN32})
    include(${CMAKE_SOURCE_DIR}/cmake/Windows-sample.cmake)
endif ()

include_directories(
        ${PLATFORM_LIBS_PATH}/glfw-3.4/include
        ${PLATFORM_LIBS_PATH}/imgui-1.90.9
        ${PLATFORM_LIBS_PATH}/imgui-1.90.9/backends
        ${PLATFORM_SAMPLE_INCLUDES}
)

add_definitions(-DLOCAL_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets")

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
        ${PLATFORM_SAMPLE_SOURCES}
        samples/local/main.cpp
        samples/local/MainApp.cpp
        samples/local/MainWindow.cpp
        samples/local/gl/GLTestWindow.cpp
        samples/local/test/TestZImage.cpp
)
target_link_libraries(local-sample PRIVATE
        glfw
        ${COMMON_LIBS}
        ${PLATFORM_LIBS}
        ${TARGET_STATIC}
)
