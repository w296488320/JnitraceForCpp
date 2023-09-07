//
// Created by Administrator on 2020-09-14.
//

#include "includes/parse.h"
#include "includes/fileUtils.h"
#include "includes/logging.h"


#define BUF_SIZE 1024

using namespace std;

[[maybe_unused]] jstring parse::char2jstring(JNIEnv *env, const char *pat) {

    //定义java String类 strClass
    jclass strClass = (env)->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray((jsize) strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, (jsize) strlen(pat), (jbyte *) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}

string parse::get_process_name() {
    //调用系统原生api
    return getprogname();
}

string parse::get_process_name_pid(pid_t pid) {
    //优先尝试读取cmdline
    auto pidStr = fileUtils::readText(
            string("/proc/").append(to_string(pid)).append("/comm"));
    if (pidStr.empty()) {
        pidStr = fileUtils::readText(
                string("/proc/").append(to_string(pid)).append("/cmdline"));
    }
    if(pidStr.empty()){
        return {};
    }
    return pidStr;
}

string parse::jstring2str(JNIEnv *env, jstring jstr) {
    return {env->GetStringUTFChars(jstr, nullptr)};
}

map<string, string> parse::jmap2cmap(JNIEnv *env, jobject jmap) {
    std::map<std::string, std::string> cmap;
    jclass jMapClass = env->FindClass("java/util/HashMap");
    jmethodID jKeySetMethodId = env->GetMethodID(jMapClass, "keySet", "()Ljava/util/Set;");
    jmethodID jGetMethodId = env->GetMethodID(jMapClass, "get",
                                              "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject jSetKey = env->CallObjectMethod(jmap, jKeySetMethodId);
    jclass jSetClass = env->FindClass("java/util/Set");
    jmethodID jToArrayMethodId = env->GetMethodID(jSetClass, "toArray", "()[Ljava/lang/Object;");
    auto jObjArray = (jobjectArray) env->CallObjectMethod(jSetKey, jToArrayMethodId);
    if (jObjArray == nullptr) {
        return cmap;
    }
    jsize size = env->GetArrayLength(jObjArray);
    int i;
    for (i = 0; i < size; i++) {
        auto jkey = (jstring) env->GetObjectArrayElement(jObjArray, i);
        auto jvalue = (jstring) env->CallObjectMethod(jmap, jGetMethodId, jkey);
        if (jvalue == nullptr) {
            continue;
        }
        char *key = (char *) env->GetStringUTFChars(jkey, nullptr);
        char *value = (char *) env->GetStringUTFChars(jvalue, nullptr);
        cmap[std::string(key)] = std::string(value);
    }
    return cmap;
}


std::list<string> parse::jlist2clist(JNIEnv *env, jobject jlist) {
    std::list<std::string> clist;
    jclass listClazz = env->FindClass("java/util/ArrayList");
    jmethodID sizeMid = env->GetMethodID(listClazz, "size", "()I");
    jint size = env->CallIntMethod(jlist, sizeMid);
    jmethodID list_get = env->GetMethodID(listClazz, "get", "(I)Ljava/lang/Object;");
    for (int i = 0; i < size; i++) {
        jobject item = env->CallObjectMethod(jlist, list_get, i);
        //末尾添加
        clist.push_back(parse::jstring2str(env, (jstring) item));
    }
    return clist;
}

jobject parse::clist2jlist(JNIEnv *env, const list<string> &myList) {
    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
    jobject arrayList = env->NewObject(arrayListClass, arrayListConstructor);
    jmethodID arrayListAddMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
    for (const std::string &value: myList) {
        jstring javaString = env->NewStringUTF(value.c_str());
        env->CallBooleanMethod(arrayList, arrayListAddMethod, javaString);
        env->DeleteLocalRef(javaString);
    }
    return arrayList;
}

[[maybe_unused]] bool parse::jboolean2bool(jboolean value) {
    return value == JNI_TRUE;
}


