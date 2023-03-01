//
// Created by Administrator on 2020-09-14.
//

#include "parse.h"


#define max 100

using namespace std;

jstring parse::char2jstring(JNIEnv *env, const char *pat) {

    //定义java String类 strClass
    jclass strClass = (env)->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
}


std::list<std::string> parse::jlist2clist(JNIEnv *env, jobject jlist) {
    std::list<std::string> clist;
    jclass listClazz = env->FindClass("java/util/ArrayList");
    jmethodID sizeMid = env->GetMethodID(listClazz, "size", "()I");
    jint size = env->CallIntMethod(jlist, sizeMid);
    jmethodID list_get = env->GetMethodID(listClazz, "get", "(I)Ljava/lang/Object;");
    for (int i = 0; i < size; i++) {
        jobject item = env->CallObjectMethod(jlist, list_get, i);
        //末尾添加
        clist.push_back(jstring2str(env, (jstring) item));
    }
    return clist;
}
/**
 * 使用完毕需要清空释放
 * @return
 */
char * parse::get_process_name() {
    char *cmdline = (char *) calloc(0x400u, 1u);
    if (cmdline) {
        FILE *file = fopen("/proc/self/cmdline", "r");
        if (file) {
            int count = fread(cmdline, 1u, 0x400u, file);
            if (count) {
                if (cmdline[count - 1] == '\n') {
                    cmdline[count - 1] = '\0';
                }
            }
            fclose(file);
        }
    }
    return cmdline;
}

//char *parse::int2char(int n) {
//    int m = n;
//    char s[max];
//    char ss[max];
//    int i=0,j=0;
//    if (n < 0)// 处理负数
//    {
//        m = 0 - m;
//        j = 1;
//        ss[0] = '-';
//    }
//    while (m>0)
//    {
//        s[i++] = m % 10 + '0';
//        m /= 10;
//    }
//    s[i] = '\0';
//    i = i - 1;
//    while (i >= 0)
//    {
//        ss[j++] = s[i--];
//    }
//    ss[j] = '\0';
//    return ss;
//}

string parse::jstring2str(JNIEnv *env, jstring jstr) {
    char*   rtn   =   nullptr;
    jclass   clsstring   =   env->FindClass("java/lang/String");
    jstring   strencode   =   env->NewStringUTF("GB2312");
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
    auto   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
    jsize   alen   =   env->GetArrayLength(barr);
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
    if(alen   >   0)
    {
        rtn   =   (char*)malloc(alen+1);
        memcpy(rtn,ba,alen);
        rtn[alen]=0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    std::string stemp(rtn);
    free(rtn);
    return   stemp;
}

bool parse::jboolean2bool(jboolean value) {
    return value == JNI_TRUE;
}


