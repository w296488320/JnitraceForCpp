//
// Created by swift on 3/12/21.
//

#pragma once

#include "../common/types.h"

namespace SandHook::Inline {

    bool InlineHookImpl(void *origin, void *replace, void **backup);

    bool InlineHookSymbol(char *bin, char *symbol, void *replace, void **backup);

    template <typename Func>
    bool InlineHook(Func origin, Func replace, Func* backup) {
        return InlineHookImpl(reinterpret_cast<void *>(origin), reinterpret_cast<void *>(replace), reinterpret_cast<void **>(backup));
    }



}
