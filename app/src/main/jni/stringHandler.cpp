//
// Created by zhenxi on 2022/1/21.
//
#include <iosfwd>
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <jni.h>
#include <dlfcn.h>
#include <cstddef>
#include <fcntl.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sstream>
#include <ostream>

#include <cstdlib>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <syscall.h>
#include <climits>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <pthread.h>
#include <vector>
#include <zlib.h>
#include <list>
#include <string>
#include <map>
#include <list>

#include <cstring>
#include <cstdio>
#include <regex>
#include <cerrno>
#include <climits>
#include <iostream>
#include <fstream>

#include "stringHandler.h"
#include "mylibc.h"
#include "logging.h"
#include "common_macros.h"
#include "HookUtils.h"
#include "ZhenxiLog.h"

using namespace std;
/**
 * 因为需要hook一些string处理的函数,所以尽可能使用C语言去实现这些功能
 */
namespace ZhenxiRunTime::stringHandlerHook {

#define HOOK_SYMBOL(handle, func)  \
  hook_libc_function(handle, #func, (void*) new_##func, (void**) &orig_##func); \


#define HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)

#define JNITRACE_HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)


#define  ORIG_BUFF_SIZE  100

#define  MATCH_SO_NAME_SIZE  100

#define  MAX_PRINTF_SIZE  PATH_MAX

#define INIT_ORIG_BUFF \
    char *buff = (char*)malloc(ORIG_BUFF_SIZE); \
    my_memset(buff,0,ORIG_BUFF_SIZE); \

#define HOOK_SYMBOL_DOBBY(handle, func)  \
  hook_libc_function(handle, #func, (void*) new_##func, (void**) &orig_##func); \

//这块是为了防止他把一个void*当成char*去strlen挂掉
# define IS_NULL(value) value \

# define APPEND_INT(buff, value) \
     char* int_value = int_to_str(value);           \
     if(int_value!=nullptr){                           \
        APPEND(buff,int_value);                   \
        free(int_value);                           \
     }


# define APPEND(buff, value) \
     if(my_strlen(value)>0){     \
        buff = (char *)realloc(buff,my_strlen(buff)+my_strlen(value)+1); \
        my_strcat(buff,value);  \
     }  \

#define GET_ADDRESS \
 auto address = getAddressHex((void*)((char *) \
            __builtin_return_address(0) - ((size_t) info.dli_fbase))); \


# define CLEAN_APPEND(buff, value, size) \
     my_memset(buff,0,size); \
     my_strcat(buff,value);  \

    /**
     * 是否监听第二级返回结果。
     *
     */
    static bool isHookSecondRet = false;

    static std::ofstream *hookStrHandlerOs;
    static char *filterSoList[100] = {};
    static char *forbidSoList[100] = {};
    static bool isHookAll = false;

    static bool isSave = false;
    static char match_so_name[MATCH_SO_NAME_SIZE] = {};
    static std::mutex supernode_ids_mux_;


    static bool isAppFile(const char *path) {
        if (my_strstr(path, "/data/") != nullptr) {
            return true;
        }
        return false;
    }

    const char *getFileNameForPath(const char *path) {
        const char *fileName = strrchr(path, '/');
        if (fileName != nullptr) {
            return fileName + 1;
        }
        return path;
    }

# define DL_INFO \
    Dl_info info={nullptr}; \
    int addr_ret_0 = dladdr((void *) __builtin_return_address(0), &info); \

# define IS_MATCH \
        if(ZhenxiRunTime::stringHandlerHook::isLister(addr_ret_0,&info)){  \


    static bool isLister(int dladd_ret, Dl_info *info) {
        if (dladd_ret == 0) {
            return false;
        }
        if (info == nullptr) {
            return false;
        }
        const char *name = info->dli_fname;

        if (name == nullptr) {
            return false;
        }
        //系统apk暂不处理,只监听当前apk包下的
        if (!isAppFile(name)) {
            return false;
        }
        //如果是已经过滤的apk也暂不处理
        //比如我们注入的SO文件
        for (auto forbid: forbidSoList) {
            if (my_strstr(name, forbid) != nullptr) {
                //找到了则不进行处理
                return false;
            }
        }
        //如果是监听全部直接返回true
        if (isHookAll) {
            CLEAN_APPEND(match_so_name, getFileNameForPath(name), MATCH_SO_NAME_SIZE);
            return true;
        } else {
            //根据关键字进行过滤
            for (auto filter: filterSoList) {
                //默认监听一级
                if (my_strstr(name, filter) != nullptr) {
                    CLEAN_APPEND(match_so_name, getFileNameForPath(name), MATCH_SO_NAME_SIZE);
                    return true;
                }
            }
            return false;
        }
    }

    char *getAddressHex(void *ptr) {
        if (ptr == nullptr) {
            return nullptr;
        }
        char *buffer = (char *) malloc(20);
        sprintf(buffer, "0x%lx", (uintptr_t) ptr);
        return buffer;
    }

    static void write(const char *msg, Dl_info info) {
        INIT_ORIG_BUFF
        if ((size_t) info.dli_fbase > 0) {
            GET_ADDRESS
            if (address != nullptr) {
                APPEND(buff, "<")
                APPEND(buff, address)
                APPEND(buff, ">")
                free((void *) address);
            }
        }
        APPEND(buff, "[")
        APPEND(buff, match_so_name)
        APPEND(buff, "]")
        APPEND(buff, msg)
        if (isSave) {
            if (hookStrHandlerOs != nullptr) {
                (*hookStrHandlerOs) << buff;
            }
        }
        LOG(INFO) << buff;
        if (msg != nullptr) {
            free((void *) msg);
        }
        if (buff != nullptr) {
            free((void *) buff);
        }
    }





// char* strstr(char* h, const char* n)
    HOOK_DEF(char*, strstr, char *h, const char *n) {
        DL_INFO
        IS_MATCH char *ret = orig_strstr(h, n);
            if (ret != nullptr) {
                INIT_ORIG_BUFF
                APPEND(buff, "strstr() arg1 -> ")
                APPEND(buff, IS_NULL(h))
                APPEND(buff, "  arg2-> ")
                APPEND(buff, IS_NULL(n))
                APPEND(buff, "\n")
                write(buff, info);
            }
            return ret;

        }
        return orig_strstr(h, n);
    }

    //size_t __strlen_chk(const char *a1, size_t a2)
    HOOK_DEF(size_t, __strlen_chk, const char *__s,size_t a2) {
        DL_INFO
        IS_MATCH
        size_t size = orig___strlen_chk(__s,a2);
            if (size > 0) {
                INIT_ORIG_BUFF
                APPEND(buff, "strlen_chk() arg1 -> ")
                APPEND(buff, IS_NULL(__s))
                APPEND(buff, "\n")
                write(buff, info);
            }
            return size;
        }
        return orig___strlen_chk(__s,a2);
    }

    HOOK_DEF(size_t, strlen, const char *__s) {
        DL_INFO
        IS_MATCH
            size_t size = orig_strlen(__s);
            if (size > 0) {
                INIT_ORIG_BUFF
                APPEND(buff, "strlen() arg1 -> ")
                APPEND(buff, IS_NULL(__s))
                APPEND(buff, "\n")
                write(buff, info);
            }
            return size;
        }
        return orig_strlen(__s);
    }

//char* strcat(char* __dst, const char* __src);
    HOOK_DEF(char *, strcat, char *__dst, const char *__src) {
        DL_INFO
        IS_MATCH char *ret = orig_strcat(__dst, __src);
            INIT_ORIG_BUFF
            APPEND(buff, "strcat() arg1 -> ")
            APPEND(buff, IS_NULL(__dst))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(__src))
            if (ret != nullptr) {
                APPEND(buff, " result -> ")
                APPEND(buff, IS_NULL(ret))
            }
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }
        return orig_strcat(__dst, __src);
    }
