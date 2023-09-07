//
// Created by Zhenxi on 2022/10/17.
//
#include <sys/system_properties.h>
#include <cstdlib>
#include <jni.h>
#include <string>
#include <map>
#include <list>
#include <cstring>
#include <cstdio>
#include <regex>
#include <cerrno>
#include <climits>
#include <iostream>
#include <fstream>


#ifndef ZHENXIRUNTIME_ADAPTER_H
#define ZHENXIRUNTIME_ADAPTER_H

extern JavaVM *mVm;
extern JNIEnv *mEnv;

JNIEnv *getRunTimeEnv();
JNIEnv *ensureEnvCreated();
void DetachCurrentThread();
int get_sdk_level();

namespace ZhenxiRunTime{
    /**
     * 打印具体的退出原因
     */
    void getExitSignInfo(const std::string& msg,int sign,
                         void* address,void* context,int pid,int tid);
}


class ScopeUtfString {
public:
    ScopeUtfString(jstring j_str);


    const char *c_str() {
        return _c_str;
    }

    ~ScopeUtfString();

private:
    jstring _j_str;
    const char *_c_str;
};


#endif //ZHENXIRUNTIME_ADAPTER_H
