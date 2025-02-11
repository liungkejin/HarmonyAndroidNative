set(PLATFORM_SOURCES
        ${PLATFORM_SRC_PATH}/AppContext.cpp
)

if (WIN32)
    include(cmake/Windows.cmake)
elseif (APPLE)
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS OpenGL::GL)

    include(cmake/MacOS.cmake)
else ()
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS OpenGL::GL)

    include(cmake/Linux.cmake)
endif ()