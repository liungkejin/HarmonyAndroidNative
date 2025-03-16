
set(OpenCV_DIR ${PLATFORM_LIBS_PATH}/opencv-4.3.0/sdk/native/jni)

set(PLATFORM_SOURCES
        src/android/AppContext.cpp
        src/android/utils/JNIUtils.cpp
        src/android/cam/CamManager.cpp
        src/android/cam/CamUtils.cpp
        src/android/cam/CamCharacteristics.cpp
        src/android/cam/CamDevice.cpp
        src/android/cam/CaptureSession.cpp
        src/android/cam/CaptureRequest.cpp
        src/android/image/ImageReader.cpp
)

set(PLATFORM_LIBS
        android
        log
        z
        jnigraphics
        camera2ndk
        mediandk
)

if (${ZNATIVE_ENABLE_GL})
    set(PLATFORM_SOURCES
            ${PLATFORM_SOURCES}
            src/android/surface/NativeWindow.cpp
    )
    set(PLATFORM_LIBS
            ${PLATFORM_LIBS}
            GLESv2
            GLESv3
            EGL
    )
endif ()