set(PLATFORM_SOURCES
        ${PLATFORM_SRC_PATH}/AppContext.cpp
)

if (NOT WIN32 AND ZNATIVE_GL_ENABLE)
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS OpenGL::GL)
endif ()

if (WIN32)
    include(cmake/Windows.cmake)
elseif (APPLE)
    include(cmake/MacOS.cmake)
else ()
    include(cmake/Linux.cmake)
endif ()