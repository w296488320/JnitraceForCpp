//
// Created by 甘尧 on 2021/4/26.
//

#pragma once

#include <common/types.h>
#include <common/dlfcn_nougat.h>
#include <common/elf_util.h>
#include <include/inline_hook.h>

namespace SandHook::Inline {

    bool InlineHookSymbol(char *bin, char *sym, void *replace, void **backup) {
        auto handle = fake_dlopen(bin, 0);
        if (!handle) {
            return false;
        }
        auto fun = fake_dlsym(handle, sym);
        fake_dlclose(handle);
        if (!fun) {
            ElfImg img{bin};
            fun = reinterpret_cast<void *>(img.GetSymAddress(sym));
        }
        if (!fun) {
            return false;
        }
        return InlineHookImpl(fun, replace, backup);
    }

}