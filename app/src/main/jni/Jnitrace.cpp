

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
        Jnitrace::getJObjectInfoInternal(env, ret, "result object :", true, nullptr);
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
        Jnitrace::getJObjectInfoInternal(env, ret, "result object :", true, nullptr);
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


JNI_HOOK_DEF(jfloat, CallStaticFloatMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
             va_list args)
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

    Jnitrace::getJObjectInfoInternal(env, obj, "invoke this object", true, nullptr);


}


void Jnitrace::getJObjectInfoInternal(JNIEnv *env,
                                      jobject obj,
                                      char *message,
                                      bool isPrintClassinfo,
                                      char *classinfo) {

    if (obj == nullptr) {
        return;
    }
    jclass objClass = env->GetObjectClass(obj);
    if (classinfo == nullptr) {
        classinfo = getJObjectClassInfo(env, objClass);
    }

    jmethodID method_id_toString = env->GetMethodID(
            env->FindClass("java/lang/Object"),
            "toString",
            "()Ljava/lang/String;");

    //打印当前obj的toString
    auto toString_ret = static_cast<jstring>(env->CallObjectMethod(obj, method_id_toString));

    const char *toString = env->GetStringUTFChars(toString_ret, NULL);

    if (isPrintClassinfo) {
        LOGI("%s ->  %s  %s ", message, classinfo, toString)
    } else {
        LOGI("%s -> %s ", message, toString)
    }

    //free
    env->ReleaseStringUTFChars(toString_ret, toString);
    env->DeleteLocalRef(toString_ret);

    //todo print Gson

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
    Jnitrace::getJObjectInfoInternal(env, invokeMethod, "invoke method", false, nullptr);

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

    auto objectArray = static_cast<jobjectArray>(env->CallObjectMethod(invokeMethod,
                                                                       method_id_getTypes));



    //分别判断每个参数的类型
    for (int i = 0; i < size; i++) {
        auto obj = env->GetObjectArrayElement(objectArray, i);
        if (obj == nullptr) {
            continue;
        }
        char argsInfo[20];
        sprintf(argsInfo, "args %d ", i);

        //具体每个类型的class
        char *classInfo = getJObjectClassInfo(env, obj);
        //LOGI("->   %s   ", classInfo)

        if (strcmp(classInfo, "boolean") == 0 || strcmp(classInfo, "java.lang.Boolean") == 0) {
            jboolean arg = va_arg(args, jboolean);
            LOGI("%s -> Boolean %hhu ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "byte") == 0 || strcmp(classInfo, "java.lang.Byte") == 0) {
            jbyte arg = va_arg(args, jbyte);
            LOGI("%s ->Byte %hhd ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "char") == 0 ||
                   strcmp(classInfo, "java.lang.Character") == 0) {
            jchar arg = va_arg(args, jchar);
            LOGI("%s ->Character %hu ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "short") == 0 || strcmp(classInfo, "java.lang.Short") == 0) {
            jshort arg = va_arg(args, jshort);
            LOGI("%s ->Short %d ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "int") == 0 || strcmp(classInfo, "java.lang.Integer") == 0) {
            jint arg = va_arg(args, jint);
            LOGI("%s ->Integer %d ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "float") == 0 || strcmp(classInfo, "java.lang.Float") == 0) {
            jfloat arg = va_arg(args, jfloat);
            LOGI("%s ->Float %f ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "double") == 0 || strcmp(classInfo, "java.lang.Double") == 0) {
            jdouble arg = va_arg(args, jdouble);
            LOGI("%s ->Double %f ", argsInfo, arg)
            continue;
        } else if (strcmp(classInfo, "long") == 0 || strcmp(classInfo, "java.lang.Long") == 0) {
            jlong arg = va_arg(args, jlong);
            LOGI("%s ->Long %ld ", argsInfo, arg)
            continue;
        } else if (strstr(classInfo, "[")) {
            jobjectArray arg = va_arg(args, jobjectArray);
            //数组类型参数
            jclass ArrayClazz = env->FindClass("java/util/Arrays");
            jmethodID methodid = nullptr;
            jstring argJstr = nullptr;
            const char *ret = nullptr;
            if (strcmp(classInfo, "[Z") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([Z)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[C") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([C)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[D") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([D)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[F") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([F)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[I") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([I)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[J") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([J)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[S") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString", "([S)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[Ljava/lang/Object;") == 0) {
                methodid = env->GetStaticMethodID(ArrayClazz, "toString","([Ljava/lang/Object;)Ljava/lang/String;");
            } else if (strcmp(classInfo, "[B") == 0) {
                //字节数组,特殊处理,打印字符串数组的U8编码
                jclass strclazz = env->FindClass("java/lang/String");
                jmethodID strInit = env->GetMethodID(strclazz, "<init>", "([BLjava/lang/String;)V");
                if(strInit == nullptr){
                    LOGI("get string <init> error  ");
                    continue;
                }
                jstring utf = env->NewStringUTF("UTF-8");
                argJstr = static_cast<jstring>(env->NewObject(strclazz, strInit, arg,utf));
                //如果这个native方法不会切换到Java环境可能导致不会被释放
                env->DeleteLocalRef(utf);
            }
            if (argJstr == nullptr) {
                argJstr = static_cast<jstring>(env->CallStaticObjectMethod(ArrayClazz, methodid,arg));
            }
            ret = env->GetStringUTFChars(argJstr, nullptr);
            if (argJstr != nullptr) {
                LOGI("%s ->  %s  -> %s ", argsInfo, classInfo, ret);
                env->ReleaseStringUTFChars(argJstr, ret);
            }
            continue;
        } else {
            //object类型
            jobject arg = va_arg(args, jobject);
            Jnitrace::getJObjectInfoInternal(env, arg, argsInfo, true, classInfo);
        }

    }

    //end
    va_end(args);

}


char *Jnitrace::getJObjectClassInfo(JNIEnv *env, jobject obj) {
    if (obj == nullptr) {
        return "";
    }

    jclass clazz = env->FindClass("java/lang/Class");

    jmethodID method_id_getName = env->GetMethodID(clazz, "getName", "()Ljava/lang/String;");

    //print class info
    auto getClassName_ret = static_cast<jstring>(env->CallObjectMethod(obj, method_id_getName));


    const char *getClass = env->GetStringUTFChars(getClassName_ret, NULL);

    //free
    env->ReleaseStringUTFChars(getClassName_ret, getClass);
    env->DeleteLocalRef(getClassName_ret);

    return const_cast<char *>(getClass);
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




