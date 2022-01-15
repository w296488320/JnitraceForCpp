package com.example.jnitrace;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;


import com.example.Jnitrace.R;
import com.example.jnitrace.*;

import java.lang.reflect.Method;


public class MainActivity extends AppCompatActivity {

    public native void startJnitrace(String soname);

    static {
        System.loadLibrary("jnitrace");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        startJnitrace("tracetest");

        System.loadLibrary("tracetest");

    }


}