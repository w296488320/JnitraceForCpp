//
// Created by zhenxi on 2022/4/30.
//

#include "../includes/libpath.h"
#include "../includes/version.h"
#include "../includes/adapter.h"


char *getlibArtPath() {
    char *art = nullptr;
#if defined(__aarch64__)
    if (get_sdk_level() >= ANDROID_R) {
        art = (char *) "/apex/com.android.art/lib64/libart.so";
    } else if (get_sdk_level() >= ANDROID_Q) {
        art = (char *) "/apex/com.android.runtime/lib64/libart.so";
    } else {
        art = (char *) "/system/lib64/libart.so";
    }
#else
    if (get_sdk_level() >= ANDROID_R) {
        art = (char*)"/apex/com.android.art/lib/libart.so";
    } else if (get_sdk_level() >= ANDROID_Q) {
        art = (char*)"/apex/com.android.runtime/lib/libart.so";
    } else {
        art = (char*)"/system/lib/libart.so";
    }
#endif

    return art;
}

char *getLinkerPath() {
    char *linker;

    //get_sdk_level 是dlfc自己实现的方法
    //android_get_device_api_level是系统方法,低版本的NDK没有此方法。
#if defined(__aarch64__)
    if (get_sdk_level()>= ANDROID_R) {
        linker = (char *) "/apex/com.android.runtime/bin/linker64";
    } else if (get_sdk_level() >= ANDROID_Q) {
        linker = (char *) "/apex/com.android.runtime/bin/linker64";
    } else {
        linker = (char *) "/system/bin/linker64";
    }
#else
    if (get_sdk_level() >= ANDROID_R) {
        linker = (char*)"/apex/com.android.runtime/bin/linker";
    } else if (get_sdk_level() >= ANDROID_Q) {
        linker = (char*)"/apex/com.android.runtime/bin/linker";
    } else {
        linker = (char*)"/system/bin/linker";
    }
#endif

    return linker;
}

//这里面包含了一些 对string操作的方法
char *getlibcPlusPath() {
    char *libc;
#if defined(__aarch64__)
    libc = (char *) "/system/lib64/libstdc++.so";
#else
    libc = (char*)"/system/lib/libstdc++.so";

#endif
    return libc;
}

char *getlibcPath() {
    char *libc;

#if defined(__aarch64__)
    if (get_sdk_level() >= ANDROID_R) {
        libc = (char *) "/apex/com.android.runtime/lib64/bionic/libc.so";
    } else if (get_sdk_level() >= ANDROID_Q) {
        libc = (char *) "/apex/com.android.runtime/lib64/bionic/libc.so";
    } else {
        libc = (char *) "/system/lib64/libc.so";
    }
#else
    if (get_sdk_level() >= ANDROID_R) {
        libc = (char*)"/apex/com.android.runtime/lib/bionic/libc.so";
    } else if (get_sdk_level() >= ANDROID_Q) {
        libc =(char*) "/apex/com.android.runtime/lib/bionic/libc.so";
    } else {
        libc = (char*)"/system/lib/libc.so";
    }
#endif
    return libc;
}

char *getMediaPath() {
    char *libc;

#if defined(__aarch64__)
    libc = (char *) "/system/lib64/libmediandk.so";
#else
    libc = (char*)"/system/lib/libmediandk.so";
#endif
    return libc;
}

char *getJitPath() {
    char *libc;

#if defined(__aarch64__)
    if (get_sdk_level() >= ANDROID_R) {
        libc = (char *) "/apex/com.android.art/lib64/libart-compiler.so";
    } else if (get_sdk_level() >= ANDROID_Q) {
        libc = (char *) "/apex/com.android.runtime/lib64/libart-compiler.so";
    } else {
        libc = (char *) "/system/lib64/libart-compiler.so";
    }
#else
    if (get_sdk_level() >= ANDROID_R) {
        libc = (char*)"/apex/com.android.art/lib/libart-compiler.so";
    } else if (get_sdk_level() >= ANDROID_Q) {
        libc =(char*) "/apex/com.android.runtime/lib/libart-compiler.so";
    } else {
        libc = (char*)"/system/lib/libart-compiler.so";
    }
#endif
    return libc;
}