//int strcmp(const char* __lhs, const char* __rhs)
    HOOK_DEF(int, strcmp, const char *__lhs, const char *__rhs) {
        DL_INFO
        IS_MATCH int ret = orig_strcmp(__lhs, __rhs);
            INIT_ORIG_BUFF
            APPEND(buff, "strcmp() arg1 -> ")
            APPEND(buff, IS_NULL(__lhs))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(__rhs))
            APPEND(buff, "\n")
            write(buff, info);
            return ret; }
        return orig_strcmp(__lhs, __rhs);
    }
//char* strcpy(char* __dst, const char* __src);
    HOOK_DEF(char *, strcpy, char *__dst, const char *__src) {

        DL_INFO
        IS_MATCH char *ret = orig_strcpy(__dst, __src);
            INIT_ORIG_BUFF
            APPEND(buff, "strcpy() arg1 -> ")
            APPEND(buff, IS_NULL(__dst))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(__src))

            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }
        return orig_strcpy(__dst, __src);
    }
//int sprintf(char* __s, const char* __fmt, ...)
    HOOK_DEF(int, sprintf, char *__s, const char *__fmt, char *p...) {
        DL_INFO
        IS_MATCH int ret = orig_sprintf(__s, __fmt, p);
            INIT_ORIG_BUFF
            APPEND(buff, "sprintf() arg1 -> ")
            APPEND(buff, IS_NULL(__s))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(__fmt))
            APPEND(buff, "  arg3-> ")
            APPEND(buff, IS_NULL(p))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }

        return orig_sprintf(__s, __fmt, p);
    }
