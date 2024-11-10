# HarmonyAndroidNative

## 简介

HarmonyAndroidNative 是一个对 HarmonyOS 和 Android Native API 的封装，
同时将两个平台的代码整合在一个项目中，尽可能实现在开发中只维护一套 native 代码。


## 使用

### 项目配置

本项目使用 CLion 作为开发工具, CMake 作为构建工具.
首先先配置 CMake 工具链，分别配置 HarmonyOS 和 Android 的 CMake 工具链。

<img src="./docs/screenshots/android-toolchains.png">
<img src="./docs/screenshots/harmonyos-toolchains.png">

再配置 CMake 的 profile，分别配置 HarmonyOS 和 Android 的 profile。

<img src="./docs/screenshots/android-cmake-profile.png">
<img src="./docs/screenshots/harmonyos-cmake-profile.png">

### 项目结构

```
build/                   # 构建目录
   |_ android/           # Android 构建目录
        |_ debug/        # Android Debug 构建目录
            |_ arm64-v8a/ 
        |_ release/      # Android Release 构建目录
            |_ arm64-v8a/ 
   |_ harmony/           # HarmonyOS 构建目录
        |_ debug/        # HarmonyOS Debug 构建目录
            |_ arm64-v8a/ 
        |_ release/      # HarmonyOS Release 构建目录
            |_ arm64-v8a/ 
   |_ install/           # 安装目录
src/                     # 源码目录
    |_ android/          # Android 源码目录
    |_ harmony/          # HarmonyOS 源码目录
    |_ common/           # 共享源码目录

libs/                    # 依赖库目录
    |_ android/          # Android 依赖库目录
    |_ harmony/          # HarmonyOS 依赖库目录
    |_ common/           # 共享依赖库目录
CMakeLists.txt           # 项目 CMakeLists.txt
```

### 接入

可以选择源码接入，或者作为一个 subdirectory ，或者作为静态库或者动态库使用

#### Log.h

```cpp
// _INFO()
// _DEBUG()
// _WARN()
// _ERROR()
// _FATAL()

// 如果没有定义 LOG_TAG，默认使用 zzz_native
#ifndef LOG_TAG
#define LOG_TAG "zzz_native"
#endif

// 如果不是 debug 模式，默认关闭 debug 和 info 日志
#ifdef __DEBUG__
#define __LOG_DEBUG(msg) OH_LOG_DEBUG(LOG_APP, "%{public}s", msg);
#define __LOG_INFO(msg) OH_LOG_INFO(LOG_APP, "%{public}s", msg);
#else
#define __LOG_DEBUG(msg)
#define __LOG_INFO(msg)
#endif

// debug 模式默认为严格模式，即使用 _ERROR() 直接抛出运行时异常
#ifdef __DEBUG__
#define STRICT_MODE true
#else
#define STRICT_MODE false
#endif

// 设置日志文件路径
void setLogFile(const char *path)
```

#### AppContext.h

```cpp
// 初始化 AppContext
AppContext::initialize(xxx);

// 初始化之后就可以获取 AssetsMgr
AppContext::getAssetsMgr()
```

#### AssetsMgr.h

```cpp
bool isFile(const char *path) {
    return openFile(path) != nullptr;
}

virtual bool isDirectory(const char *path) {
    return openDir(path) != nullptr;
}

/**
 * 打开目录
 * @param path
 * @return 没有文件夹打开失败返回 nullptr
 */
virtual std::shared_ptr<AssetDir> openDir(const char *path) = 0;

std::shared_ptr<AssetFile> openFile(const char *path) {
    return openFile(path, AssetsReadMode::STREAMING);
}

/**
 * 打开文件
 * @param path
 * @return 没有文件打开失败返回 nullptr
 */
virtual std::shared_ptr<AssetFile> openFile(const char *path, AssetsReadMode mode) = 0;

std::shared_ptr<AssetFile64> openFile64(const char *path) {
    return openFile64(path, AssetsReadMode::STREAMING);
}

/**
 * 打开大文件
 * @param path
 * @return 没有文件打开失败返回 nullptr
 */
virtual std::shared_ptr<AssetFile64> openFile64(const char *path, AssetsReadMode mode) = 0;
```