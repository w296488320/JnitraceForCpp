//
// Created by Zhenxi on 2022/10/17.
//


#include "adapter.h"
#include "xdl.h"
#include "logging.h"
#include "xunwind.h"
#include "common_macros.h"

JavaVM *mVm;
JNIEnv *mEnv;


static int SDK_INT = -1;

int get_sdk_level() {
    if (SDK_INT > 0) {
        return SDK_INT;
    }
    char sdk[PROP_VALUE_MAX] = {0};
    __system_property_get("ro.build.version.sdk", sdk);
    char* endptr;
    SDK_INT = (int)strtol(sdk, &endptr, 10);
    if (*endptr != '\0') {
        // 转换失败，处理错误
        return -1;
    }
    return SDK_INT;
}




ScopeUtfString::ScopeUtfString(jstring j_str) {
    _j_str = j_str;
    _c_str = getRunTimeEnv()->GetStringUTFChars(j_str, nullptr);
}

ScopeUtfString::~ScopeUtfString() {
    getRunTimeEnv()->ReleaseStringUTFChars(_j_str, _c_str);
}

JNIEnv *getRunTimeEnv() {
    //一个进程一个env
    //JNIEnv *env;
    if (mEnv == nullptr) {
        mVm->GetEnv(reinterpret_cast<void **>(&mEnv), JNI_VERSION_1_6);
    }
    return mEnv;
}

JNIEnv *ensureEnvCreated() {
    JNIEnv *env = getRunTimeEnv();
    if (env == nullptr) {
        mVm->AttachCurrentThread(&env, nullptr);
    }
    return env;
}


void DetachCurrentThread() {
    mVm->DetachCurrentThread();
}

void ZhenxiRunTime::getExitSignInfo(const std::string& msg,int sign,
                                    void* address,void* context,int pid,int tid){
#if  ZHENXI_BUILD_TYPE == DEBUG
    void *cache = nullptr;
    xdl_info_t info;
    xdl_addr(address,&info,&cache);
    //计算相对偏移
    size_t offset = (size_t) info.dli_saddr - (size_t) info.dli_fbase;
    LOG(ERROR)<<msg<<" sign->["<< sign<<"] offerset: "<< (void*)offset
              <<" ["<<IS_NULL_STR(info.dli_fname)<<"] "<<" ["<<IS_NULL_STR(info.dli_sname)<<"]";
    xdl_addr_clean(&cache);

    //栈回溯打印,打印被调试线程的pid,打印主线程的tid
    xunwind_cfi_log(pid,
                    tid,
                    context,
                    TAG,
                    ANDROID_LOG_ERROR,
                    TAG);
//    char *errorMsg = xunwind_cfi_get(pid, tid, context, TAG);
//    if(errorMsg!= nullptr){
//        LOG(ERROR)<<"error msg \n "<< msg;
//    }
#endif

}