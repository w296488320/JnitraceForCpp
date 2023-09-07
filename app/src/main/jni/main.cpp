#include <jni.h>
#include <logging.h>
#include <parse.h>

#include "includes/JnitraceForC.h"
#include "includes/stringHandler.h"


extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnitrace_MainActivity_startJnitrace(JNIEnv *env, jobject thiz, jobject jmap) {
    const auto &clist = parse::jlist2clist(env, jmap);
    //如果需要将log保存,filepath写对应路径即可,不需要保存则传nullptr
    //auto path = parse::jstring2str(env, filepath);
    //auto *saveOs = new ofstream();
    //saveOs->open(path, ios::app);
    //if (!saveOs->is_open()) {
    //    return;
    //}
    //Jnitrace::startjnitrace(env,clist, saveOs);

    const std::list<string> forbid_list{CORE_SO_NAME};

    Jnitrace::startjnitrace(env, false,forbid_list,clist, nullptr);
    stringHandler::hookStrHandler(false,forbid_list,clist, nullptr);

}