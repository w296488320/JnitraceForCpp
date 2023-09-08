package com.example.jnitrace;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;


import com.example.Jnitrace.R;
import com.example.jnitrace.*;

import java.lang.reflect.Method;
import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {

    public native void startJnitrace(ArrayList<String> soname,String save_path);

    static {
        System.loadLibrary("jnitrace");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        //需要监听的SO关键字,监听多个SO添加即可
        ArrayList<String> list = new ArrayList<>();
        list.add("tracetest");
        startJnitrace(list,"/data/data/"+getPackageName()+"/111.txt");

        System.loadLibrary("tracetest");

    }


}