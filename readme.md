## JnitraceForCpp

### 前言：

很多时候我们在分析一个App的时候，需要做黑盒调用对方里面的方法或者监听

对方So都调用了哪些方法，想知道传递的内容是什么。并且把参数打印出来。

这个时候就希望用到jnitrace ，Git有Frida实现的版本（@see https://github.com/chame1eon/jnitrace）

因为自己平时也经常需要分析一些So的调用逻辑，包括一些文件监听的功能，所以经常需要和沙盒打交道。

于是就有了JnitraceForCpp这个框架。



### 主要用途：

>1，监听指定so调用哪些Java方法&打印参数信息
>
>2，分析java2c , java2c的调用方法打印

### 功能实现：

hook env里面的各种callmethod , 正常的 **callmethod** 最终底层会走到**callmethodV** 

hook过程如下

```
    //hook jni
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

    HOOK_JNI(env, NewStringUTF)
    HOOK_JNI(env, GetStringUTFChars)
```

在回调里面判断是否是指定So调用了当前的方法

如果是的话打印参数信息和被调用者本身的object的信息，目前只打印了toString 

(这块可以优化一下打印对象的Json串，更清晰的打印变量信息)



在回调里面判断是哪个So调用了当前callmethod方法，如果复合打印参数信息即可

```
char* getFilterName() {
    //todo add filter name
    return "libtemp.so";
}

JNI_HOOK_DEF(jobject, CallObjectMethodV, JNIEnv *env, jobject obj, jmethodID jmethodId,
             va_list args)

    Dl_info info;
    dladdr((void *) __builtin_return_address(0), &info);
    if (strstr(info.dli_fname, getFilterName())) {
        GET_JOBJECT_INFO(env, obj)
        GET_METHOD_INFO_ARGS(env, obj, jmethodId, args)
        jobject ret = orig_CallObjectMethodV(env, obj, jmethodId, args);
        jnitrace::getJObjectInfo(env, ret, "result object :", true);
        return ret;
    }
    return orig_CallObjectMethodV(env, obj, jmethodId, args);
}

...
```

我们尝试在目标So里面调用activity的toString方法

```
public class MainActivity extends Activity {
	@Override
    public String toString() {
        return "this is main activity ";
    }
}
```



```
//cjnitrace  初始化成功回调
2021-12-21 19:28:59.943 22328-22328/com.example.cjnitrace E/jnitrace: >>>>>>>>> jnitrace hook sucess! 
//发现有人调用callmethod方法
2021-12-21 19:28:59.944 22328-22328/com.example.cjnitrace E/jnitrace: <<<<<------------------start--------------------->>>>>
//打印被调用者的ac信息
2021-12-21 19:28:59.944 22328-22328/com.example.cjnitrace I/jnitrace: invoke this object class info ->  com.example.cjnitrace.MainActivity   toString -> this is main activity  
//被调用方法信息
2021-12-21 19:28:59.944 22328-22328/com.example.cjnitrace I/jnitrace: invoke method toString public java.lang.String com.example.cjnitrace.MainActivity.toString() 
//返回结果信息
2021-12-21 19:28:59.944 22328-22328/com.example.cjnitrace I/jnitrace: result object : class info ->  java.lang.String   toString-> this is main activity  
```



