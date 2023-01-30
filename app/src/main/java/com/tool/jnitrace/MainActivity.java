package com.tool.jnitrace;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.tool.jniTrace.JniTrace;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ArrayList<String> arrayList = new ArrayList<>();
        arrayList.add("tracetest");
        JniTrace.startJnitrace(arrayList, null);

        System.loadLibrary("tracetest");
    }
}