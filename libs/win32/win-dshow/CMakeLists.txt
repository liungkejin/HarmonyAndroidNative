cmake_minimum_required(VERSION 3.28...3.30)
project(win-dshow)

set(FFmpeg_DIR "D:/projects/github/obs-studio/.deps/obs-deps-2024-09-12-x64/lib")
find_package(FFmpeg REQUIRED avcodec avutil)
#set(libavcodec ${DEPS_DIR}/lib/libavcodec.lib)
#set(libavutil ${DEPS_DIR}/lib/libavutil.lib)
include_directories(
        ${DEPS_DIR}/include
)

#add_library(win-dshow INTERFACE)

#if(NOT TARGET OBS::tiny-nv12-scale)
#  add_subdirectory("${CMAKE_SOURCE_DIR}/shared/obs-tiny-nv12-scale" obs-tiny-nv12-scale)
#endif()
#
#if(NOT TARGET OBS::shared-memory-queue)
#  add_subdirectory("${CMAKE_SOURCE_DIR}/shared/obs-shared-memory-queue" obs-shared-memory-queue)
#endif()
#
#if(NOT TARGET OBS::libdshowcapture)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../libdshowcapture" libdshowcapture)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../libdshowcapture
)
#endif()
#
#if(NOT TARGET OBS::winhandle)
#  add_subdirectory("${CMAKE_SOURCE_DIR}/libobs" "${CMAKE_BINARY_DIR}/libobs")
#endif()

#configure_file(cmake/windows/obs-module.rc.in win-dshow.rc)
#target_sources(win-dshow PRIVATE win-dshow.rc)

add_library(win-dshow
        dshow-plugin.cpp encode-dstr.hpp ffmpeg-decode.c ffmpeg-decode.h win-dshow-encoder.cpp win-dshow.cpp shared-memory-queue.c tiny-nv12-scale.c
)

target_link_libraries(
  win-dshow
  PRIVATE
    libobs
    libdshowcapture
        avcodec
        avutil
#        ${libavcodec}
#        ${libavutil}
    strmiids
    winmm
)

#add_subdirectory(virtualcam-module)

#if(TARGET OBS::virtualcam AND TARGET OBS::virtualcam-guid)
#  target_sources(win-dshow PRIVATE virtualcam.c)
#  target_link_libraries(win-dshow PRIVATE OBS::virtualcam-guid)
#  target_compile_definitions(win-dshow PRIVATE VIRTUALCAM_AVAILABLE)
#
#  add_dependencies(win-dshow obs-virtualcam-module)
#endif()

#set_target_properties_obs(win-dshow PROPERTIES FOLDER plugins/win-dshow PREFIX "")
