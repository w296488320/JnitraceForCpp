//
// Created by SwiftGan on 2020/9/15.
//

#include "code_buffer.h"

namespace SandHook::Cache {

    CodeBuffer::CodeBuffer(u32 max_size, u32 align) : buffer_size_{max_size}, align{align} {
        code_buffer_.Resize(max_size);
    }

    std::pair<void *, void *> CodeBuffer::AcquireBuffer(size_t size) {
        LockGuard guard(lock_);
        auto offset_later = AlignUp(current_offset + size, align);
        if (current_offset + size > buffer_size_) {
            return {};
        }
        auto rx = code_buffer_.DataRX() + current_offset;
        auto rw = code_buffer_.DataRW() + current_offset;
        current_offset = offset_later;
        return {rx, rw};
    }

    void *CodeBuffer::GetRwBuffer(void *rx) {
        if (code_buffer_.DataRX() == code_buffer_.DataRW()) {
            return rx;
        }
        auto offset = reinterpret_cast<u8*>(rx) - code_buffer_.DataRX();
        return code_buffer_.DataRW() + offset;
    }

}
