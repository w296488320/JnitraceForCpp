

#include "Jnitrace.h"


static string filterSoName;



//jobject CallObjectMethod(JNIEnv*, jobject, jmethodID, va_list args);
JNI_HOOK_DEF(jobject, CallObjectMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jobject ret = orig_CallObjectMethodV(env, obj, jmethodId, args);
        Jnitrace::getJObjectInfo(env, ret, "result object :", true);
        return ret;
    }
    return orig_CallObjectMethodV(env, obj, jmethodId, args);
}

//void CallVoidMethod(jobject obj, jmethodID methodID, va_list args)
JNI_HOOK_DEF(void, CallVoidMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
    }
    return orig_CallVoidMethodV(env, obj, jmethodId, args);
}

JNI_HOOK_DEF(jboolean, CallBooleanMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
             va_list args)

    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jboolean ret = orig_CallBooleanMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }

    return orig_CallBooleanMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jbyte, CallByteMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jbyte ret = orig_CallByteMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }

    return orig_CallByteMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jchar, CallCharMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jchar ret = orig_CallCharMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallCharMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jshort, CallShortMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jshort ret = orig_CallShortMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallShortMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jint, CallIntMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jint ret = orig_CallIntMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallIntMethodV(env, obj, jmethodId, args);

}


JNI_HOOK_DEF(jlong, CallLongMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jlong ret = orig_CallLongMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallLongMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jfloat, CallFloatMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jfloat ret = orig_CallFloatMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallFloatMethodV(env, obj, jmethodId, args);

}

JNI_HOOK_DEF(jdouble, CallDoubleMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jdouble ret = orig_CallDoubleMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallDoubleMethodV(env, obj, jmethodId, args);
}



//------------------ static ------------------------------------------------------------

JNI_HOOK_DEF(jobject, CallStaticObjectMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jobject ret = orig_CallStaticObjectMethodV(env, obj, jmethodId, args);
        Jnitrace::getJObjectInfo(env, ret, "result object :", true);
        return ret;
    }
    return orig_CallStaticObjectMethodV(env, obj, jmethodId, args);
}

//void CallVoidMethod(jobject obj, jmethodID methodID, va_list args)
JNI_HOOK_DEF(void, CallStaticVoidMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
    }
    return orig_CallStaticVoidMethodV(env, obj, jmethodId, args);
}

JNI_HOOK_DEF(jboolean, CallStaticBooleanMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)

    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jboolean ret = orig_CallStaticBooleanMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }

    return orig_CallStaticBooleanMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jbyte, CallStaticByteMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jbyte ret = orig_CallStaticByteMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }

    return orig_CallStaticByteMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jchar, CallStaticCharMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jchar ret = orig_CallStaticCharMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallStaticCharMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jshort, CallStaticShortMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jshort ret = orig_CallStaticShortMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallStaticShortMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jint, CallStaticIntMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jint ret = orig_CallStaticIntMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallStaticIntMethodV(env, obj, jmethodId, args);

}


JNI_HOOK_DEF(jlong, CallStaticLongMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jlong ret = orig_CallStaticLongMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallStaticLongMethodV(env, obj, jmethodId, args);
}


JNI_HOOK_DEF(jfloat, CallStaticFloatMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId, va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jfloat ret = orig_CallStaticFloatMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallStaticFloatMethodV(env, obj, jmethodId, args);

}


JNI_HOOK_DEF(jdouble, CallStaticDoubleMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jdouble ret = orig_CallStaticDoubleMethodV(env, obj, jmethodId, args);
        LOG(INFO) << "result : " << ret;
        return ret;
    }
    return orig_CallStaticDoubleMethodV(env, obj, jmethodId, args);
}



void Jnitrace::getJObjectInfo(JNIEnv *env, jobject obj) {
    if (obj == nullptr) {
        return;
    }
    LOGE("<<<<<------------------start--------------------->>>>>")

    Jnitrace::getJObjectInfo(env, obj, "invoke this object", true);


}


