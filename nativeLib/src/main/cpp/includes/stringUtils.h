//
// Created by zhenxi on 2021/12/8.
//

#ifndef QCONTAINER_PRO_STRINGUTILS_H
#define QCONTAINER_PRO_STRINGUTILS_H

#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <malloc.h>
#include <regex>
#include <bits/getopt.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <asm/fcntl.h>
#include <fcntl.h>
#include <vector>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

namespace StringUtils {

    bool compare_pred(unsigned char a, unsigned char b);

//    std::string vector2str(vector<std::string> &&arr, const char* sp = ", ");

    bool equals(const std::string &str1, const std::string &str2);

    bool endsWith(const std::string &str, const std::string &suffix);
    bool isNumeric(const std::string& str);
    bool endsWith(const char *s, const std::string &suffix);

    bool startWith(const std::string &str, const std::string &suffix);

    bool startWith(const char *str, const std::string &suffix);

    bool contains(const std::string &str, const std::string &suffix);

    bool contains(const char *s, const std::string &suffix);
    /**
     * 包含不区分大小写
     * @param str
     * @param substr
     * @return
     */
    bool containsInsensitive(const std::string& str, const std::string& substr);

    int replace(std::string &str, const std::string &pattern, const std::string &newpat);

    /**
     * 字符串转换16进制
     */
    std::string binaryToHex(const std::string &binaryStr);

    /**
     * 16进制转换10进制int
     */
    int HexToInt(const std::string &str);

    std::string toUpperCase(const std::string& str);
    std::string toLowerCase(const std::string& str);
}
#endif //QCONTAINER_PRO_STRINGUTILS_H
