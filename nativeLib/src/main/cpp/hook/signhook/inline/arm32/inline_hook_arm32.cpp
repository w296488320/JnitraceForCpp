//
// Created by swift on 3/12/21.
//

#include <include/inline_hook.h>
#include <common/cow_vector.h>
#include <instrcache/arm32/translator.h>
#include <memory/code_buffer.h>

namespace SandHook::Inline {

    using namespace vixl::aarch32;

    static Cache::CodeBuffer inline_code_buffer{0x4000, 16};

    constexpr bool IsThumbCode(VAddr code) {
        return (code & 0x1) == 0x1;
    }

    inline VAddr GetThumbCodeAddress(void *code) {
        return reinterpret_cast<VAddr>(code) & (~0x1);
    }

    inline void* GetThumbPC(void *code) {
        VAddr addr = reinterpret_cast<VAddr>(code) & (~0x1);
        return reinterpret_cast<void*>(addr + 1);
    }

    bool InlineHookImpl(void *origin, void *replace, void **backup) {
        if (!origin || !replace) return false;
        bool is_thumb = IsThumbCode(reinterpret_cast<VAddr>(origin));
        Platform::MakeRWX(reinterpret_cast<VAddr>(origin), 16);
        MacroAssembler inline_asm{};
        is_thumb ? inline_asm.UseT32() : inline_asm.UseA32();
        auto origin_code = is_thumb ? GetThumbCodeAddress(origin) : reinterpret_cast<VAddr>(origin);

        // inline
        if (is_thumb) {
            Literal<u32> target(reinterpret_cast<u32>(replace));
            inline_asm.ldr(pc, &target);
            inline_asm.place(&target);
        } else {
            // change mode to a32
            inline_asm.mov(ip, reinterpret_cast<u32>(replace));
            inline_asm.bx(ip);
        }
        inline_asm.FinalizeCode();

        if (backup) {
            auto inline_stub_size = inline_asm.GetBuffer()->GetSizeInBytes();
            A32::Translator translator{origin_code, inline_stub_size, is_thumb};
            translator.Decode();
            auto &backup_asm = translator.Assembler();
            Label label_origin_address;
            backup_asm.ldr(pc, &label_origin_address);
            backup_asm.bind(&label_origin_address);
            backup_asm.GetBuffer()->Emit32(reinterpret_cast<u32>(origin_code) + translator.DecodeSize());
            translator.Finish();
            auto backup_size = backup_asm.GetBuffer()->GetSizeInBytes();
            auto [backup_dest_rx, backup_dest_rw] = inline_code_buffer.AcquireBuffer(backup_size);
            std::memcpy(backup_dest_rw, backup_asm.GetBuffer()->GetStartAddress<void *>(), backup_size);

            Platform::ClearDCache(reinterpret_cast<VAddr>(backup_dest_rw), backup_size);
            Platform::ClearICache(reinterpret_cast<VAddr>(backup_dest_rx), backup_size);
            *backup = is_thumb ? GetThumbPC(backup_dest_rx) : backup_dest_rx;
        }
        std::memcpy(reinterpret_cast<void *>(origin_code), inline_asm.GetBuffer()->GetStartAddress<void *>(), inline_asm.GetBuffer()->GetSizeInBytes());
        Platform::ClearICache(reinterpret_cast<VAddr>(origin), 16);

        return true;
    }

}

