

#include "JnitraceForC.h"

namespace ZhenxiRunTime::JniTrace {
    static void
    getArgsInfo(JNIEnv *env, jobject obj, jmethodID jmethodId, va_list args, bool isStatic);

    static void getJObjectInfo(JNIEnv *env, jobject obj, const string &methodname);

    static char *getJObjectToString(JNIEnv *env, jobject obj);

    static void
    getJObjectInfoInternal(JNIEnv *env, jobject obj, string message, bool isPrintClassinfo,
                           const char *classinfo);

    static char *getJObjectClassInfo(JNIEnv *env, jobject obj);

    static std::ofstream *jnitraceOs;
    static std::list<string> filterSoList;
    static bool isSave = false;
    static string match_so_name = {};


#define HOOK_JNITRACE(env, func) \
    MSHookFunction(    \
      (void*)(env)->functions->func,\
      (void*)new_##func,\
      (void**)&orig_##func );    \


//#define GET_TOSTRING_METHOD(type) env->GetStaticMethodID(ArrayClazz,"toString", "("##type##")Ljava/lang/String;");

#define JNITRACE_HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)

#define JNI_HOOK_DEF(ret, func, ...)    \
  ret (*orig_##func)(JNIEnv *env, jobject obj, jmethodID jmethodId,...);  \
  ret new_##func(__VA_ARGS__) {         \
  //if(orig_##func == nullptr){LOG(INFO) <<"jnitrace hook error ,org sym == null "<<#func;}  \
  //LOG(INFO) << #func<<"("#__VA_ARGS__")" ;\

#define GET_JOBJECT_INFO(env, obj, methodname) getJObjectInfo(env,obj,methodname);

#define GET_METHOD_INFO_ARGS(env, obj, methodid, args, isStatic) getArgsInfo(env,obj,methodid,args,isStatic);


# define DL_INFO \
    Dl_info info; \
    dladdr((void *) __builtin_return_address(0), &info); \

# define IS_MATCH \
    for (const auto &soname: filterSoList) { \
        if (strstr(info.dli_fname, soname.c_str())) { \
            match_so_name = soname;     \


    std::mutex m_lock;

    static void write(const std::string &msg) {
        if(msg.c_str() == nullptr) {
            return;
        }
        if (isSave) {
            std::lock_guard<std::mutex> lock(m_lock);
            if (jnitraceOs != nullptr) {
                (*jnitraceOs) << msg;
            }
        }

        LOG(INFO) << "[" << match_so_name << "] " << msg.c_str();
    }

    /**
     * 第二个参数标识当前是否是分隔符
     */
    static inline void write(const std::string &msg, [[maybe_unused]] bool isApart) {
        if(msg.c_str() == nullptr) {
            return;
        }
        if (isSave) {
            std::lock_guard<std::mutex> lock(m_lock);
            if (jnitraceOs != nullptr) {
                (*jnitraceOs) << msg;
            }
        }
        LOG(INFO) << "[" << match_so_name << "] " << msg.c_str();
    }

    //jobject CallObjectMethod(JNIEnv*, jobject, jmethodID, va_list args);
    JNI_HOOK_DEF(jobject, CallObjectMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallObjectMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jobject ret = orig_CallObjectMethodV(env, obj, jmethodId, args);
                getJObjectInfoInternal(env, ret, "result object :", true, nullptr);
                return ret;
            }
        }
        return orig_CallObjectMethodV(env, obj, jmethodId, args);
    }

    //void CallVoidMethod(jobject obj, jmethodID methodID, va_list args)
    JNI_HOOK_DEF(void, CallVoidMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallVoidMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
            }
        }
        return orig_CallVoidMethodV(env, obj, jmethodId, args);
    }

