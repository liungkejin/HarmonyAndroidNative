//
// Created by LiangKeJin on 2024/11/8.
//

#include "JNIUtils.h"

NAMESPACE_DEFAULT

std::string JNIUtils::jstring2stdString(JNIEnv *env, jstring jstr) {
    const char *strChar = env->GetStringUTFChars(jstr, JNI_FALSE);
    std::string str(strChar);
    env->ReleaseStringUTFChars(jstr, strChar);
    return str;
}

jstring JNIUtils::stdString2jstring(JNIEnv *env, std::string &str) {
    return cstr2jstring(env, str.c_str(), (int)str.length());
}

jstring JNIUtils::cstr2jstring(JNIEnv *env, const char *str, int len) {
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(len);
    env->SetByteArrayRegion(bytes, 0, len, (const jbyte *) str);
    jstring encoding = env->NewStringUTF("UTF-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

jobject JNIUtils::callObjectMethod(JNIEnv *env, jobject obj, const char *method, const char *sig) {
    jclass obj_clz = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(obj_clz, method, sig);
    return env->CallObjectMethod(obj, mid);
}

jobject JNIUtils::callObjectMethodV(JNIEnv *env, jobject obj, const char *method, const char *sig,
                                    const void *arg) {
    jclass obj_clz = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(obj_clz, method, sig);

    return env->CallObjectMethod(obj, mid, arg);
}

int JNIUtils::getNativeFD(JNIEnv *env, jobject fdesc) {
    jclass clazz;
    jfieldID fid;

    /* get the fd from the FileDescriptor */
    if (!(clazz = env->GetObjectClass(fdesc)) ||
        !(fid = env->GetFieldID(clazz, "descriptor", "I")))
        return -1;

    /* return the descriptor */
    return env->GetIntField(fdesc, fid);
}

jobject JNIUtils::newBitmap(JNIEnv *env, int width, int height) {
    jclass clzBmp = env->FindClass("android/graphics/Bitmap");
    jclass clzConf = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID mid = env->GetStaticMethodID(
            clzBmp, "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jfieldID confId = env->GetStaticFieldID(clzConf, "ARGB_8888",
                                            "Landroid/graphics/Bitmap$Config;");
    jobject conf = env->GetStaticObjectField(clzConf, confId);

    return env->CallStaticObjectMethod(clzBmp, mid, width, height, conf);
}

void JNIUtils::recycleBitmap(JNIEnv *env, jobject bitmap) {
    jclass clzBmp = env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(clzBmp, "recycle", "()V");

    env->CallVoidMethod(bitmap, mid);
}

NAMESPACE_END