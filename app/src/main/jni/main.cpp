#include <jni.h>
#include <logging.h>
#include <parse.h>
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

#include "includes/JnitraceForC.h"
#include "includes/stringHandler.h"


extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnitrace_MainActivity_startJnitrace(JNIEnv *env, jobject thiz, jobject jmap,jstring save_path) {
    const auto &clist = parse::jlist2clist(env, jmap);
    //如果需要将log保存,filepath写对应路径即可,不需要保存则传nullptr
    auto path = parse::jstring2str(env, save_path);
    auto *saveOs = new ofstream();
    saveOs->open(path, ios::app);
    if (!saveOs->is_open()) {
        return;
    }
    const std::list<string> forbid_list{CORE_SO_NAME};
    Jnitrace::startjnitrace(env, false,forbid_list,clist, saveOs);
    stringHandler::hookStrHandler(false,forbid_list,clist, saveOs);

}