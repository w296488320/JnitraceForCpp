package com.example.jnitrace;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;


import com.example.Jnitrace.R;
import com.example.jnitrace.*;

import java.lang.reflect.Method;
import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {

    /**
     * @param soname 需要过滤的soname,支持多个so同时过滤
     * @param savepath 保存的路径,当为null时候则通过日志进行打印
     */
    public native void startJnitrace(ArrayList<String> soname, String savepath);

    static {
        System.loadLibrary("jnitrace");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        ArrayList<String> arrayList = new ArrayList<>();
        arrayList.add("tracetest");
        startJnitrace(arrayList,null);

        System.loadLibrary("tracetest");

    }


}