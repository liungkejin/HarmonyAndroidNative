//
// Created by LiangKeJin on 2024/11/8.
//

#pragma once

#include <cstdio>
#include <string>
#include <jni.h>

#include "Namespace.h"

NAMESPACE_DEFAULT

class JNIUtils {
public:
    /**
     * jstring to std::string
     * @param env
     * @param jstr
     * @return
     */
    static std::string jstring2stdString(JNIEnv *env, jstring jstr);

    /**
     * std::string to jstring
     * @return
     */
    static jstring stdString2jstring(JNIEnv *env, std::string& str);

    static jstring cstr2jstring(JNIEnv *env, const char *str, int len);

    /**
     * 方便调用 JNI 方法
     * @param env
     * @param obj
     * @param method
     * @param sig
     * @returnV
     */
    static jobject callObjectMethod(JNIEnv *env, jobject obj, const char *method, const char *sig);
    static jobject callObjectMethodV(JNIEnv *env, jobject obj, const char *method, const char *sig, const void *arg);

    /**
     * 获取 native 文件描述符
     * @param env
     * @param jfd
     * @return
     */
    static int getNativeFD(JNIEnv *env, jobject jfd);

    /**
     * Bitmap.create(int width, int height, Config.ARGB_8888)
     * @param env
     * @param width
     * @param height
     * @return
     */
    static jobject newBitmap(JNIEnv *env, int width, int height);

    /**
     * bitmap.recycle();
     * @param env
     * @param bitmap
     */
    static void recycleBitmap(JNIEnv *env, jobject bitmap);
};

NAMESPACE_END
