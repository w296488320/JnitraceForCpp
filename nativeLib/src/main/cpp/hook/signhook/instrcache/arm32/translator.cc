//
// Created by swift on 1/10/21.
//

#include "translator.h"

namespace SandHook::A32 {

#define __ masm.

    const vixl::aarch32::Register &Reg(int reg) {
#define CASE(x) case R##x:  \
        return vixl::aarch32::r##x;

        switch (reg) {
            CASE(0)
            CASE(1)
            CASE(2)
            CASE(3)
            CASE(4)
            CASE(5)
            CASE(6)
            CASE(7)
            CASE(8)
            CASE(9)
            CASE(10)
            CASE(11)
            CASE(12)
            CASE(13)
            CASE(14)
            CASE(15)
            default:
                abort();
        }
#undef CASE
    }

    constexpr bool IsThumb16(u16 first_part) {
        return (first_part & 0xF800) <= 0xE800;
    }

    std::tuple<u32, Translator::ThumbType> Translator::ReadThumbInstr(u32 arm_pc) {
        u32 first_part = *reinterpret_cast<u16*>(arm_pc);
        if (IsThumb16(static_cast<u16>(first_part))) {
            return std::make_tuple(first_part, Thumb16);
        }
        u32 second_part = *reinterpret_cast<u16*>(arm_pc + 2);
        u32 instr_32 = (first_part << 16) | second_part;
        return std::make_tuple(instr_32, Thumb32);
    }

    Translator::Translator(u32 code_address, size_t size, bool thumb_mode)
            : Disassembler(NullStream(), code_address), code_address(code_address), code_start(code_address), size(size), thumb_mode(thumb_mode) {
        thumb_mode ? masm.UseT32() : masm.UseA32();
    }

    void Translator::Decode() {
        while (code_address < code_start + size) {
            if (thumb_mode) {
                auto [thumb_instr, thumb_type] = ReadThumbInstr(code_address);
                current_thumb_type = thumb_type;
                pc = code_address + 4;
                if (thumb_type == Thumb32) {
                    DecodeT32(thumb_instr);
                    code_address += 4;
                    if (!hit_instr) {
                        UnImplT32(thumb_instr);
                    }
                } else {
                    DecodeT32(thumb_instr << 16);
                    code_address += 2;
                    if (!hit_instr) {
                        UnImplT16(thumb_instr);
                    }
                }
            } else {
                auto a32 = *reinterpret_cast<u32*>(code_address);
                pc = code_address + 8;
                DecodeA32(a32);
                code_address += 4;
                if (!hit_instr) {
                    UnImplA32(a32);
                }
            }
            hit_instr = false;
        }
    }

    void Translator::Finish() {
        __ FinalizeCode();
    }

    void Translator::adr(Condition cond, EncodingSize size, Register rd,
                         Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        __ mov(cond, rd, target);
        hit_instr = true;
    }

    void Translator::b(Condition cond, EncodingSize size, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        __ mov(cond, vixl::aarch32::pc, target);
        hit_instr = true;
    }

    void Translator::bl(Condition cond, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        __ add(cond, lr, vixl::aarch32::pc, 4);
        __ mov(cond, vixl::aarch32::pc, target);
        hit_instr = true;
    }

    void Translator::blx(Condition cond, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        __ add(cond, lr, vixl::aarch32::pc, 4);
        __ mov(cond, vixl::aarch32::pc, target);
        hit_instr = true;
    }

    void Translator::cbz(Register rn, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto label_continue = AllocLabel();
        __ cbnz(rn, label_continue);
        __ mov(vixl::aarch32::pc, target);
        __ bind(label_continue);
        hit_instr = true;
    }

    void Translator::cbnz(Register rn, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto label_continue = AllocLabel();
        __ cbz(rn, label_continue);
        __ mov(vixl::aarch32::pc, target);
        __ bind(label_continue);
        hit_instr = true;
    }

