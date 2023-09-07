//
// Created by swift on 3/12/21.
//

#pragma once

#include <android/log.h>


#ifdef ZHENXI_BUILD_TYPE_NOLOG

#define LOGE(...)   ((void)0);
#define LOGI(...)   ((void)0);
#define LOGD(...)   ((void)0);
#define LOGW(...)   ((void)0);
#define ALOGI(...) ((void)0);
#define ALOGD(...) ((void)0);
#define ALOGW(...) ((void)0);
#define ALOGE(...) ((void)0);



#else

#define TAG "Sandhook"


#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG ,__VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG ,__VA_ARGS__);
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG ,__VA_ARGS__);

//#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG ,__VA_ARGS__);
//#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG ,__VA_ARGS__);
//#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG ,__VA_ARGS__);
//#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG ,__VA_ARGS__);


#endif
