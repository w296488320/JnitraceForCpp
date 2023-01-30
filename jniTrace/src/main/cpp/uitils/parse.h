//
// Created by Administrator on 2020-09-14.
//

#ifndef INC_01_PARSE_H
#define INC_01_PARSE_H


#include <jni.h>
#include <string>
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
#include <string.h>
#include <cerrno>
#include <cstring>
#include <climits>
#include <map>
#include <list>


using namespace std;

class parse {

public:
    static jstring char2jstring(JNIEnv *env, const char *pat);

    static string jstring2str(JNIEnv *env, jstring jstr);

    static std::map<std::string, std::string> jmap2cmap(JNIEnv *env,jobject jmap);
    /**
     * Java list转换成C list
     */
    static std::list<std::string> jlist2clist(JNIEnv *env,jobject jlist);

//    static const char *get_process_name();
//
//    static const char *get_process_name_pid(pid_t pid);

    static bool jboolean2bool(jboolean value);

};


#endif //INC_01_PARSE_H
