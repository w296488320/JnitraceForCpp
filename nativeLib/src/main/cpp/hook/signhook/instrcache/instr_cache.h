//
// Created by swift on 2021/2/24.
//

#pragma once

#include <common/types.h>

namespace SandHook {

    class InstrCache {
    public:
        virtual std::vector<u8> GetPICInstrCache(void *src, size_t len) = 0;
    };

}
