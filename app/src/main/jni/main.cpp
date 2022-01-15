#include <jni.h>
#include <logging.h>
#include <Jnitrace.h>
#include <parse.h>


extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnitrace_MainActivity_startJnitrace(JNIEnv *env, jobject thiz, jstring soname) {
    string str = parse::jstring2str(env,soname);
    Jnitrace::startjnitrace(env, const_cast<char *>(str.c_str()));

}