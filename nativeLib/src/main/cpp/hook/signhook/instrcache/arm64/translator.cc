//
// Created by swift on 2021/2/24.
//

#include "translator.h"

namespace SandHook::A64 {

#define __ masm.

    const XRegister& reg_tmp = x16;
    const XRegister& reg_tmp2 = x17;
    const XRegister& reg_forward = x16;

    Translator::Translator(u64 start, size_t length)
            : current_pc(start), length(length) {
        auto end = start + length;
        Decoder decoder{};
        decoder.AppendVisitor(this);
        while (current_pc < end) {
            decoder.Decode(reinterpret_cast<Instruction *>(current_pc));
            current_pc += 4;
        }
    }

    void Translator::VisitCompareBranch(const Instruction *instr) {
        auto &rt = XRegister::GetXRegFromCode(instr->GetRt());
        auto offset = instr->GetImmBranch() * static_cast<int>(kInstructionSize);
        auto target = current_pc + offset;
        auto label_continue = AllocLabel();
        switch (instr->Mask(CompareBranchMask)) {
            case CBZ_w:
                __ Cbnz(rt.W(), label_continue);
                break;
            case CBZ_x:
                __ Cbnz(rt, label_continue);
                break;
            case CBNZ_w:
                __ Cbz(rt.W(), label_continue);
                break;
            case CBNZ_x:
                __ Cbz(rt, label_continue);
                break;
            default:
                abort();
        }
        __ Mov(reg_forward, target);
        __ Br(reg_forward);
        __ Bind(label_continue);
    }

    void Translator::VisitConditionalBranch(const Instruction *instr) {
        auto cond = instr->GetConditionBranch();
        auto offset = instr->GetImmBranch() * static_cast<int>(kInstructionSize);
        auto target = current_pc + offset;
        auto label_continue = AllocLabel();
        auto label_goto = AllocLabel();
        __ B(Condition(cond), label_goto);
        __ B(label_continue);
        __ Bind(label_goto);
        __ Mov(reg_forward, target);
        __ Br(reg_forward);
        __ Bind(label_continue);
    }

    void Translator::VisitLoadLiteral(const Instruction *instr) {
        const auto rt = instr->GetRt();
        auto &rt_v = VRegister::GetVRegFromCode(rt);
        auto &rt_x = XRegister::GetXRegFromCode(rt);
        auto offset = instr->GetImmLLiteral() * kLiteralEntrySize;
        auto load_address = current_pc + offset;
        __ Mov(reg_tmp, load_address);
        switch (instr->Mask(LoadLiteralMask)) {
            case LDR_w_lit:
                __ Ldr(rt_x.W(), MemOperand(reg_tmp));
                break;
            case LDR_x_lit:
                __ Ldr(rt_x, MemOperand(reg_tmp));
                break;
            case LDR_s_lit:
                __ Ldr(rt_v.S(), MemOperand(reg_tmp));
                break;
            case LDR_d_lit:
                __ Ldr(rt_v.D(), MemOperand(reg_tmp));
                break;
            case LDR_q_lit:
                __ Ldr(rt_v.Q(), MemOperand(reg_tmp));
                break;
            case LDRSW_x_lit:
                __ Ldrsw(rt_x, MemOperand(reg_tmp));
                break;
            case PRFM_lit:
                __ Prfm(PrefetchOperation(instr->GetImmPrefetchOperation()), MemOperand(reg_tmp));
                break;
            default:
                abort();
                break;
        }
    }

    void Translator::VisitPCRelAddressing(const Instruction *instr) {
        auto &rd = XRegister::GetXRegFromCode(instr->GetRd());
        auto offset = instr->GetImmPCRel();
        auto base = current_pc;
        auto target = base + offset;
        if (instr->Mask(PCRelAddressingMask) == ADRP) {
            offset *= page_size;
            base = AlignDown(base, page_size);
        }
        __ Mov(rd, base + offset);
    }

    void Translator::VisitTestBranch(const Instruction *instr) {
        auto &rt = XRegister::GetXRegFromCode(instr->GetRt());
        auto offset = instr->GetImmBranch() * static_cast<int>(kInstructionSize);
        auto target = current_pc + offset;
        auto label_continue = AllocLabel();
        auto bit_pos =
                (instr->GetImmTestBranchBit5() << 5) | instr->GetImmTestBranchBit40();
        switch (instr->Mask(TestBranchMask)) {
            case TBZ:
                __ Tbnz(rt, bit_pos, label_continue);
                break;
            case TBNZ:
                __ Tbz(rt, bit_pos, label_continue);
                break;
            default:
                abort();
        }
        __ Mov(reg_forward, target);
        __ Br(reg_forward);
        __ Bind(label_continue);
    }

    void Translator::VisitUnconditionalBranch(const Instruction *instr) {
        auto target = reinterpret_cast<u64>(instr->GetImmPCOffsetTarget());
        __ Mov(reg_forward, target);
        switch (instr->Mask(UnconditionalBranchMask)) {
            case BL:
                __ Blr(reg_forward);
                break;
            case B:
                __ Br(reg_forward);
                break;
            default:
                VIXL_UNREACHABLE();
        }
    }

    void Translator::VisitUnimplemented(const Instruction *instr) {
        __ Emit(instr->GetInstructionBits());
    }

    void Translator::VisitReserved(const Instruction *instr) {
        __ Emit(instr->GetInstructionBits());
    }

    void Translator::VisitUnallocated(const Instruction *instr) {
        __ Emit(instr->GetInstructionBits());
    }

    void Translator::Finish() {
        __ FinalizeCode();
    }


}