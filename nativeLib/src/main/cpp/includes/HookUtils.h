//
// Created by zhenxi on 2021/5/16.
//


#include "logging.h"

#include "dobby.h"

#ifndef VMP_HOOKUTILS_H
#define VMP_HOOKUTILS_H

#define HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__)=nullptr; \
  ret new_##func(__VA_ARGS__)

#define HOOK_ADDRES(base,offset,name) bool is##name = HookUtils::Hooker((char *) il2cpp_base + offset,\
                                (void *) new_##name, \
                                (void **) &orig_##name);                                         \
                                if(is##name){                                                    \
                                   LOGE(#name)     \
                                } \


class HookUtils {
public:
    static bool HookerForSign(void *dysym, void *newrep, void **org);

    static bool Hooker(void *dysym, void *repl, void **org);

    static bool Hooker(void *handler, const char *dysym, void *repl, void **org);

    static bool Hooker(void *dysym, void *repl, void **org, const char *dynSymName);

    static bool Hooker(const char *libName, const char *dysym, void *repl, void **org);

    static bool addTrampoline(void *dysym , dobby_instrument_callback_t pre_handler);

    static void startBranchTrampoline();

    static bool unHook(void *sym);

};

void hook_libc_function(void *handle, const char *symbol, void *new_func, void **old_func) ;

#endif //VMP_HOOKUTILS_H