    void Translator::ldr(Condition cond, EncodingSize size, Register rt,
                         Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = (rt.Is(r0)) ? r1 : r0;
        __ push(tmp);
        __ mov(tmp, target);
        __ ldr(rt, MemOperand(tmp));
        __ pop(tmp);
        hit_instr = true;
    }

    void Translator::ldrb(Condition cond, Register rt, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = (rt.Is(r0)) ? r1 : r0;
        __ push(tmp);
        __ mov(tmp, target);
        __ ldrb(rt, MemOperand(tmp));
        __ pop(tmp);
        hit_instr = true;
    }

    void Translator::ldrd(Condition cond, Register rt, Register rt2, Disassembler::Location *location) {
        // TODO
//        auto target = pc + location->GetImmediate();
//        auto &tmp = (rt.Is(r0)) ? r1 : r0;
//        __ push(tmp);
//        __ mov(tmp, target);
//        __ ldrd(rt, MemOperand(tmp));
//        __ pop(tmp);
    }

    void Translator::ldrh(Condition cond, Register rt, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = (rt.Is(r0)) ? r1 : r0;
        __ push(tmp);
        __ mov(tmp, target);
        __ ldrh(rt, MemOperand(tmp));
        __ pop(tmp);
        hit_instr = true;
    }

    void Translator::ldrsb(Condition cond, Register rt, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = (rt.Is(r0)) ? r1 : r0;
        __ push(tmp);
        __ mov(tmp, target);
        __ ldrsb(rt, MemOperand(tmp));
        __ pop(tmp);
        hit_instr = true;
    }

    void Translator::ldrsh(Condition cond, Register rt, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = (rt.Is(r0)) ? r1 : r0;
        __ push(tmp);
        __ mov(tmp, target);
        __ ldrsh(rt, MemOperand(tmp));
        __ pop(tmp);
        hit_instr = true;
    }

    void Translator::pld(Condition cond, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = r0;
        __ push(cond, tmp);
        __ mov(cond, tmp, target);
        __ pld(cond, MemOperand(tmp));
        __ pop(cond, tmp);
        hit_instr = true;
    }

    void Translator::pli(Condition cond, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = r0;
        __ push(cond, tmp);
        __ mov(cond, tmp, target);
        __ pli(cond, MemOperand(tmp));
        __ pop(cond, tmp);
        hit_instr = true;
    }

    void Translator::vldr(Condition cond, DataType dt, DRegister rd, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = r0;
        __ push(cond, tmp);
        __ mov(cond, tmp, target);
        __ vldr(cond, rd, MemOperand(tmp));
        __ pop(cond, tmp);
        hit_instr = true;
    }

    void Translator::vldr(Condition cond, DataType dt, SRegister rd, Disassembler::Location *location) {
        auto target = pc + location->GetImmediate();
        auto &tmp = r0;
        __ push(cond, tmp);
        __ mov(cond, tmp, target);
        __ vldr(cond, rd, MemOperand(tmp));
        __ pop(cond, tmp);
        hit_instr = true;
    }

    void Translator::UnImplT32(u32 code) {
        __ GetBuffer()->Emit32(code);
    }

    void Translator::UnImplT16(u16 code) {
        if ((code & 0xf800) == 0xa000) {
            // add from pc
            Register rd = Reg((code >> 8) & 0x7);
            int imm8 = (code >> 0) & 0xff;
            int offset = imm8 << 2;
            u32 target = AlignDown(pc + offset, 4);
            __ mov(rd, target);
        } else if ((code & 0xf800) == 0x4800) {
            // ldr lit
            int imm8 = (code >> 0) & 0xff;
            auto rd = Reg(((code >> 8) & 0x7));
            int offset = imm8 << 2;
            u32 target = AlignDown(pc + offset, 4);
            __ mov(rd, target);
            __ ldr(rd, MemOperand(rd, 0));
        } else {
            __ GetBuffer()->Emit16(code);
        }
    }

    void Translator::UnImplA32(u32 code) {
        __ GetBuffer()->Emit32(code);
    }

}