//int printf(const char* __fmt, ...)
    HOOK_DEF(int, printf, const char *__fmt, char *p...) {
        DL_INFO
        IS_MATCH int ret = orig_printf(__fmt, p);
            INIT_ORIG_BUFF
            APPEND(buff, "printf() arg1 -> ")
            APPEND(buff, IS_NULL(__fmt))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(p))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }

        return orig_printf(__fmt, p);
    }
//char *strtok(char *str, const char *delim)
    HOOK_DEF(char *, strtok, char *str, const char *delim) {
        DL_INFO
        IS_MATCH char *ret = orig_strtok(str, delim);
            INIT_ORIG_BUFF
            APPEND(buff, "strtok() arg1 -> ")
            APPEND(buff, IS_NULL(str))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(delim))
            APPEND(buff, "\n")
            write(buff, info);
            return ret; }
        return orig_strtok(str, delim);
    }
//char* strdup(const char* __s);
    HOOK_DEF(char*, strdup, char *__s) {

        DL_INFO
        IS_MATCH

            INIT_ORIG_BUFF
            APPEND(buff, "strdup() arg1 -> ")
            APPEND(buff, IS_NULL(__s))
            APPEND(buff, "\n")
            write(buff, info);
            return orig_strdup(__s);

        }
        return orig_strdup(__s);
    }


//把 str1 和 str2 进行比较，结果取决于 LC_COLLATE 的位置设置。
//int strcoll(const char* __lhs, const char* __rhs) __attribute_pure__;
    HOOK_DEF(int, strcoll, const char *__lhs, const char *__rhs) {
        DL_INFO
        IS_MATCH int ret = orig_strcoll(__lhs, __rhs);

            INIT_ORIG_BUFF
            APPEND(buff, "strcoll() arg1 -> ")
            APPEND(buff, IS_NULL(__lhs))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(__rhs))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }
        return orig_strcoll(__lhs, __rhs);
    }
//size_t strxfrm(char *dest, const char *src, size_t n)
    HOOK_DEF(size_t, strxfrm, char *dest, const char *src, size_t n) {

        DL_INFO
        IS_MATCH size_t ret = orig_strxfrm(dest, src, n);
            INIT_ORIG_BUFF
            APPEND(buff, "strxfrm() arg1 -> ")
            APPEND(buff, IS_NULL(dest))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL(src))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }
        return orig_strxfrm(dest, src, n);
    }
//char* fgets(char* __buf, int __size, FILE* __fp);
    HOOK_DEF(char*, fgets, char *__buf, int __size, FILE *__fp) {
        DL_INFO
        IS_MATCH char *ret = orig_fgets(__buf, __size, __fp);
            INIT_ORIG_BUFF
            APPEND(buff, "fgets() arg1 -> ")
            APPEND(buff, IS_NULL(__buf))
            APPEND(buff, "  arg2-> ")
            APPEND_INT(buff, __size)
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }
        return orig_fgets(__buf, __size, __fp);
    }
    //void *memcpy(void *destin, void *source, unsigned n);
    HOOK_DEF(void*, memcpy, void *destin, void *source, size_t __n) {
        DL_INFO
        IS_MATCH

            void *ret;
            if (orig_memcpy == nullptr) {
                ret = orig_memcpy(destin, source, __n);
            } else {
                ret = my_memcpy(destin, source, __n);
            }
            if (source == nullptr || destin == nullptr) {
                return ret;
            }
            INIT_ORIG_BUFF
            APPEND(buff, "memcpy() arg1 -> ")
            APPEND(buff, IS_NULL((char *) destin))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL((char *) source))
            APPEND(buff, "  arg3-> ")
            APPEND_INT(buff, __n)
            APPEND(buff, "\n")
            write(buff, info);
            return ret;
        }

        return orig_memcpy(destin, source, __n);
    }