void
Jnitrace::getJObjectInfo(JNIEnv *env, jobject obj, char *message, bool isPrintClassinfo) {
    if (obj == nullptr) {
        return;
    }

    jclass objClass = env->GetObjectClass(obj);

    char* objClassInfo = nullptr ;

    if (isPrintClassinfo) {
        //print class info
        objClassInfo = getJObjectClassInfo(env, objClass);
    }

    jmethodID method_id_toString = env->GetMethodID(
            env->FindClass("java/lang/Object"), "toString",
                                                    "()Ljava/lang/String;");

    //打印当前obj的toString
    auto toString_ret = static_cast<jstring>(env->CallObjectMethod(obj, method_id_toString));

    const char *toString = env->GetStringUTFChars(toString_ret, NULL);

    if(objClassInfo!= nullptr ){
        LOGI("%s ->  %s  %s ", message, objClassInfo ,toString)
    } else{
        LOGI("%s -> %s ", message, toString)
    }

    //free
    env->ReleaseStringUTFChars(toString_ret, toString);
    env->DeleteLocalRef(toString_ret);

    //todo print Gson

}


char*  Jnitrace::getJObjectClassInfo(JNIEnv *env, jclass obj) {
    if (obj == nullptr) {
        return nullptr;
    }

    jclass clazz = env->FindClass("java/lang/Class");
    
    jmethodID method_id_getName = env->GetMethodID(clazz, "getName","()Ljava/lang/String;");

    //print class info
    auto getClassName_ret = 
            static_cast<jstring>(env->CallObjectMethod(obj, method_id_getName));

    const char *getClass = env->GetStringUTFChars(getClassName_ret, NULL);
    
    
    if (strstr(getClass, "[")) {
        env->ReleaseStringUTFChars(getClassName_ret, getClass);
        env->DeleteLocalRef(getClassName_ret);
        //数组打印toString没意义，直接返回
        return nullptr;
    }

    //free
    env->ReleaseStringUTFChars(getClassName_ret, getClass);
    env->DeleteLocalRef(getClassName_ret);

    return const_cast<char *>(getClass);
}

void Jnitrace::getArgsInfo(JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args) {
    if (obj == nullptr) {
        return;
    }
    if (jmethodId == nullptr) {
        return;
    }

    jclass pJclass = env->GetObjectClass(obj);
    //获取被调用的方法信息
    jobject invokeMethod = env->ToReflectedMethod(pJclass, jmethodId, false);
    if (invokeMethod == nullptr) {
        invokeMethod = env->ToReflectedMethod(pJclass, jmethodId, true);
    }

    //打印被调用方法信息
    Jnitrace::getJObjectInfo(env, invokeMethod, "invoke method", false);

    // 获取方法长度
    jmethodID method_id_getTypeCount = env->GetMethodID(
            env->FindClass("java/lang/reflect/Method"), "getParameterCount", "()I");

    jint size = env->CallIntMethod(invokeMethod, method_id_getTypeCount);

    if (size == 0) {
        //长度等于0直接return
        return;
    }

    jmethodID method_id_getTypes = env->GetMethodID(
            env->FindClass("java/lang/reflect/Method"), "getParameterTypes",
            "()[Ljava/lang/Class;");

    auto classArray = static_cast<jobjectArray>(env->CallObjectMethod(invokeMethod,
                                                                      method_id_getTypes));


    //输出参数信息
    for (int i = 0; i < size; i++) {
        auto obj = env->GetObjectArrayElement(classArray, i);
        if (obj == nullptr) {
            continue;
        }
        char *classInfo = getJObjectClassInfo(env, env->GetObjectClass(obj));

        if (strcmp(classInfo, "boolean") == 0 || strcmp(classInfo, "java.lang.Boolean") == 0) {
            jboolean arg = va_arg(args, jboolean);
            //LOGI("Boolean %hhu ", arg)
            LOGI("Boolean %hhu ", arg)
            continue;
        } else if (strcmp(classInfo, "byte") == 0 || strcmp(classInfo, "java.lang.Byte") == 0) {
            jbyte arg = va_arg(args, jbyte);
            LOGI("Byte %hhd ", arg)
            continue;
        } else if (strcmp(classInfo, "char") == 0 || strcmp(classInfo, "java.lang.Character") == 0) {
            jchar arg = va_arg(args, jchar);
            LOGI("Character %hu ", arg)
            continue;
        } else if (strcmp(classInfo, "short") == 0 || strcmp(classInfo, "java.lang.Short") == 0) {
            jshort arg = va_arg(args, jshort);
            LOGI("Short %d ", arg)
            continue;
        } else if (strcmp(classInfo, "int") == 0 || strcmp(classInfo, "java.lang.Integer") == 0) {
            jint arg = va_arg(args, jint);
            LOGI("Integer %d ", arg)
            continue;
        } else if (strcmp(classInfo, "float") == 0 || strcmp(classInfo, "java.lang.Float") == 0) {
            jfloat arg = va_arg(args, jfloat);
            LOGI("Float %f ", arg)
            continue;
        } else if (strcmp(classInfo, "double") == 0 || strcmp(classInfo, "java.lang.Double") == 0) {
            jdouble arg = va_arg(args, jdouble);
            LOGI("Double %f ", arg)
            continue;
        } else if (strcmp(classInfo, "long") == 0 || strcmp(classInfo, "java.lang.Long") == 0) {
            jlong arg = va_arg(args, jlong);
            LOGI("Long %ld ", arg)
            continue;
        } else {
            //object类型
            jobject arg = va_arg(args, jobject);
            char info[20];
            sprintf(info, "args %d ", i);
            Jnitrace::getJObjectInfo(env, arg, info, true);
        }
    }

    //end
    va_end(args);

}




