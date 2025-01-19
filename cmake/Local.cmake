set(PLATFORM_SOURCES
        src/local/AppContext.cpp
)
find_package(OpenGL REQUIRED)
set(PLATFORM_LIBS OpenGL::GL)

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif ()

if (WIN32)
    include(cmake/Windows.cmake)
endif ()