//int snprintf(char* __buf, size_t __size, const char* __fmt, ...) __printflike(3, 4);
    HOOK_DEF(int, snprintf, char *__buf, size_t __size, const char *__fmt, char *p  ...) {
        DL_INFO
        IS_MATCH int ret = orig_snprintf(__buf, __size, __fmt, p);
            if (ret == -1) {
                return ret;
            }
            INIT_ORIG_BUFF
            APPEND(buff, "snprintf() arg1 -> ")
            APPEND(buff, IS_NULL(__buf))
            APPEND(buff, "  arg2-> ")
            APPEND_INT(buff, __size)
            APPEND(buff, "  arg3-> ")
            APPEND(buff, IS_NULL(__fmt))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }

        return orig_snprintf(__buf, __size, __fmt, p);
    }

    HOOK_DEF(void *, malloc, size_t __byte_count) {
        DL_INFO
        IS_MATCH void *ret = orig_malloc(__byte_count);
            if (ret == nullptr || __byte_count == 0) {
                return ret;
            }
            INIT_ORIG_BUFF
            APPEND(buff, "malloc() arg1 -> ")
            APPEND_INT(buff, __byte_count)
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }
        return orig_malloc(__byte_count);
    }
//int vsnprintf(char* __buf, size_t __size, const char* __fmt, va_list __args) __printflike(3, 0);
    HOOK_DEF(int, vsnprintf, char *__buf, size_t __size, const char *__fmt, va_list __args) {
        DL_INFO
        IS_MATCH
            int ret = orig_vsnprintf(__buf, __size, __fmt, __args);
            INIT_ORIG_BUFF
            APPEND(buff, "vsnprintf() arg1 -> ")
            APPEND(buff, IS_NULL(__buf))
            APPEND(buff, "  arg2-> ")
            APPEND_INT(buff, __size)
            APPEND(buff, "  arg3-> ")
            APPEND(buff, IS_NULL(__fmt))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;
        }
        return orig_vsnprintf(__buf, __size, __fmt, __args);
    }
    //void *memmove(void *dest, const void *src, size_t n);
    HOOK_DEF(void *, memmove, void *dest, const void *src, size_t n) {
        DL_INFO
        IS_MATCH auto ret = orig_memmove(dest, src, n);
            INIT_ORIG_BUFF
            APPEND(buff, "vsnprintf() arg1 -> ")
            APPEND(buff, IS_NULL((char *) dest))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL((char *) src))
            APPEND(buff, "  arg3-> ")
            APPEND_INT(buff, n)
            APPEND(buff, "\n")
            write(buff, info);
            return ret;

        }

        return orig_memmove(dest, src, n);
    }
    //char *strncat(char *dest, const char *src, size_t n)
    HOOK_DEF(char *, strncat, char *dest, const char *src, size_t n) {
        DL_INFO
        IS_MATCH
            auto ret = orig_strncat(dest, src, n);
            INIT_ORIG_BUFF
            APPEND(buff, "strncat() arg1 -> ")
            APPEND(buff, IS_NULL((char *) dest))
            APPEND(buff, "  arg2-> ")
            APPEND(buff, IS_NULL((char *) src))
            APPEND(buff, "  arg3-> ")
            APPEND_INT(buff, n)
            APPEND(buff, "\n")
            write(buff, info);
            return ret;
        }
        return orig_strncat(dest, src, n);
    }
    //char *strlwr(char *str)
    HOOK_DEF(char *, strlwr, char *str) {
        DL_INFO
        IS_MATCH
            auto ret = orig_strlwr(str);
            INIT_ORIG_BUFF
            APPEND(buff, "strlwr() arg1 -> ")
            APPEND(buff, IS_NULL((char *) str))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;
        }
        return orig_strlwr(str);
    }
