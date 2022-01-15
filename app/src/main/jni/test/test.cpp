//
// Created by Zhenxi on 2022/1/15.
//

#include <jni.h>
#include <logging.h>


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *_vm, void *) {

    LOG(INFO) << "start test  JNI_OnLoad ";

    JNIEnv *env;
    _vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);

    jclass tracetest = env->FindClass("com/example/jnitrace/traceTest");

    jmethodID md1 = env->GetStaticMethodID(tracetest, "test4", "(Ljava/lang/String;)I");
    
    env->CallStaticIntMethod(tracetest,md1,env->NewStringUTF("666"));

    LOG(INFO) << "test  invoke finsh  ";

    return JNI_VERSION_1_6;
}