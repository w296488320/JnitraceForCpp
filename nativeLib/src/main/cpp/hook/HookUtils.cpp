//
// Created by zhenxi on 2021/5/16.
//

#include <list>

#include "HookUtils.h"

/**
 * sandhook里面的inlinehook
 * 异常hook的实现
 */
#include <inline_hook.h>

#include "xdl.h"
#include "logging.h"
#include "ZhenxiLog.h"

using namespace std;

/**
 * 保存全部hook的地址,防止某一个方法被多次Hook
 */
static list<void *> *hookedList = nullptr;

/**
 * 取消函数Hook
 * @param sym  被Hook函数地址
 */
bool HookUtils::unHook(void *sym) {
    bool ret = DobbyDestroy(sym) == RT_SUCCESS;
    if(hookedList!= nullptr){
        hookedList->remove(sym);
    }
    return ret;
}

#define PUT_PTR(dys) \
    if(hookedList!= nullptr){ \
        hookedList->push_back(dys); \
    } \

void HookUtils::startBranchTrampoline(){
    dobby_enable_near_branch_trampoline();
}

bool HookUtils::HookerForSign(void *dysym, void *newrep, void **org){
    if (dysym == nullptr) {
        LOG(ERROR) << "dobby hook org == null ";
        return false;
    }
    if (hookedList == nullptr) {
        hookedList = new list<void *>();
    }
    //如果这个地址已经被Hook了 。也有可能返回失败 。dobby 会提示 already been hooked 。
    for (void *ptr: *hookedList) {
        if (ptr == dysym) {
            //如果保存了这个地址,说明之前hook成功过,我们也认为hook成功
            return true;
        }
    }
    return  SandHook::Inline::InlineHookImpl(dysym, newrep, org);
}
/**
 * Hook的整体封装,这个方法可以切换别的Hook框架
 * 先尝试用DobbyHook 如果Hook失败的话用InlineHook二次尝试
 *
 * @param dysym  被Hook函数地址
 * @param repl   替换函数
 * @param org    原始函数指针
 * @return 是否Hook成功
 */
bool HookUtils::Hooker(void *dysym, void *newrep, void **org) {
    if (dysym == nullptr) {
        LOG(ERROR) << "dobby hook org == null ";
        return false;
    }
    if (hookedList == nullptr) {
        hookedList = new list<void *>();
    }

    //如果这个地址已经被Hook了 。也有可能返回失败 。dobby 会提示 already been hooked 。
    for (void *ptr: *hookedList) {
        if (ptr == dysym) {
            //如果保存了这个地址,说明之前hook成功过,我们也认为hook成功
            return true;
        }
    }

    bool ret = DobbyHook(dysym,
                         reinterpret_cast<dobby_dummy_func_t>(newrep),
                         reinterpret_cast<dobby_dummy_func_t *>(org)) == RT_SUCCESS;
    if (ret) {
        //LOG(ERROR) << "hook utils hook success !" ;
        //将地址添加到已经hook的列表,防止这个地址被多次hook
        hookedList->push_back(dysym);
        return true;
    }

    //如果dobby hook失败了,采用sandhook异常hook进行补救,
    LOG(ERROR) << "zhenxi runtime inlinehook start sandhook InlineHookImpl  ";
    ret = SandHook::Inline::InlineHookImpl(dysym, newrep, org);
    if (ret) {
        PUT_PTR(dysym)
        return true;
    }

    LOG(ERROR)
            << ">>>>>>>>>>>>>>> sandhook inlinehook hook error,start dobby branch_trampoline hook ";
    //如果sandhook sign hook 也失败了,我们采用dobby附近插装去hook
    dobby_enable_near_branch_trampoline();
    //二次hook
    ret = DobbyHook(dysym,
                    reinterpret_cast<dobby_dummy_func_t>(newrep),
                    reinterpret_cast<dobby_dummy_func_t *>(org)) == RT_SUCCESS;
    //关闭附近插装
    dobby_disable_near_branch_trampoline();
    if (!ret) {
        LOG(ERROR) << "!!!!!!!!!!!!!!!  HookUtils hook error   ";
        return false;
    }
    PUT_PTR(dysym)
    return ret;

}

bool HookUtils::Hooker(void *handler, const char *dysym, void *repl, void **org) {
    void *sym = getSymCompatForHandler(handler, dysym);
    if (sym == nullptr) {
        LOG(ERROR) << "HookUtils hook sym == null    " << dysym;
        return false;
    }
    bool isSuccess = Hooker(sym, repl, org);
    if (!isSuccess) {
        LOG(ERROR) << "HookUtils hook sym error   " << dysym;
    }
    return isSuccess;
}

/**
 *
 * @param dysym  被Hook函数地址
 * @param repl   替换函数
 * @param org    原始函数指针
 * @param dynSymName  函数的符号,主要为了在失败时候方便打印那个函数失败了
 * @return 是否Hook成功
 */
bool HookUtils::Hooker(void *dysym, void *repl, void **org, const char *dynSymName) {
    if (Hooker(dysym, repl, org)) {
        return true;
    }
    return false;
}
/**
 * 对一个方法进行插装
 * dobby_instrument_callback_t
 */
bool HookUtils::addTrampoline(void *dysym,dobby_instrument_callback_t pre_handler) {
    return DobbyInstrument(dysym,pre_handler) == RT_SUCCESS;
}
/**
 *
 * @param libName  So的路径
 * @param dysym  函数的符号
 * @param repl   替换函数
 * @param org    原始函数指针
 * @return 是否Hook成功
 */
bool HookUtils::Hooker(const char *libName, const char *dysym, void *repl, void **org) {
    void *sym = getSymCompat(libName, dysym);
    if (sym == nullptr) {
        LOG(ERROR) << "Hooker  get sym error " << dysym;
        return false;
    }
    bool isSucess = Hooker(sym, repl, org);
    if (!isSucess) {
        LOG(ERROR) << "Hooker hook error " << libName << " " << dysym;
    }
    return isSucess;
}



/**
 * 处理libc相关
 */
void hook_libc_function(void *handle, const char *symbol, void *new_func, void **old_func) {
    void *addr = dlsym(handle, symbol);
    if (addr == nullptr) {
        LOGE("hook_function_libc not found symbol : %s", symbol)
        return;
    }
    if (!HookUtils::Hooker(addr, new_func, old_func)) {
        LOGE(">>>>>>>>>>> io  hook %s fail !", symbol)
    }
}