//
// Created by swift on 2021/2/24.
//

#pragma once

#include <common/types.h>
#include <aarch64/macro-assembler-aarch64.h>

namespace SandHook::A64 {

#define VISITOR_LIST_THAT_INTEREST(V)     \
  V(CompareBranch)                      \
  V(ConditionalBranch)                  \
  V(LoadLiteral)                        \
  V(PCRelAddressing)                    \
  V(TestBranch)                         \
  V(UnconditionalBranch)

    using namespace vixl::aarch64;

    class Translator : public DecoderVisitor {
    public:

        explicit Translator(u64 start, size_t length);

        void Finish();

        // Declare all Visitor functions.
#define DECLARE(A) \
  virtual void Visit##A(const Instruction* instr) VIXL_OVERRIDE;
        VISITOR_LIST_THAT_INTEREST(DECLARE)
#undef DECLARE


#define DECLARE(A) \
  virtual void Visit##A(const Instruction* instr) VIXL_OVERRIDE;
        VISITOR_LIST_THAT_DONT_RETURN(DECLARE)
#undef DECLARE


#define DECLARE(A)                                                             \
  VIXL_NO_RETURN_IN_DEBUG_MODE virtual void Visit##A(const Instruction* instr) \
      VIXL_OVERRIDE;
        VISITOR_LIST_THAT_DONT_RETURN_IN_DEBUG_MODE(DECLARE)
#undef DECLARE

        constexpr MacroAssembler &Assembler() {
            return masm;
        }

    private:

        constexpr Label *AllocLabel() {
            auto res = &labels.emplace_back();
            return res;
        }

        u64 current_pc{};
        size_t length{};
        MacroAssembler masm{PositionIndependentCode};
        std::list<Label> labels{};
    };

}
