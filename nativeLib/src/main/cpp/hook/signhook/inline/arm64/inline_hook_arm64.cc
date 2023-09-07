//
// Created by swift on 3/12/21.
//

#include <include/inline_hook.h>
#include <common/types.h>
#include <instrcache/arm64/translator.h>
#include <memory/code_buffer.h>

namespace SandHook::Inline {

    using namespace vixl::aarch64;

    const XRegister &reg_forward = x16;
    static Cache::CodeBuffer inline_code_buffer{0x4000, 4};

    bool InlineHookImpl(void *origin, void *replace, void **backup) {
        if (!origin || !replace) return false;

        Platform::MakeRWX(reinterpret_cast<VAddr>(origin), 16);

        MacroAssembler inline_asm{PositionIndependentCode};

        inline_asm.Mov(reg_forward, reinterpret_cast<VAddr>(replace));
        inline_asm.Br(reg_forward);
        inline_asm.FinalizeCode();

        if (backup) {
            A64::Translator translator{reinterpret_cast<VAddr>(origin),
                                       inline_asm.GetBuffer()->GetSizeInBytes()};

            auto &backup_asm = translator.Assembler();
            backup_asm.Mov(reg_forward,
                           reinterpret_cast<VAddr>(origin) +
                           inline_asm.GetBuffer()->GetSizeInBytes());
            backup_asm.Br(reg_forward);
            translator.Finish();

            auto backup_size = backup_asm.GetBuffer()->GetSizeInBytes();
            auto[backup_dest_rx, backup_dest_rw] = inline_code_buffer.AcquireBuffer(backup_size);
            std::memcpy(backup_dest_rw, backup_asm.GetBuffer()->GetStartAddress<void *>(),
                        backup_size);

            Platform::ClearDCache(reinterpret_cast<VAddr>(backup_dest_rw), backup_size);
            Platform::ClearICache(reinterpret_cast<VAddr>(backup_dest_rx), backup_size);
            *backup = backup_dest_rx;
        }

        //将原地址内容进行替换
        std::memcpy(origin,
                    inline_asm.GetBuffer()->GetStartAddress<void *>(),
                    inline_asm.GetBuffer()->GetSizeInBytes()
                    );

        Platform::ClearICache(reinterpret_cast<VAddr>(origin), 16);

        return true;
    }

}


