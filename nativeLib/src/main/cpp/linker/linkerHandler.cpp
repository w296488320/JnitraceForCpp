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

#include "logging.h"
#include "libpath.h"
#include "HookUtils.h"
#include "appUtils.h"
#include "fileUtils.h"
#include "mylibc.h"

#include "elf_util.h"
#include "Symbol.h"
#include "BinarySyscallFinder.h"
#include "linkerHandler.h"
#include "solist_util.h"

#define MATCH_ELF "libil2cpp.so"


namespace ZhenxiRunTime {
    /**
    * 保存全部回调的接口类,如果有部分模块需要使用到so的加载回调,只需要往这个callback里面添加即可 。
    */
    static list<LinkerLoadCallBack *> *linkerCallBackList;
    /**
     * 处理linker io重定向的实例该对象只能被赋值一次 。
     */
    static LinkerIORedirect *linkerIoRedirect;

    /**
     * dlopen after
     */
    void onSoLoadedAfter(const char *path, const char *redirect_path, void *ret) {
        if (path == nullptr) {
            return;
        }
        if (linkerCallBackList != nullptr) {
            for (const auto &callback: *linkerCallBackList) {
                callback->loadAfter(path,redirect_path,ret);
            }
        }
    }

    /**
     * dlopen before
     */
    void onSoLoadedBefore(const char *filename) {
        if (filename == nullptr) {
            return;
        }
        if (linkerCallBackList != nullptr) {
            for (const auto &callback: *linkerCallBackList) {
                callback->loadBefore(filename);
            }
        }
    }

    string linkerIORedirect(const char *path) {
        if (path == nullptr) {
            return {};
        }
        if (linkerIoRedirect == nullptr) {
            return {};
        }
        return linkerIoRedirect->linkerRedirect(path);
    }

    HOOK_DEF(void *, dlopen_CI, const char *filename, int flag) {
        onSoLoadedBefore(filename);
        const string &relocated_path = linkerIORedirect(filename);
        //LOGE("dlopen_CI %s ", relocated_path.c_str())
        void *ret = orig_dlopen_CI(
                relocated_path.empty() ? filename : relocated_path.c_str(), flag);
        onSoLoadedAfter(filename,relocated_path.c_str(), ret);
        return ret;
    }

    HOOK_DEF(void*, do_dlopen_CIV, const char *filename, int flag, const void *extinfo) {
        onSoLoadedBefore(filename);
        const string &relocated_path = linkerIORedirect(filename);

        //LOGE("do_dlopen_CIV %s ", relocated_path.c_str())
        void *ret = orig_do_dlopen_CIV(relocated_path.empty() ? filename : relocated_path.c_str(),
                                       flag, extinfo);
        onSoLoadedAfter(filename,relocated_path.c_str(), ret);
        return ret;
    }

    HOOK_DEF(void*, do_dlopen_CIVV, const char *filename, int flags, const void *extinfo,
             void *caller_addr) {
        onSoLoadedBefore(filename);
        const string &relocated_path = linkerIORedirect(filename);

        //LOGE("do_dlopen_CIVV %s ", relocated_path.c_str())
        void *ret = orig_do_dlopen_CIVV(relocated_path.empty() ? filename : relocated_path.c_str(),
                                        flags, extinfo, caller_addr);
        onSoLoadedAfter(filename,relocated_path.c_str(), ret);
        return ret;
    }
    HOOK_DEF(int, __openat, int fd, const char *pathname, int flags, int mode) {
        onSoLoadedBefore(pathname);
        const string &relocated_path = linkerIORedirect(pathname);
        int ret = (int) syscall(__NR_openat, fd,
                                relocated_path.empty() ? pathname : relocated_path.c_str(),
                                flags, mode);
        onSoLoadedAfter(pathname,relocated_path.c_str(), nullptr);
        return ret;
    }

#if defined(__aarch64__)
#else
    // int __open(const char *pathname, int flags, int mode);
    HOOK_DEF(int, __open, const char *pathname, int flags, int mode) {
        onSoLoadedBefore(pathname);
        const string &relocated_path = linkerIORedirect(pathname);
        int ret = (int) syscall(__NR_open,
                                relocated_path.empty() ? pathname : relocated_path.c_str(),
                                flags, mode);
        onSoLoadedAfter(pathname,relocated_path.c_str(), nullptr);
        return ret;
    }
#endif


