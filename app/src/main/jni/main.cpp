#include <jni.h>
#include <logging.h>
#include <JnitraceForC.h>
#include <parse.h>


extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnitrace_MainActivity_startJnitrace(JNIEnv *env, jobject thiz, jobject soname,
                                                     jstring filepath) {

    const list<std::string> &clist = parse::jlist2clist(env, soname);
    string path;
    if (filepath != nullptr) {
        path = parse::jstring2str(env, filepath);
        Jnitrace::startjnitrace(env, clist,const_cast<char *>(path.c_str()));
    } else {
        Jnitrace::startjnitrace(env, clist, nullptr);
    }
}