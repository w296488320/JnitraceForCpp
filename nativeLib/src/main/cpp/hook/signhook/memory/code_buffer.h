//
// Created by SwiftGan on 2020/9/15.
//

#pragma once

#include <common/types.h>
#include <vector>
#include <common/cow_vector.h>

namespace SandHook::Cache {

    // simple impl
    // TODO
    class CodeBuffer {
    public:

        explicit CodeBuffer(u32 max_size, u32 align = 0);

        std::pair<void *, void *> AcquireBuffer(size_t size);

        void* GetRwBuffer(void *rx);

    protected:
        u32 current_offset{0};
        CowVector<u8, true> code_buffer_;
        u32 buffer_size_;
        u32 align;
        std::mutex lock_;
    };

}