    JNI_HOOK_DEF(jboolean, CallBooleanMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)

        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallBooleanMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jboolean ret = orig_CallBooleanMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "invoke method result Boolean : " << (ret == JNI_TRUE?"true":"false");
                //os << "invoke method result Boolean :  " << (ret == JNI_TRUE?"true":"false") << "\n";
                write(string("invoke method result Boolean : ").append(
                        (ret == JNI_TRUE ? "true" : "false")).append("\n"));
                return ret;
            }
        }

        return orig_CallBooleanMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jbyte, CallByteMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallByteMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jbyte ret = orig_CallByteMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Byte: " << ret;
                //os << "result Byte :  " << ret << "\n";
                write(string("result Byte : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }

        return orig_CallByteMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jchar, CallCharMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallCharMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jchar ret = orig_CallCharMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Char : " << ret;
                //os << "result Char :  " << ret << "\n";;
                write(string("result Char : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallCharMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jshort, CallShortMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallShortMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jshort ret = orig_CallShortMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Short: " << ret;
                //os << "result Short :  " << ret << "\n";;
                write(string("result Short : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallShortMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jint, CallIntMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallIntMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jint ret = orig_CallIntMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Int: " << ret;
                //os << "result Int :  " << ret << "\n";;
                write(string("result Int : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallIntMethodV(env, obj, jmethodId, args);

    }


    JNI_HOOK_DEF(jlong, CallLongMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallLongMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jlong ret = orig_CallLongMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Long: " << ret;
                //os << "result Long :  " << ret << "\n";;
                write(string("result Long : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallLongMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jfloat, CallFloatMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallFloatMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jfloat ret = orig_CallFloatMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Float: " << ret;
                //os << "result Float :  " << ret << "\n";;
                write(string("result Float : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallFloatMethodV(env, obj, jmethodId, args);

    }

    JNI_HOOK_DEF(jdouble, CallDoubleMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallDoubleMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                jdouble ret = orig_CallDoubleMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Double: " << ret;
                //os << "result Double :  " << ret << "\n";;
                write(string("result Double : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallDoubleMethodV(env, obj, jmethodId, args);
    }



    //------------------ static ------------------------------------------------------------

    JNI_HOOK_DEF(jobject, CallStaticObjectMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticObjectMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jobject ret = orig_CallStaticObjectMethodV(env, obj, jmethodId, args);
                getJObjectInfoInternal(env, ret, "result object :", true, nullptr);
                return ret;
            }
        }
        return orig_CallStaticObjectMethodV(env, obj, jmethodId, args);
    }

    //void CallVoidMethod(jobject obj, jmethodID methodID, va_list args)
    JNI_HOOK_DEF(void, CallStaticVoidMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticVoidMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
            }
        }
        return orig_CallStaticVoidMethodV(env, obj, jmethodId, args);
    }

    JNI_HOOK_DEF(jboolean, CallStaticBooleanMethodV, JNIEnv *env, jclass obj,
                 jmethodID jmethodId,
                 va_list args)

        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticBooleanMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jboolean ret = orig_CallStaticBooleanMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "invoke method result Boolean : " << (ret == JNI_TRUE?"true":"false");
                //os << "invoke method result Boolean :  " <<(ret == JNI_TRUE?"true":"false") << "\n";;
                write(string("result Boolean : ").append(
                        (ret == JNI_TRUE ? "true" : "false")).append(
                        "\n"));
                return ret;
            }
        }

        return orig_CallStaticBooleanMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jbyte, CallStaticByteMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticByteMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jbyte ret = orig_CallStaticByteMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Byte : " << ret;
                //            os << "result Byte :  " << ret << "\n";;
                write(string("result Byte : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }

        return orig_CallStaticByteMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jchar, CallStaticCharMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticCharMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jchar ret = orig_CallStaticCharMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Char : " << ret;
                //            os << "result Char :  " << ret << "\n";;
                write(string("result Char : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallStaticCharMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jshort, CallStaticShortMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticShortMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jshort ret = orig_CallStaticShortMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Short : " << ret;
                //os << "result Short :  " << ret << "\n";;
                write(string("result Short : ").append(to_string(ret)).append("\n"));

                return ret;
            }
        }
        return orig_CallStaticShortMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jint, CallStaticIntMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticIntMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jint ret = orig_CallStaticIntMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Int : " << ret;
                //os << "result Int :  " << ret << "\n";;
                write(string("result Int : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallStaticIntMethodV(env, obj, jmethodId, args);

    }


    JNI_HOOK_DEF(jlong, CallStaticLongMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticLongMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jlong ret = orig_CallStaticLongMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Long : " << ret;
                //os << "result Long :  " << ret << "\n";;
                write(string("result Long : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallStaticLongMethodV(env, obj, jmethodId, args);
    }


    JNI_HOOK_DEF(jfloat, CallStaticFloatMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticFloatMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jfloat ret = orig_CallStaticFloatMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Float : " << ret;
                //os << "result Float :  " << ret << "\n";;
                write(string("result Float : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallStaticFloatMethodV(env, obj, jmethodId, args);

    }


    JNI_HOOK_DEF(jdouble, CallStaticDoubleMethodV, JNIEnv *env, jclass obj, jmethodID jmethodId,
                 va_list args)
        DL_INFO
        IS_MATCH
                GET_JOBJECT_INFO(env, obj, "CallStaticDoubleMethodV")
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, true)
                jdouble ret = orig_CallStaticDoubleMethodV(env, obj, jmethodId, args);
                //LOG(INFO) << "result Double : " << ret;
                //os << "result Double :  " << ret << "\n";;
                write(string("result Double : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_CallStaticDoubleMethodV(env, obj, jmethodId, args);
    }


    void getJObjectInfo(JNIEnv *env, jobject obj, const string &methodname) {
        if (obj == nullptr) {
            return;
        }
        const string temptag =
                "<<<<<------------------" + methodname + " start--------------------->>>>>";
        //LOGE("%s", temptag.c_str())
        write(string("\n ").append(temptag).append("\n"), true);
        getJObjectInfoInternal(env, obj, "invoke this object", true, nullptr);
    }


    /**
     * 打印一个obj类型的信息
     *
     * @param env
     * @param obj 需要打印的obj
     * @param message 输出变量的信息
     * @param isPrintClassinfo 是否输出Class信息
     * @param classinfo classinfo信息
     */
    void getJObjectInfoInternal(JNIEnv *env,
                                jobject obj,
                                string message,
                                bool isPrintClassinfo,
                                const char *classInfo) {

        if (obj == nullptr) {
            return;
        }
        if (classInfo == nullptr) {
            jclass objClass = env->GetObjectClass(obj);
            classInfo = getJObjectClassInfo(env, objClass);
        }
        if (classInfo == nullptr) {
            return;
        }
        const char *ret;
        //数组类型需要特殊处理
        if (strstr(classInfo, "[")) {
            auto arg = (jobjectArray) obj;
            //数组类型参数
            jstring argJstr;
            //byte数组 特殊处理
            if (strcmp(classInfo, "[B") == 0) {
                jclass strclazz = env->FindClass("java/lang/String");
                jstring utf = env->NewStringUTF("UTF-8");
                jmethodID strInit = env->GetMethodID(strclazz, "<init>",
                                                     "([BLjava/lang/String;)V");
                argJstr = (jstring) (env->NewObject(strclazz, strInit, arg, utf));
                env->DeleteLocalRef(utf);
                env->DeleteLocalRef(strclazz);
            } else {
                //其他的则调用Arrays.toString 处理
                jclass ArrayClazz = env->FindClass("java/util/Arrays");
                jmethodID methodid = env->GetStaticMethodID(ArrayClazz, "toString",
                                                            "([Z)Ljava/lang/String;");
                argJstr = (jstring) (env->CallStaticObjectMethod(ArrayClazz, methodid, arg));
            }
            //上面的逻辑主要是为了处理argJstr的赋值
            ret = env->GetStringUTFChars(argJstr, nullptr);
        } else {
            ret = getJObjectToString(env, obj);
        }
        if (ret != nullptr) {
            if (isPrintClassinfo) {
                string basicString = message.append("  ").append(classInfo).append("  ").append(
                        ret).append("\n");
                write(basicString);
            } else {
                string basicString = message.append("  ").append(ret).append("\n");
                write(basicString);
            }
        }
    }


    void getArgsInfo(JNIEnv *env, jobject obj, jmethodID jmethodId,
                     va_list args, bool isStatic
    ) {

        if (obj == nullptr) {
            return;
        }
        if (jmethodId == nullptr) {
            return;
        }
        jclass pJclass = env->GetObjectClass(obj);

        //获取被调用的方法信息
        jobject invokeMethod = env->ToReflectedMethod(pJclass, jmethodId, isStatic);

        //打印被调用方法信息,只需要调用他的toString
        getJObjectInfoInternal(env, invokeMethod, "invoke method", false, nullptr);


        jclass objclass = env->GetObjectClass(invokeMethod);

        jmethodID parCountId = env->GetMethodID(objclass, "getParameterCount", "()I");
        // 获取方法长度
        jint size = env->CallIntMethod(invokeMethod, parCountId);

        if (size == 0) {
            //长度等于0直接return,不打印参数信息
            return;
        }

        jmethodID method_id_getTypes = env->GetMethodID(
                objclass,
                "getParameterTypes",
                "()[Ljava/lang/Class;");

        auto objectArray = (jobjectArray) (env->CallObjectMethod(invokeMethod,
                                                                 method_id_getTypes));


        //分别判断每个参数的类型
        for (int i = 0; i < size; i++) {
            auto argobj = env->GetObjectArrayElement(objectArray, i);
            if (argobj == nullptr) {
                continue;
            }
            char argsInfo[20];
            sprintf(argsInfo, "args %d ", i);

            //具体每个类型的class
            char *classInfo = getJObjectClassInfo(env, argobj);

            if (strcmp(classInfo, "boolean") == 0 ||
                strcmp(classInfo, "java.lang.Boolean") == 0) {
                jboolean arg = va_arg(args, jboolean);
                //LOGI("%s -> Boolean %hhu ", argsInfo, arg)
                //os<<argsInfo<<" Boolean "<< arg<<"\n";
                write(string(argsInfo).append(" Boolean : ").append(
                        (arg == JNI_TRUE ? "true" : "false")).append("\n"));
                continue;
            } else if (strcmp(classInfo, "byte") == 0 ||
                       strcmp(classInfo, "java.lang.Byte") == 0) {
                jbyte arg = va_arg(args, jbyte);
                //LOGI("%s ->Byte %hhd ", argsInfo, arg)
                //os<<argsInfo<<" Byte "<< arg<<"\n";
                write(string(argsInfo).append(" Byte : ").append(to_string(arg)).append("\n"));
                continue;
            } else if (strcmp(classInfo, "char") == 0 ||
                       strcmp(classInfo, "java.lang.Character") == 0) {
                jchar arg = va_arg(args, jchar);
                //LOGI("%s ->char %hu ", argsInfo, arg)
                //os<<argsInfo<<" Character "<< arg<<"\n";
                write(string(argsInfo).append(" char : ").append(to_string(arg)).append("\n"));
                continue;
            } else if (strcmp(classInfo, "short") == 0 ||
                       strcmp(classInfo, "java.lang.Short") == 0) {
                jshort arg = va_arg(args, jshort);
                //LOGI("%s ->Short %d ", argsInfo, arg)
                // os<<argsInfo<<" Short "<< arg<<"\n";
                write(string(argsInfo).append(" Short : ").append(to_string(arg)).append("\n"));
                continue;
            } else if (strcmp(classInfo, "int") == 0 ||
                       strcmp(classInfo, "java.lang.Integer") == 0) {
                jint arg = va_arg(args, jint);
                //LOGI("%s ->Integer %d ", argsInfo, arg)
                // os<<argsInfo<<" Integer "<< arg<<"\n";
                write(string(argsInfo).append(" Integer : ").append(to_string(arg)).append(
                        "\n"));
                continue;
            } else if (strcmp(classInfo, "float") == 0 ||
                       strcmp(classInfo, "java.lang.Float") == 0) {
                jfloat arg = va_arg(args, jfloat);
                //LOGI("%s ->Float %f ", argsInfo, arg)
                //os<<argsInfo<<" Float "<< arg<<"\n";
                write(string(argsInfo).append(" Float : ").append(to_string(arg)).append("\n"));
                continue;
            } else if (strcmp(classInfo, "double") == 0 ||
                       strcmp(classInfo, "java.lang.Double") == 0) {
                jdouble arg = va_arg(args, jdouble);
                //LOGI("%s ->Double %f ", argsInfo, arg)
                //os<<argsInfo<<" Double "<< arg<<"\n";
                write(string(argsInfo).append(" Double : ").append(to_string(arg)).append(
                        "\n"));

                continue;
            } else if (strcmp(classInfo, "long") == 0 ||
                       strcmp(classInfo, "java.lang.Long") == 0) {
                jlong arg = va_arg(args, jlong);
                //LOGI("%s ->Long %ld ", argsInfo, arg)
                //os<<argsInfo<<" Long "<< arg<<"\n";
                write(string(argsInfo).append(" Long : ").append(to_string(arg)).append("\n"));
                continue;
            } else if (strstr(classInfo, "[")) {
                jobjectArray arg = va_arg(args, jobjectArray);
                //数组类型参数
                jstring argJstr;
                //byte数组 特殊处理
                if (strcmp(classInfo, "[B") == 0) {
                    jclass strclazz = env->FindClass("java/lang/String");
                    jstring utf = env->NewStringUTF("UTF-8");
                    jmethodID strInit = env->GetMethodID(strclazz, "<init>",
                                                         "([BLjava/lang/String;)V");
                    argJstr = (jstring) (env->NewObject(strclazz, strInit, arg, utf));
                    env->DeleteLocalRef(utf);
                    env->DeleteLocalRef(strclazz);

                } else {
                    jclass ArrayClazz = env->FindClass("java/util/Arrays");
                    jmethodID methodid = env->GetStaticMethodID(ArrayClazz, "toString",
                                                                "([Z)Ljava/lang/String;");
                    argJstr = (jstring) (env->CallStaticObjectMethod(ArrayClazz, methodid,
                                                                     arg));
                }

                //上面的逻辑主要是为了处理argJstr的赋值
                const char *ret = env->GetStringUTFChars(argJstr, nullptr);
                if (ret != nullptr) {
                    write(string(argsInfo).append(" class info ->  ")
                                  .append(classInfo).append(" ").append((ret)).append("\n"));
                }
                continue;
            } else {
                jobject arg = va_arg(args, jobject);
                getJObjectInfoInternal(env, arg, argsInfo, true, classInfo);
            }
        }
        //end
        va_end(args);

    }

    /*
     * get obj class info
     */
    char *getJObjectClassInfo(JNIEnv *env, jobject obj) {
        if (obj == nullptr) {
            return nullptr;
        }

        jclass clazz = env->FindClass("java/lang/Class");

        jmethodID method_id_getName = env->GetMethodID(clazz, "getName",
                                                       "()Ljava/lang/String;");

        //print class info
        auto getClassName_ret = (jstring) (env->CallObjectMethod(obj, method_id_getName));


        const char *getClass = env->GetStringUTFChars(getClassName_ret, nullptr);

        //free
        env->ReleaseStringUTFChars(getClassName_ret, getClass);
        env->DeleteLocalRef(getClassName_ret);

        return const_cast<char *>(getClass);
    }

    /*
     * get obj to string
     */
    char *getJObjectToString(JNIEnv *env, jobject obj) {
        jmethodID method_id_toString =
                env->GetMethodID(env->FindClass("java/lang/Object"), "toString",
                                 "()Ljava/lang/String;");
        return const_cast<char *>(env->GetStringUTFChars(
                (jstring) (env->CallObjectMethod(obj, method_id_toString)), nullptr));
    }
    //jstring NewStringUTF(const char* bytes)
    JNITRACE_HOOK_DEF(jstring, NewStringUTF, JNIEnv *env, const char *bytes) {
        DL_INFO
        IS_MATCH
                write(string("NewStringUTF : ").append(bytes== nullptr?"":bytes).append("\n"));
                return orig_NewStringUTF(env, bytes);
            }
        }
        return orig_NewStringUTF(env, bytes);
    }

    //const char* GetStringUTFChars(jstring string, jboolean* isCopy)
    JNITRACE_HOOK_DEF(const char*, GetStringUTFChars, JNIEnv *env, jstring argstring,
                      [[maybe_unused]] jboolean * isCopy) {
        DL_INFO
        IS_MATCH
                if(argstring == nullptr) {
                    return nullptr;
                }
                const char *chars = orig_GetStringUTFChars(env, argstring, isCopy);
                write(string("GetStringUTFChars : ").append(chars== nullptr?"":chars).append("\n"));
                return chars;
            }
        }
        return orig_GetStringUTFChars(env, argstring, isCopy);
    }

    //jclass FindClass(const char* name)
    JNITRACE_HOOK_DEF(jclass, FindClass, JNIEnv *env, const char *name) {
        DL_INFO
        IS_MATCH
                //LOG(INFO) << "find class : " << name;
                //os << "find class : " << name << "\n";
                write(string("find class : ").append(name== nullptr?"":name).append("\n"));
                return orig_FindClass(env, name);
            }
        }
        return orig_FindClass(env, name);
    }



    //jobject     (*NewObjectV)(JNIEnv*, jclass, jmethodID, va_list);
    JNITRACE_HOOK_DEF(jobject, NewObjectV, JNIEnv *env, jclass clazz, jmethodID jmethodId,
                      va_list args) {
        DL_INFO
        IS_MATCH
                //当method id 错误时候可能null
                jobject obj = orig_NewObjectV(env, clazz, jmethodId, args);
                //LOGE("<<<<<------------------NewObjectV start  --------------------->>>>>")
                //os <<"\n" <<"<<<<<------------------NewObjectV start  --------------------->>>>>" << "\n";;
                write(string(
                        "\n <<<<<------------------NewObjectV start  --------------------->>>>>").append(
                        "\n"), true);
                //打印构造方法参数信息
                GET_METHOD_INFO_ARGS(env, obj, jmethodId, args, false)
                return obj;
            }
        }
        return orig_NewObjectV(env, clazz, jmethodId, args);
    }

    //jobject     (*ToReflectedMethod)(JNIEnv*, jclass, jmethodID, jboolean);
    JNITRACE_HOOK_DEF(jobject, ToReflectedMethod, JNIEnv *env, jclass clazz,
                      jmethodID jmethodId,
                      jboolean isStatic) {
        DL_INFO
        IS_MATCH
                jobject obj = orig_ToReflectedMethod(env, clazz, jmethodId, isStatic);
                if (obj != nullptr) {
                    getJObjectInfoInternal(env, obj, "ToReflectedMethod result ", false,
                                           nullptr);
                }
                return obj;
            }
        }
        return orig_ToReflectedMethod(env, clazz, jmethodId, isStatic);
    }

    //jmethodID FromReflectedMethod(jobject method)
    JNITRACE_HOOK_DEF(jobject, FromReflectedMethod, JNIEnv *env, jobject method) {
        DL_INFO
        IS_MATCH
                getJObjectInfoInternal(env, method, "FromReflectedMethod arg ", false,
                                       nullptr);
                return orig_FromReflectedMethod(env, method);
            }
        }
        return orig_FromReflectedMethod(env, method);
    }


    //jfieldID  (*GetFieldID)(JNIEnv*, jclass, const char*, const char*);
    JNITRACE_HOOK_DEF(jfieldID, GetFieldID, JNIEnv *env, jclass clazz, const char *name,
                      const char *sig) {
        DL_INFO
        IS_MATCH
                jmethodID method_id_toString = env->GetMethodID(
                        env->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;");
                auto toString_ret = (jstring) (env->CallObjectMethod(clazz,
                                                                     method_id_toString));
                const char *toString = env->GetStringUTFChars(toString_ret, nullptr);
                //LOG(INFO) << "get field info  :  " << toString << "  " << name << "  " << sig;
                //os << "get field info  :  " << toString << "  " << name << "  " << sig << "\n";
                write(string("get field info  : ").append(toString== nullptr?"":toString)
                              .append(" name-> ").append(name== nullptr?"":name)
                                    .append(" sign -> ").append(sig== nullptr?"":sig).append("\n"));
                return orig_GetFieldID(env, clazz, name, sig);
            }
        }
        return orig_GetFieldID(env, clazz, name, sig);
    }

    //jfieldID    (*GetStaticFieldID)(JNIEnv*, jclass, const char*,const char*);
    JNITRACE_HOOK_DEF(jfieldID, GetStaticFieldID, JNIEnv *env, jclass clazz, const char *name,
                      const char *sig) {
        DL_INFO
        IS_MATCH
                jmethodID method_id_toString = env->GetMethodID(
                        env->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;");
                auto toString_ret = (jstring) (env->CallObjectMethod(clazz,
                                                                     method_id_toString));
                const char *toString = env->GetStringUTFChars(toString_ret, nullptr);
                //LOG(INFO) << "get static field info  :  " << toString << "  " << name << "  " << sig;
                //os << "get static field info  :  " << toString << "  " << name << "  " << sig << "\n";
                write(string("get static field info  : ").append(toString == nullptr?"":toString)
                              .append(" ").append(name).append(" ").append(sig).append("\n"));
                return orig_GetStaticFieldID(env, clazz, name, sig);
            }
        }
        return orig_GetStaticFieldID(env, clazz, name, sig);
    }

    //--------------------------getField----------------------------
    //jobject     (*GetObjectField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jobject, GetObjectField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetObjectField")
                jobject ret = orig_GetObjectField(env, argObj, jfieldId);
                if (ret != nullptr) {
                    getJObjectInfoInternal(env, ret, "GetObjectField result object :", true,
                                           nullptr);
                }
                return ret;
            }
        }
        return orig_GetObjectField(env, argObj, jfieldId);
    }

    //jboolean    (*GetBooleanField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jboolean, GetBooleanField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetBooleanField")
                jboolean ret = orig_GetBooleanField(env, argObj, jfieldId);
                //LOG(INFO) << "GetBooleanField result jboolean : " << (ret == JNI_TRUE?"true":"false");
                //os << "GetBooleanField result jboolean :  " << (ret == JNI_TRUE?"true":"false") << "\n";;
                write(string("GetBooleanField result jboolean : ").append(
                        (ret == JNI_TRUE ? "true" : "false")).append("\n"));
                return ret;
            }
        }
        return orig_GetBooleanField(env, argObj, jfieldId);
    }
    //jbyte       (*GetByteField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jbyte, GetByteField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetByteField")
                jbyte ret = orig_GetByteField(env, argObj, jfieldId);
                //LOG(INFO) << "GetByteField result jbyte : " << ret;
                //            os << "GetByteField result jbyte :  " << ret << "\n";;
                write(string("GetByteField result jbyte : ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetByteField(env, argObj, jfieldId);
    }
    //jchar       (*GetCharField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jchar, GetCharField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetCharField")
                jchar ret = orig_GetCharField(env, argObj, jfieldId);
                //LOG(INFO) << "GetCharField result jbyte : " << ret;
                //os << "GetCharField result jbyte :  " << ret << "\n";;
                write(string("GetCharField result jbyte : ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetCharField(env, argObj, jfieldId);
    }
    //jshort      (*GetShortField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jshort, GetShortField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetShortField")
                jshort ret = orig_GetShortField(env, argObj, jfieldId);
                //LOG(INFO) << "GetShortField result jshort : " << ret;
                //os << "GetShortField result jshort :  " << ret << "\n";;
                write(string("GetShortField result jshort : ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetShortField(env, argObj, jfieldId);
    }
    //jint        (*GetIntField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jint, GetIntField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetIntField")
                jint ret = orig_GetIntField(env, argObj, jfieldId);
                //LOG(INFO) << "GetIntField result jint : " << ret;
                //os << "GetIntField result jint :  " << ret << "\n";;
                write(string("GetIntField result jint : ").append(to_string(ret)).append("\n"));
                return ret;
            }
        }
        return orig_GetIntField(env, argObj, jfieldId);
    }
    //jlong       (*GetLongField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jlong, GetLongField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetLongField")
                jlong ret = orig_GetLongField(env, argObj, jfieldId);
                //LOG(INFO) << "GetLongField result jlong : " << ret;
                //os << "GetLongField result jlong :  " << ret << "\n";;
                write(string("GetLongField result jlong : ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetLongField(env, argObj, jfieldId);
    }
    //jfloat      (*GetFloatField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jfloat, GetFloatField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetFloatField")
                jfloat ret = orig_GetFloatField(env, argObj, jfieldId);
                //LOG(INFO) << "GetFloatField result jfloat : " << ret;
                //os << "GetFloatField result jfloat :  " << ret << "\n";;
                write(string("GetFloatField result jfloat : ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetFloatField(env, argObj, jfieldId);
    }
    //jdouble     (*GetDoubleField)(JNIEnv*, jobject, jfieldID);
    JNITRACE_HOOK_DEF(jdouble, GetDoubleField, JNIEnv *env, [[maybe_unused]] jobject argObj,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                //GET_JOBJECT_INFO(env, argObj,"GetDoubleField")
                jdouble ret = orig_GetDoubleField(env, argObj, jfieldId);
                //LOG(INFO) << "GetDoubleField result jdouble : " << ret;
                //os << "GetDoubleField result jdouble :  " << ret << "\n";;
                write(string("GetDoubleField result jdouble : ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetDoubleField(env, argObj, jfieldId);
    }
    //--------------------------getStaticField----------------------------
    //jobject     (*GetStaticObjectField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jobject, GetStaticObjectField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jobject ret = orig_GetStaticObjectField(env, clazz, jfieldId);
                if (ret != nullptr) {
                    getJObjectInfoInternal(env, ret, "GetStaticObjectField result object :",
                                           true,
                                           nullptr);
                }
                return ret;
            }
        }
        return orig_GetStaticObjectField(env, clazz, jfieldId);
    }

    //jboolean    (*GetStaticBooleanField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jboolean, GetStaticBooleanField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jboolean ret = orig_GetStaticBooleanField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticBooleanField result jboolean : " << (ret == JNI_TRUE?"true":"false");
                //os << "GetStaticBooleanField result jboolean :  " << (ret == JNI_TRUE?"true":"false") << "\n";;
                write(string("GetStaticBooleanField result jboolean : ").append(
                        (ret == JNI_TRUE ? "true" : "false")).append("\n"));
                return ret;
            }
        }
        return orig_GetStaticBooleanField(env, clazz, jfieldId);
    }
    //jbyte       (*GetStaticByteField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jbyte, GetStaticByteField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jbyte ret = orig_GetStaticByteField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticByteField result jbyte : " << ret;
                //            os << "GetStaticByteField result jbyte :  " << ret << "\n";;
                write(string("GetStaticByteField result jbyte : ").append(
                        to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticByteField(env, clazz, jfieldId);
    }
    //jchar       (*GetStaticCharField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jchar, GetStaticCharField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jchar ret = orig_GetStaticCharField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticCharField result jchar : " << ret;
                //os << "GetStaticCharField result jchar :  " << ret << "\n";;
                write(string("GetStaticCharField result jchar : ").append(
                        to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticCharField(env, clazz, jfieldId);
    }
    //jshort      (*GetStaticShortField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jshort, GetStaticShortField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jshort ret = orig_GetStaticShortField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticShortField result jshort : " << ret;
                //os << "GetStaticShortField result jshort :  " << ret << "\n";;
                write(string("GetStaticShortField result jshort : ").append(
                        to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticShortField(env, clazz, jfieldId);
    }
    //jint        (*GetStaticIntField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jint, GetStaticIntField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jint ret = orig_GetStaticIntField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticIntField result jint : " << ret;
                //            os << "GetStaticIntField result jint :  " << ret << "\n";;
                write(string("GetStaticIntField result jint :  ").append(to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticIntField(env, clazz, jfieldId);
    }
    //jlong       (*GetStaticLongField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jlong, GetStaticLongField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jlong ret = orig_GetStaticLongField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticLongField result jlong : " << ret;
                //os << "GetStaticLongField result jlong :  " << ret << "\n";;
                write(string("GetStaticLongField result jlong :  ").append(
                        to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticLongField(env, clazz, jfieldId);
    }
    //jfloat      (*GetStaticFloatField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jfloat, GetStaticFloatField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jfloat ret = orig_GetStaticFloatField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticFloatField result jfloat : " << ret;
                //os << "GetStaticFloatField result jfloat :  " << ret << "\n";;
                write(string("GetStaticFloatField result jfloat :  ").append(
                        to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticFloatField(env, clazz, jfieldId);
    }
    //jdouble     (*GetStaticDoubleField)(JNIEnv*, jclass, jfieldID);
    JNITRACE_HOOK_DEF(jdouble, GetStaticDoubleField, JNIEnv *env, jclass clazz,
                      [[maybe_unused]] jfieldID jfieldId) {
        DL_INFO
        IS_MATCH
                jdouble ret = orig_GetStaticDoubleField(env, clazz, jfieldId);
                //LOG(INFO) << "GetStaticDoubleField result jdouble : " << ret;
                //os << "GetStaticDoubleField result jdouble :  " << ret << "\n";;
                write(string("GetStaticDoubleField result jdouble :  ").append(
                        to_string(ret)).append(
                        "\n"));
                return ret;
            }
        }
        return orig_GetStaticDoubleField(env, clazz, jfieldId);
    }
}

using namespace ZhenxiRunTime::JniTrace;

void Jnitrace::startjnitrace(JNIEnv *env, const std::list<string> &filter_list, char *path) {
    filterSoList = filter_list;
    if (path != nullptr) {
        //init save file
        isSave = true;
        //打开文件时，但是文件之前的内容都会被清空。可能存在多进程问题导致文件被清空
        //os.open(path,ios::trunc);
        //以追加方式打开文件，所有写文件的数据都是追加在文件末尾。
        jnitraceOs = new ofstream();
        jnitraceOs->open(path, ios::app);
        if (!jnitraceOs->is_open()) {
            LOG(INFO) << "jniTrace open file error  " << path;
            LOG(INFO) << "jniTrace open file error  " << path;
            LOG(INFO) << "jniTrace open file error  " << path;
            return;
        }
    }

    HOOK_JNITRACE(env, CallObjectMethodV)
    HOOK_JNITRACE(env, CallBooleanMethodV)
    HOOK_JNITRACE(env, CallByteMethodV)
    HOOK_JNITRACE(env, CallCharMethodV)
    HOOK_JNITRACE(env, CallShortMethodV)
    HOOK_JNITRACE(env, CallIntMethodV)
    HOOK_JNITRACE(env, CallLongMethodV)
    HOOK_JNITRACE(env, CallFloatMethodV)
    HOOK_JNITRACE(env, CallDoubleMethodV)
    HOOK_JNITRACE(env, CallVoidMethodV)

    HOOK_JNITRACE(env, CallStaticObjectMethodV)
    HOOK_JNITRACE(env, CallStaticBooleanMethodV)
    HOOK_JNITRACE(env, CallStaticByteMethodV)
    HOOK_JNITRACE(env, CallStaticCharMethodV)
    HOOK_JNITRACE(env, CallStaticShortMethodV)
    HOOK_JNITRACE(env, CallStaticIntMethodV)
    HOOK_JNITRACE(env, CallStaticLongMethodV)
    HOOK_JNITRACE(env, CallStaticFloatMethodV)
    HOOK_JNITRACE(env, CallStaticDoubleMethodV)
    HOOK_JNITRACE(env, CallStaticVoidMethodV)


    HOOK_JNITRACE(env, GetObjectField)
    HOOK_JNITRACE(env, GetBooleanField)
    HOOK_JNITRACE(env, GetByteField)
    HOOK_JNITRACE(env, GetCharField)
    HOOK_JNITRACE(env, GetShortField)
    HOOK_JNITRACE(env, GetIntField)
    HOOK_JNITRACE(env, GetLongField)
    HOOK_JNITRACE(env, GetFloatField)
    HOOK_JNITRACE(env, GetDoubleField)

    HOOK_JNITRACE(env, GetStaticObjectField)
    HOOK_JNITRACE(env, GetStaticBooleanField)
    HOOK_JNITRACE(env, GetStaticByteField)
    HOOK_JNITRACE(env, GetStaticCharField)
    HOOK_JNITRACE(env, GetStaticShortField)
    HOOK_JNITRACE(env, GetStaticIntField)
    HOOK_JNITRACE(env, GetStaticLongField)
    HOOK_JNITRACE(env, GetStaticFloatField)
    HOOK_JNITRACE(env, GetStaticDoubleField)

    //常用的字符串操作函数
    HOOK_JNITRACE(env, NewStringUTF)
    HOOK_JNITRACE(env, GetStringUTFChars)

    HOOK_JNITRACE(env, FindClass)
    HOOK_JNITRACE(env, ToReflectedMethod)
    HOOK_JNITRACE(env, FromReflectedMethod)


    HOOK_JNITRACE(env, GetFieldID)
    HOOK_JNITRACE(env, GetStaticFieldID)

    HOOK_JNITRACE(env, NewObjectV)

    LOG(INFO) << ">>>>>>>>> Jnitrace hook sucess! ";

}

[[maybe_unused]] void Jnitrace::stopjnitrace() {
    filterSoList.clear();
    if (jnitraceOs != nullptr) {
        if (jnitraceOs->is_open()) {
            jnitraceOs->close();
        }
    }
    isSave = false;
}




