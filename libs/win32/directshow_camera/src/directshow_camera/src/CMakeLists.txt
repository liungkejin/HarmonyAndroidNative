# Get sources file
file(GLOB_RECURSE DIRECTSHOW_CAMERA_SOURCE_FILES
    "*.h" "*.hpp" "*.cpp"
)

# Create static library
add_library(${PROJECT_NAME} STATIC ${DIRECTSHOW_CAMERA_SOURCE_FILES})

# Make sure the compiler can find include files
target_include_directories (${PROJECT_NAME}
    PUBLIC
        ./
)

# Group source files
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${DIRECTSHOW_CAMERA_SOURCE_FILES}
)