//jstring NewStringUTF(const char* bytes)
HOOK_DEF(jstring, NewStringUTF, JNIEnv *env, const char *bytes) {
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        LOG(INFO) << "NewStringUTF : " << bytes;
        return orig_NewStringUTF(env, bytes);
    }
    return orig_NewStringUTF(env, bytes);
}

//const char* GetStringUTFChars(jstring string, jboolean* isCopy)
HOOK_DEF(const char*, GetStringUTFChars, JNIEnv *env, jstring string, jboolean *isCopy) {
    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, filterSoName.c_str())) {
        const char *chars = orig_GetStringUTFChars(env, string, isCopy);
        LOG(INFO) << "GetStringUTFChars : " << chars;
        return chars;
    }
    return orig_GetStringUTFChars(env, string, isCopy);
}


void Jnitrace::startjnitrace(JNIEnv *env, char *soname) {

    filterSoName = string(soname);


    HOOK_JNI(env, CallObjectMethodV)
    HOOK_JNI(env, CallBooleanMethodV)
    HOOK_JNI(env, CallByteMethodV)
    HOOK_JNI(env, CallCharMethodV)
    HOOK_JNI(env, CallShortMethodV)
    HOOK_JNI(env, CallIntMethodV)
    HOOK_JNI(env, CallLongMethodV)
    HOOK_JNI(env, CallFloatMethodV)
    HOOK_JNI(env, CallDoubleMethodV)
    HOOK_JNI(env, CallVoidMethodV)

    HOOK_JNI(env, CallStaticObjectMethodV)
    HOOK_JNI(env, CallStaticBooleanMethodV)
    HOOK_JNI(env, CallStaticByteMethodV)
    HOOK_JNI(env, CallStaticCharMethodV)
    HOOK_JNI(env, CallStaticShortMethodV)
    HOOK_JNI(env, CallStaticIntMethodV)
    HOOK_JNI(env, CallStaticLongMethodV)
    HOOK_JNI(env, CallStaticFloatMethodV)
    HOOK_JNI(env, CallStaticDoubleMethodV)
    HOOK_JNI(env, CallStaticVoidMethodV)

    //常用的字符串操作函数
    HOOK_JNI(env, NewStringUTF)
    HOOK_JNI(env, GetStringUTFChars)


    LOG(INFO) << ">>>>>>>>> Jnitrace hook sucess! ";

}




