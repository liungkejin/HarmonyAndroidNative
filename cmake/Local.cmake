set(PLATFORM_SOURCES
        ${PLATFORM_SRC_PATH}/AppContext.cpp
)
find_package(OpenGL REQUIRED)
set(PLATFORM_LIBS OpenGL::GL)

if (WIN32)
    include(cmake/Windows.cmake)
elseif (APPLE)
    include(cmake/MacOS.cmake)
else ()
    include(cmake/Linux.cmake)
endif ()