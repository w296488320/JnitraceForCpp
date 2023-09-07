//
// Created by swift on 1/10/21.
//

#include <common/types.h>
#include <exts/vixl/aarch32/disasm-aarch32.h>
#include <exts/vixl/aarch32/macro-assembler-aarch32.h>

namespace SandHook::A32 {

    using namespace vixl::aarch32;

    enum ArmRegister {
        R0 = 0,
        R1 = 1,
        R2 = 2,
        R3 = 3,
        R4 = 4,
        R5 = 5,
        R6 = 6,
        R7 = 7,
        R8 = 8,
        R9 = 9,
        R10 = 10,
        R11 = 11,
        R12 = 12,
        R13 = 13,
        R14 = 14,
        R15 = 15,
        MR = 8,
        TR = 9,
        FP = 11,
        IP = 12,
        SP = 13,
        LR = 14,
        PC = 15,
        kNumberOfCoreRegisters = 16,
        kNoRegister = -1,
    };

    class Translator : public vixl::aarch32::Disassembler {
    public:

        Translator(u32 code_address, size_t size, bool thumb_mode);

        void Decode();

        void Finish();

    private:

        enum ThumbType {
            Thumb16, Thumb32
        };

    public:
        void adr(Condition cond, EncodingSize size, Register rd, Location *location) override;

        void b(Condition cond, EncodingSize size, Location *location) override;

        void bl(Condition cond, Location *location) override;

        void blx(Condition cond, Location *location) override;

        void cbnz(Register rn, Location *location) override;

        void cbz(Register rn, Location *location) override;

        void ldr(Condition cond, EncodingSize size, Register rt, Location *location) override;

        void ldrb(Condition cond, Register rt, Location *location) override;

        void ldrd(Condition cond, Register rt, Register rt2, Location *location) override;

        void ldrh(Condition cond, Register rt, Location *location) override;

        void ldrsb(Condition cond, Register rt, Location *location) override;

        void ldrsh(Condition cond, Register rt, Location *location) override;

        void pld(Condition cond, Location *location) override;

        void pli(Condition cond, Location *location) override;

        void vldr(Condition cond, DataType dt, DRegister rd, Location *location) override;

        void vldr(Condition cond, DataType dt, SRegister rd, Location *location) override;

        constexpr u32 DecodeSize() {
            return code_address - code_start;
        }

        constexpr MacroAssembler &Assembler() {
            return masm;
        }

    private:

        void UnImplT32(u32 code);

        void UnImplT16(u16 code);

        void UnImplA32(u32 code);

        constexpr Label *AllocLabel() {
            auto res = &labels.emplace_back();
//            auto res = &labels.push_back();
            return res;
        }

        std::tuple<u32, ThumbType> ReadThumbInstr(u32 arm_pc);

        u32 code_start{};
        u32 code_address{};
        size_t size{};
        bool thumb_mode{false};
        ThumbType current_thumb_type{};
        u32 pc{};
        MacroAssembler masm{};
        std::list<Label> labels{};
        bool hit_instr{false};
    };

}
