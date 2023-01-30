package com.tool.jniTrace;

import java.util.ArrayList;

public class JniTrace {

    /**
     * @param soname 需要过滤的soname,支持多个so同时过滤
     * @param savepath 保存的路径,当为null时候则通过日志进行打印
     */
    public static native void startJnitrace(ArrayList<String> soname, String savepath);

    static {
        System.loadLibrary("jnitrace");
    }
}