//char *strupr(char *str)
    HOOK_DEF(char *, strupr, char *str) {
        DL_INFO
        IS_MATCH
            auto ret = orig_strupr(str);
            INIT_ORIG_BUFF
            APPEND(buff, "strupr() arg1 -> ")
            APPEND(buff, IS_NULL((char *) str))
            APPEND(buff, "\n")
            write(buff, info);
            return ret;
        }
        return orig_strupr(str);
    }
//char *strchr(const char *str, int c)
    HOOK_DEF(char *, strchr, const char *str, int c) {
        DL_INFO
        IS_MATCH
            auto ret = orig_strchr(str, c);
            INIT_ORIG_BUFF
            APPEND(buff, "strchr() arg1 -> ")
            APPEND(buff, IS_NULL((char *) str))
            APPEND(buff, "  arg2-> ")
            APPEND_INT(buff, c)
            APPEND(buff, "\n")
            write(buff, info);
            return ret;
        }
        return orig_strchr(str, c);
    }

}


using namespace ZhenxiRunTime::stringHandlerHook;


void stringHandler::init() {
    void *handle = dlopen("libc.so", RTLD_NOW);

    if (handle == nullptr) {
        LOG(ERROR) << "str handler get handle == null   ";
        return;
    }

    HOOK_SYMBOL(handle, strlen)
    //strlen buff check
    HOOK_SYMBOL(handle, __strlen_chk)
    HOOK_SYMBOL(handle, strcmp)
    HOOK_SYMBOL(handle, strstr)
    HOOK_SYMBOL(handle, fgets)
    HOOK_SYMBOL(handle, strcpy)
    HOOK_SYMBOL(handle, strdup)

//    HOOK_SYMBOL_DOBBY(handle, strxfrm)
//    HOOK_SYMBOL_DOBBY(handle, strtok)
//    HOOK_SYMBOL_DOBBY(handle, strncat)
//    HOOK_SYMBOL_DOBBY(handle, strlwr)
//    HOOK_SYMBOL_DOBBY(handle, strupr)
//    HOOK_SYMBOL_DOBBY(handle, strchr)

    //strcat底层走的是strcpy
    //https://cs.android.com/android/platform/superproject/+/master:external/musl/src/string/strcat.c;l=5?q=strcat
//    HOOK_SYMBOL_DOBBY(handle, strcat)
    //strcoll底层是strcmp
    //https://cs.android.com/android/platform/superproject/+/master:external/musl/src/locale/strcoll.c;l=12;drc=master?q=strcoll&ss=android%2Fplatform%2Fsuperproject
//    HOOK_SYMBOL_DOBBY(handle, strcoll)


    //HOOK_SYMBOL_DOBBY(handle, sprintf)
    //HOOK_SYMBOL_DOBBY(handle, printf)
    //HOOK_SYMBOL_DOBBY(handle, snprintf)
    //HOOK_SYMBOL_DOBBY(handle, vsnprintf)

//    HOOK_SYMBOL_DOBBY(handle, memmove)
//    HOOK_SYMBOL_DOBBY(handle, memcpy)




    dlclose(handle);

    LOG(ERROR) << ">>>>>>>>> string handler init success !  ";
}

void stringHandler::hookStrHandler(bool hookAll, const std::list<string> &forbid_list,
                                   const std::list<string> &filter_list, std::ofstream *os) {
    isHookAll = hookAll;

    int i = 0;
    for (const auto &string: forbid_list) {
        forbidSoList[i] = strdup(string.c_str());
        LOG(ERROR) << "forbidSoList -> " << i << " " << forbidSoList[i];
        i++;
    }
    i = 0;
    for (const auto &string: filter_list) {
        filterSoList[i] = strdup(string.c_str());
        LOG(ERROR) << "filterSoList -> " << i << " " << filterSoList[i];
        i++;
    }
    if (os != nullptr) {
        isSave = true;
        hookStrHandlerOs = os;
    } else {
        LOG(ERROR) << ">>>>>>>>>>>>> string handler init fail hookStrHandlerOs == null  ";
    }
    init();
}

[[maybe_unused]]
void stringHandler::stopjnitrace() {
    for (auto str: forbidSoList) {
        free(str);
    }
    for (auto str: filterSoList) {
        free(str);
    }
    if (hookStrHandlerOs != nullptr) {
        if (hookStrHandlerOs->is_open()) {
            hookStrHandlerOs->close();
        }
        delete hookStrHandlerOs;
    }
    isSave = false;
}