    bool relocate_linker(const char *linker_path) {
        intptr_t linker_addr, dlopen_off, symbol;
        if ((linker_addr = get_addr(linker_path)) == 0) {
            ALOGE("cannot found linker addr  %s", linker_path)
            return false;
        }
        if (resolve_symbol(linker_path, "__dl__Z9do_dlopenPKciPK17android_dlextinfoPKv",
                           &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIVV,
                              (void **) &orig_do_dlopen_CIVV);
            return true;
        } else if (resolve_symbol(linker_path, "__dl__Z9do_dlopenPKciPK17android_dlextinfoPv",
                                  &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIVV,
                              (void **) &orig_do_dlopen_CIVV);
            return true;
        } else if (resolve_symbol(linker_path, "__dl__ZL10dlopen_extPKciPK17android_dlextinfoPv",
                                  &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIVV,
                              (void **) &orig_do_dlopen_CIVV);
            return true;
        } else if (
                resolve_symbol(linker_path,
                               "__dl__Z20__android_dlopen_extPKciPK17android_dlextinfoPKv",
                               &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIVV,
                              (void **) &orig_do_dlopen_CIVV);
            return true;
        } else if (
                resolve_symbol(linker_path, "__dl___loader_android_dlopen_ext",
                               &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIVV,
                              (void **) &orig_do_dlopen_CIVV);
            return true;
        } else if (resolve_symbol(linker_path, "__dl__Z9do_dlopenPKciPK17android_dlextinfo",
                                  &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIV,
                              (void **) &orig_do_dlopen_CIV);
            return true;
        } else if (resolve_symbol(linker_path, "__dl__Z8__dlopenPKciPKv",
                                  &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIV,
                              (void **) &orig_do_dlopen_CIV);
            return true;
        } else if (resolve_symbol(linker_path, "__dl___loader_dlopen",
                                  &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_do_dlopen_CIV,
                              (void **) &orig_do_dlopen_CIV);
            return true;
        } else if (resolve_symbol(linker_path, "__dl_dlopen",
                                  &dlopen_off) == 0) {
            symbol = linker_addr + dlopen_off;
            HookUtils::Hooker((void *) symbol, (void *) new_dlopen_CI,
                              (void **) &orig_dlopen_CI);
            return true;
        }
        return false;
    }




}

using namespace ZhenxiRunTime;


void linkerHandler::addLinkerCallBack(LinkerLoadCallBack *callBack) {
    if (linkerCallBackList == nullptr) {
        linkerCallBackList = new list<LinkerLoadCallBack *>();
    }
    linkerCallBackList->emplace_back(callBack);
}


#if defined(__aarch64__)

bool on_found_linker_syscall_arch64([[maybe_unused]] const char *path, int num, void *func) {
    //ALOGE("start hook linker load so for 64 %d  ",num);
    static uint pass = 0;
    switch (num) {
        case __NR_openat:
            HookUtils::Hooker(func, (void *) new___openat, (void **) &orig___openat);
            return BREAK_FIND_SYSCALL;
    }
    if (pass == 5) {
        return BREAK_FIND_SYSCALL;
    }
    return CONTINUE_FIND_SYSCALL;
}

#else


bool on_found_linker_syscall_arm(const char *path, int num, void *func) {
    switch (num) {
        case __NR_openat:
            HookUtils::Hooker(func, (void *) new___openat, (void **) &orig___openat);
            break;
        case __NR_open:
            HookUtils::Hooker(func, (void *) new___open, (void **) &orig___open);
            break;
    }
    return CONTINUE_FIND_SYSCALL;
}

#endif

void linkerHandler::init() {
    const char *linker = getLinkerPath();
    if (!relocate_linker(linker)) {
#if defined(__aarch64__)
        findSyscalls(linker, on_found_linker_syscall_arch64);
#else
        findSyscalls(linker, on_found_linker_syscall_arm);
#endif
    }
    LOG(ERROR) << ">>>>>>>>> linker handler init  finish!  ";
}

void linkerHandler::cleanCallBack() {
    if (linkerCallBackList != nullptr) {
        for (auto call: *linkerCallBackList) {
            delete call;
        }
        delete linkerCallBackList;
    }
}



list<string> linkerHandler::getlinkerLoadList() {
    list<string> load_elf_list;
    for (const auto &soinfo: solist::linker_get_solist()) {
        load_elf_list.emplace_back(soinfo->get_realpath());
    }
    return load_elf_list;
}

void linkerHandler::linkerIORedirect(LinkerIORedirect *callBack) {
    if (linkerIoRedirect == nullptr) {
        linkerIoRedirect = callBack;
        return;
    }
    LOGE(">>>>>>>>>>> linker IORedirect not set twice !")
    abort();
}

void linkerHandler::removelinkerList(const set<std::string_view> &names) {
    solist::RemoveFromSoList(names);
}
