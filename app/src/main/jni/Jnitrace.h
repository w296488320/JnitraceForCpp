

#ifndef QCONTAINER_PRO_APPUTILS_H
#define QCONTAINER_PRO_APPUTILS_H

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <malloc.h>
#include <regex>
#include <bits/getopt.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <asm/fcntl.h>
#include <logging.h>
#include "limits.h"
#include <string.h>
#include <cerrno>
#include <cstring>
#include <climits>
#include "syscall.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <malloc.h>
#include <regex>
#include <bits/getopt.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <asm/fcntl.h>
#include <fcntl.h>
#include <logging.h>
#include <parse.h>
#include <SubstrateHook.h>



#define HOOK_JNI(env, func) \
     MSHookFunction(    \
      (void*)env->functions->func,\
      (void*)new_##func,\
      (void**)&orig_##func );     \


#define HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)

#define JNI_HOOK_DEF(ret, func, ...)    \
  ret (*orig_##func)(JNIEnv *env, jobject obj, jmethodID jmethodId,...);  \
  ret new_##func(__VA_ARGS__) {         \
//LOG(INFO) << #func<<"("#__VA_ARGS__")" ;\

#define GET_JOBJECT_INFO(env, obj) Jnitrace::getJObjectInfo(env,obj);

#define GET_JOBJECT_INFO_MSG(env, obj,meg) appUtils::getJObjectInfo(env,obj,msg);

#define GET_METHOD_INFO_ARGS(env, obj, methodid, args) Jnitrace::getArgsInfo(env,obj,methodid,args);

typedef size_t Addr;

class Jnitrace {

public:
    static void getArgsInfo(JNIEnv *env, jobject obj, jmethodID jmethodId, va_list  args);

    static void getJObjectInfo(JNIEnv *env, jobject obj);

    static void getJObjectInfo(JNIEnv *env, jobject obj, char * message, bool isPrintClassinfo);

    static char* getJObjectClassInfo(JNIEnv *env, jclass obj);

    static void startjnitrace(JNIEnv * env, char * soname);

private:

};


#endif //QCONTAINER_PRO_APPUTILS_H
