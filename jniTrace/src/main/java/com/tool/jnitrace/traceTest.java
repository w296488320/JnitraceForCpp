package com.tool.jniTrace;

/**
 * @author Zhenxi on 2022/1/15
 */
public class TraceTest {

    public static String test() {
        return "test method is invoke ";
    }


    public static String test1(String str) {
        return "test method is invoke " + str;
    }

    public String test2(String str) {
        return "no static test method is invoke " + str;
    }


    public void test3(String str) {

    }

    public static int test4(String str) {
        return 555;
    }

}
