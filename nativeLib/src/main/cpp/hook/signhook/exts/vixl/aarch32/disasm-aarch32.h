// Copyright 2017, VIXL authors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VIXL_DISASM_AARCH32_H_
#define VIXL_DISASM_AARCH32_H_

extern "C" {
#include <stdint.h>
}

#include <iomanip>

#include "aarch32/constants-aarch32.h"
#include "aarch32/operands-aarch32.h"

namespace vixl {
    namespace aarch32 {

        class ITBlock {
            Condition first_condition_;
            Condition condition_;
            uint16_t it_mask_;

        public:
            ITBlock() : first_condition_(al), condition_(al), it_mask_(0) {}

            void Advance() {
                condition_ = Condition((condition_.GetCondition() & 0xe) | (it_mask_ >> 3));
                it_mask_ = (it_mask_ << 1) & 0xf;
            }

            bool InITBlock() const { return it_mask_ != 0; }

            bool OutsideITBlock() const { return !InITBlock(); }

            bool LastInITBlock() const { return it_mask_ == 0x8; }

            bool OutsideITBlockOrLast() const {
                return OutsideITBlock() || LastInITBlock();
            }

            void Set(Condition first_condition, uint16_t mask) {
                condition_ = first_condition_ = first_condition;
                it_mask_ = mask;
            }

            Condition GetFirstCondition() const { return first_condition_; }

            Condition GetCurrentCondition() const { return condition_; }
        };

        class Disassembler {
        public:
            enum LocationType {
                kAnyLocation,
                kCodeLocation,
                kDataLocation,
                kCoprocLocation,
                kLoadByteLocation,
                kLoadHalfWordLocation,
                kLoadWordLocation,
                kLoadDoubleWordLocation,
                kLoadSignedByteLocation,
                kLoadSignedHalfWordLocation,
                kLoadSinglePrecisionLocation,
                kLoadDoublePrecisionLocation,
                kStoreByteLocation,
                kStoreHalfWordLocation,
                kStoreWordLocation,
                kStoreDoubleWordLocation,
                kStoreSinglePrecisionLocation,
                kStoreDoublePrecisionLocation,
                kVld1Location,
                kVld2Location,
                kVld3Location,
                kVld4Location,
                kVst1Location,
                kVst2Location,
                kVst3Location,
                kVst4Location
            };

            class ConditionPrinter {
                const ITBlock &it_block_;
                Condition cond_;

            public:
                ConditionPrinter(const ITBlock &it_block, Condition cond)
                        : it_block_(it_block), cond_(cond) {}

                const ITBlock &GetITBlock() const { return it_block_; }

                Condition GetCond() const { return cond_; }

                friend std::ostream &operator<<(std::ostream &os, ConditionPrinter cond) {
                    if (cond.it_block_.InITBlock() && cond.cond_.Is(al) &&
                        !cond.cond_.IsNone()) {
                        return os << "al";
                    }
                    return os << cond.cond_;
                }
            };

            class ImmediatePrinter {
                uint32_t imm_;

            public:
                explicit ImmediatePrinter(uint32_t imm) : imm_(imm) {}

                uint32_t GetImm() const { return imm_; }

                friend std::ostream &operator<<(std::ostream &os, ImmediatePrinter imm) {
                    return os << "#" << imm.GetImm();
                }
            };

            class SignedImmediatePrinter {
                int32_t imm_;

            public:
                explicit SignedImmediatePrinter(int32_t imm) : imm_(imm) {}

                int32_t GetImm() const { return imm_; }

                friend std::ostream &operator<<(std::ostream &os,
                                                SignedImmediatePrinter imm) {
                    return os << "#" << imm.GetImm();
                }
            };

            class RawImmediatePrinter {
                uint32_t imm_;

            public:
                explicit RawImmediatePrinter(uint32_t imm) : imm_(imm) {}

                uint32_t GetImm() const { return imm_; }

                friend std::ostream &operator<<(std::ostream &os, RawImmediatePrinter imm) {
                    return os << imm.GetImm();
                }
            };

            class DtPrinter {
                DataType dt_;
                DataType default_dt_;

            public:
                DtPrinter(DataType dt, DataType default_dt)
                        : dt_(dt), default_dt_(default_dt) {}

                DataType GetDt() const { return dt_; }

                DataType GetDefaultDt() const { return default_dt_; }

                friend std::ostream &operator<<(std::ostream &os, DtPrinter dt) {
                    if (dt.dt_.Is(dt.default_dt_)) return os;
                    return os << dt.dt_;
                }
            };

            class IndexedRegisterPrinter {
                DRegister reg_;
                uint32_t index_;

            public:
                IndexedRegisterPrinter(DRegister reg, uint32_t index)
                        : reg_(reg), index_(index) {}

                DRegister GetReg() const { return reg_; }

                uint32_t GetIndex() const { return index_; }

                friend std::ostream &operator<<(std::ostream &os,
                                                IndexedRegisterPrinter reg) {
                    return os << reg.GetReg() << "[" << reg.GetIndex() << "]";
                }
            };

            // TODO: Merge this class with PrintLabel below. This Location class
            // represents a PC-relative offset, not an address.
            class Location {
            public:
                typedef int32_t Offset;

                Location(Offset immediate, Offset pc_offset)
                        : immediate_(immediate), pc_offset_(pc_offset) {}

                Offset GetImmediate() const { return immediate_; }

                Offset GetPCOffset() const { return pc_offset_; }

            private:
                Offset immediate_;
                Offset pc_offset_;
            };

            class PrintLabel {
                LocationType location_type_;
                Location::Offset immediate_;
                Location::Offset location_;

            public:
                PrintLabel(LocationType location_type,
                           Location *offset,
                           Location::Offset position)
                        : location_type_(location_type),
                          immediate_(offset->GetImmediate()),
                          location_(static_cast<Location::Offset>(
                                            static_cast<int64_t>(offset->GetPCOffset()) +
                                            offset->GetImmediate() + position)) {}

                LocationType GetLocationType() const { return location_type_; }

                Location::Offset GetLocation() const { return location_; }

                Location::Offset GetImmediate() const { return immediate_; }

                friend inline std::ostream &operator<<(std::ostream &os,
                                                       const PrintLabel &label) {
                    os << "0x" << std::hex << std::setw(8) << std::setfill('0')
                       << label.GetLocation() << std::dec;
                    return os;
                }
            };


            class PrintMemOperand {
                LocationType location_type_;
                const MemOperand &operand_;

            public:
                PrintMemOperand(LocationType location_type, const MemOperand &operand)
                        : location_type_(location_type), operand_(operand) {}

                LocationType GetLocationType() const { return location_type_; }

                const MemOperand &GetOperand() const { return operand_; }
            };

            class PrintAlignedMemOperand {
                LocationType location_type_;
                const AlignedMemOperand &operand_;

            public:
                PrintAlignedMemOperand(LocationType location_type,
                                       const AlignedMemOperand &operand)
                        : location_type_(location_type), operand_(operand) {}

                LocationType GetLocationType() const { return location_type_; }

                const AlignedMemOperand &GetOperand() const { return operand_; }
            };

            class DisassemblerStream {
                std::ostream &os_;
                InstructionType current_instruction_type_;
                InstructionAttribute current_instruction_attributes_;

            public:
                explicit DisassemblerStream(std::ostream &os)  // NOLINT(runtime/references)
                        : os_(os),
                          current_instruction_type_(kUndefInstructionType),
                          current_instruction_attributes_(kNoAttribute) {}

                virtual ~DisassemblerStream() {}

                std::ostream &os() const { return os_; }

                void SetCurrentInstruction(
                        InstructionType current_instruction_type,
                        InstructionAttribute current_instruction_attributes) {
                    current_instruction_type_ = current_instruction_type;
                    current_instruction_attributes_ = current_instruction_attributes;
                }

                InstructionType GetCurrentInstructionType() const {
                    return current_instruction_type_;
                }

                InstructionAttribute GetCurrentInstructionAttributes() const {
                    return current_instruction_attributes_;
                }

                bool Has(InstructionAttribute attributes) const {
                    return (current_instruction_attributes_ & attributes) == attributes;
                }

                template<typename T>
                DisassemblerStream &operator<<(T value) {
                    os_ << value;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const char *string) {
                    os_ << string;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const ConditionPrinter &cond) {
                    os_ << cond;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Condition cond) {
                    os_ << cond;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const EncodingSize &size) {
                    os_ << size;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const ImmediatePrinter &imm) {
                    os_ << imm;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const SignedImmediatePrinter &imm) {
                    os_ << imm;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const RawImmediatePrinter &imm) {
                    os_ << imm;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const DtPrinter &dt) {
                    os_ << dt;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const DataType &type) {
                    os_ << type;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Shift shift) {
                    os_ << shift;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Sign sign) {
                    os_ << sign;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Alignment alignment) {
                    os_ << alignment;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const PrintLabel &label) {
                    os_ << label;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const WriteBack &write_back) {
                    os_ << write_back;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const NeonImmediate &immediate) {
                    os_ << immediate;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Register reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(SRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(DRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(QRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const RegisterOrAPSR_nzcv reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(SpecialRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(MaskedSpecialRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(SpecialFPRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(BankedRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const RegisterList &list) {
                    os_ << list;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const SRegisterList &list) {
                    os_ << list;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const DRegisterList &list) {
                    os_ << list;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const NeonRegisterList &list) {
                    os_ << list;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const DRegisterLane &reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const IndexedRegisterPrinter &reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Coprocessor coproc) {
                    os_ << coproc;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(CRegister reg) {
                    os_ << reg;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(Endianness endian_specifier) {
                    os_ << endian_specifier;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(MemoryBarrier option) {
                    os_ << option;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(InterruptFlags iflags) {
                    os_ << iflags;
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const Operand &operand) {
                    if (operand.IsImmediate()) {
                        if (Has(kBitwise)) {
                            return *this << "#0x" << std::hex << operand.GetImmediate()
                                         << std::dec;
                        }
                        return *this << "#" << operand.GetImmediate();
                    }
                    if (operand.IsImmediateShiftedRegister()) {
                        if ((operand.GetShift().IsLSL() || operand.GetShift().IsROR()) &&
                            (operand.GetShiftAmount() == 0)) {
                            return *this << operand.GetBaseRegister();
                        }
                        if (operand.GetShift().IsRRX()) {
                            return *this << operand.GetBaseRegister() << ", rrx";
                        }
                        return *this << operand.GetBaseRegister() << ", " << operand.GetShift()
                                     << " #" << operand.GetShiftAmount();
                    }
                    if (operand.IsRegisterShiftedRegister()) {
                        return *this << operand.GetBaseRegister() << ", " << operand.GetShift()
                                     << " " << operand.GetShiftRegister();
                    }
                    VIXL_UNREACHABLE();
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const SOperand &operand) {
                    if (operand.IsImmediate()) {
                        return *this << operand.GetNeonImmediate();
                    }
                    return *this << operand.GetRegister();
                }

                virtual DisassemblerStream &operator<<(const DOperand &operand) {
                    if (operand.IsImmediate()) {
                        return *this << operand.GetNeonImmediate();
                    }
                    return *this << operand.GetRegister();
                }

                virtual DisassemblerStream &operator<<(const QOperand &operand) {
                    if (operand.IsImmediate()) {
                        return *this << operand.GetNeonImmediate();
                    }
                    return *this << operand.GetRegister();
                }

                virtual DisassemblerStream &operator<<(const MemOperand &operand) {
                    *this << "[" << operand.GetBaseRegister();
                    if (operand.GetAddrMode() == PostIndex) {
                        *this << "]";
                        if (operand.IsRegisterOnly()) return *this << "!";
                    }
                    if (operand.IsImmediate()) {
                        if ((operand.GetOffsetImmediate() != 0) ||
                            operand.GetSign().IsMinus() ||
                            ((operand.GetAddrMode() != Offset) && !operand.IsRegisterOnly())) {
                            if (operand.GetOffsetImmediate() == 0) {
                                *this << ", #" << operand.GetSign() << operand.GetOffsetImmediate();
                            } else {
                                *this << ", #" << operand.GetOffsetImmediate();
                            }
                        }
                    } else if (operand.IsPlainRegister()) {
                        *this << ", " << operand.GetSign() << operand.GetOffsetRegister();
                    } else if (operand.IsShiftedRegister()) {
                        *this << ", " << operand.GetSign() << operand.GetOffsetRegister()
                              << ImmediateShiftOperand(operand.GetShift(),
                                                       operand.GetShiftAmount());
                    } else {
                        VIXL_UNREACHABLE();
                        return *this;
                    }
                    if (operand.GetAddrMode() == Offset) {
                        *this << "]";
                    } else if (operand.GetAddrMode() == PreIndex) {
                        *this << "]!";
                    }
                    return *this;
                }

                virtual DisassemblerStream &operator<<(const PrintMemOperand &operand) {
                    return *this << operand.GetOperand();
                }

                virtual DisassemblerStream &operator<<(const AlignedMemOperand &operand) {
                    *this << "[" << operand.GetBaseRegister() << operand.GetAlignment()
                          << "]";
                    if (operand.GetAddrMode() == PostIndex) {
                        if (operand.IsPlainRegister()) {
                            *this << ", " << operand.GetOffsetRegister();
                        } else {
                            *this << "!";
                        }
                    }
                    return *this;
                }

                virtual DisassemblerStream &operator<<(
                        const PrintAlignedMemOperand &operand) {
                    return *this << operand.GetOperand();
                }
            };

        private:
            class ITBlockScope {
                ITBlock *const it_block_;
                bool inside_;

            public:
                explicit ITBlockScope(ITBlock *it_block)
                        : it_block_(it_block), inside_(it_block->InITBlock()) {}

                ~ITBlockScope() {
                    if (inside_) it_block_->Advance();
                }
            };

            ITBlock it_block_;
            DisassemblerStream *os_;
            uint32_t code_address_;
            // True if the disassembler always output instructions with all the
            // registers (even if two registers are identical and only one could be
            // output).
            bool use_short_hand_form_;

        protected:
            bool owns_os_;

        public:
            explicit Disassembler(std::ostream &os,  // NOLINT(runtime/references)
                                  uint32_t code_address = 0)
                    : os_(new DisassemblerStream(os)),
                      owns_os_(true),
                      code_address_(code_address),
                      use_short_hand_form_(true) {}

            explicit Disassembler(DisassemblerStream *os, uint32_t code_address = 0)
                    : os_(os),
                      owns_os_(false),
                      code_address_(code_address),
                      use_short_hand_form_(true) {}

            explicit Disassembler(uint32_t code_address)
                    : owns_os_(true),
                      code_address_(code_address),
                      use_short_hand_form_(true) {}

            virtual ~Disassembler() {
                if (owns_os_) {
                    delete os_;
                }
            }

            DisassemblerStream &os() const { return *os_; }

            void SetIT(Condition first_condition, uint16_t it_mask) {
                it_block_.Set(first_condition, it_mask);
            }

            const ITBlock &GetITBlock() const { return it_block_; }

            bool InITBlock() const { return it_block_.InITBlock(); }

            bool OutsideITBlock() const { return it_block_.OutsideITBlock(); }

            bool OutsideITBlockOrLast() const { return it_block_.OutsideITBlockOrLast(); }

            void CheckNotIT() const { VIXL_ASSERT(it_block_.OutsideITBlock()); }

            // Return the current condition depending on the IT state for T32.
            Condition CurrentCond() const {
                if (it_block_.OutsideITBlock()) return al;
                return it_block_.GetCurrentCondition();
            }

            bool UseShortHandForm() const { return use_short_hand_form_; }

            void SetUseShortHandForm(bool use_short_hand_form) {
                use_short_hand_form_ = use_short_hand_form;
            }

            virtual void UnallocatedT32(uint32_t instruction) {
                if (T32Size(instruction) == 2) {
                    os() << "unallocated " << std::hex << std::setw(4) << std::setfill('0')
                         << (instruction >> 16) << std::dec;
                } else {
                    os() << "unallocated " << std::hex << std::setw(8) << std::setfill('0')
                         << instruction << std::dec;
                }
            }

            virtual void UnallocatedA32(uint32_t instruction) {
                os() << "unallocated " << std::hex << std::setw(8) << std::setfill('0')
                     << instruction << std::dec;
            }

            virtual void UnimplementedT32_16(const char *name, uint32_t instruction) {
                os() << "unimplemented " << name << " T32:" << std::hex << std::setw(4)
                     << std::setfill('0') << (instruction >> 16) << std::dec;
            }

            virtual void UnimplementedT32_32(const char *name, uint32_t instruction) {
                os() << "unimplemented " << name << " T32:" << std::hex << std::setw(8)
                     << std::setfill('0') << instruction << std::dec;
            }

            virtual void UnimplementedA32(const char *name, uint32_t instruction) {
                os() << "unimplemented " << name << " ARM:" << std::hex << std::setw(8)
                     << std::setfill('0') << instruction << std::dec;
            }

            virtual void Unpredictable() { os() << " ; unpredictable"; }

            virtual void UnpredictableT32(uint32_t /*instr*/) { return Unpredictable(); }

            virtual void UnpredictableA32(uint32_t /*instr*/) { return Unpredictable(); }

            static bool Is16BitEncoding(uint32_t instr) { return instr < 0xe8000000; }

            uint32_t GetCodeAddress() const { return code_address_; }

            void SetCodeAddress(uint32_t code_address) { code_address_ = code_address; }

            // Start of generated code.

            virtual void adc(Condition cond,
                             EncodingSize size,
                             Register rd,
                             Register rn,
                             const Operand &operand);

            virtual void adcs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void add(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void add(Condition cond, Register rd, const Operand &operand);

            virtual void adds(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void adds(Register rd, const Operand &operand);

            virtual void addw(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void adr(Condition cond, EncodingSize size, Register rd, Location *location);

            virtual void and_(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void ands(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void asr(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rm,
                     const Operand &operand);

            virtual void asrs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rm,
                      const Operand &operand);

            virtual void b(Condition cond, EncodingSize size, Location *location);

            virtual void bfc(Condition cond, Register rd, uint32_t lsb, uint32_t width);

            virtual void bfi(
                    Condition cond, Register rd, Register rn, uint32_t lsb, uint32_t width);

            virtual void bic(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void bics(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void bkpt(Condition cond, uint32_t imm);

            virtual void bl(Condition cond, Location *location);

            virtual void blx(Condition cond, Location *location);

            virtual void blx(Condition cond, Register rm);

            virtual void bx(Condition cond, Register rm);

            virtual void bxj(Condition cond, Register rm);

            virtual void cbnz(Register rn, Location *location);

            virtual void cbz(Register rn, Location *location);

            virtual void clrex(Condition cond);

            virtual void clz(Condition cond, Register rd, Register rm);

            virtual void cmn(Condition cond,
                     EncodingSize size,
                     Register rn,
                     const Operand &operand);

            virtual void cmp(Condition cond,
                     EncodingSize size,
                     Register rn,
                     const Operand &operand);

            virtual void crc32b(Condition cond, Register rd, Register rn, Register rm);

            virtual void crc32cb(Condition cond, Register rd, Register rn, Register rm);

            virtual void crc32ch(Condition cond, Register rd, Register rn, Register rm);

            virtual void crc32cw(Condition cond, Register rd, Register rn, Register rm);

            virtual void crc32h(Condition cond, Register rd, Register rn, Register rm);

            virtual void crc32w(Condition cond, Register rd, Register rn, Register rm);

            virtual void dmb(Condition cond, MemoryBarrier option);

            virtual void dsb(Condition cond, MemoryBarrier option);

            virtual void eor(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void eors(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void fldmdbx(Condition cond,
                         Register rn,
                         WriteBack write_back,
                         DRegisterList dreglist);

            virtual void fldmiax(Condition cond,
                         Register rn,
                         WriteBack write_back,
                         DRegisterList dreglist);

            virtual void fstmdbx(Condition cond,
                         Register rn,
                         WriteBack write_back,
                         DRegisterList dreglist);

            virtual void fstmiax(Condition cond,
                         Register rn,
                         WriteBack write_back,
                         DRegisterList dreglist);

            virtual void hlt(Condition cond, uint32_t imm);

            virtual void hvc(Condition cond, uint32_t imm);

            virtual void isb(Condition cond, MemoryBarrier option);

            virtual void it(Condition cond, uint16_t mask);

            virtual void lda(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldab(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldaex(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldaexb(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldaexd(Condition cond,
                        Register rt,
                        Register rt2,
                        const MemOperand &operand);

            virtual void ldaexh(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldah(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldm(Condition cond,
                     EncodingSize size,
                     Register rn,
                     WriteBack write_back,
                     RegisterList registers);

            virtual void ldmda(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldmdb(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldmea(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldmed(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldmfa(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldmfd(Condition cond,
                       EncodingSize size,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldmib(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void ldr(Condition cond,
                     EncodingSize size,
                     Register rt,
                     const MemOperand &operand);

            virtual void ldr(Condition cond, EncodingSize size, Register rt, Location *location);

            virtual void ldrb(Condition cond,
                      EncodingSize size,
                      Register rt,
                      const MemOperand &operand);

            virtual void ldrb(Condition cond, Register rt, Location *location);

            virtual void ldrd(Condition cond,
                      Register rt,
                      Register rt2,
                      const MemOperand &operand);

            virtual void ldrd(Condition cond, Register rt, Register rt2, Location *location);

            virtual void ldrex(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldrexb(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldrexd(Condition cond,
                        Register rt,
                        Register rt2,
                        const MemOperand &operand);

            virtual void ldrexh(Condition cond, Register rt, const MemOperand &operand);

            virtual void ldrh(Condition cond,
                      EncodingSize size,
                      Register rt,
                      const MemOperand &operand);

            virtual void ldrh(Condition cond, Register rt, Location *location);

            virtual void ldrsb(Condition cond,
                       EncodingSize size,
                       Register rt,
                       const MemOperand &operand);

            virtual void ldrsb(Condition cond, Register rt, Location *location);

            virtual void ldrsh(Condition cond,
                       EncodingSize size,
                       Register rt,
                       const MemOperand &operand);

            virtual void ldrsh(Condition cond, Register rt, Location *location);

            virtual void lsl(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rm,
                     const Operand &operand);

            virtual void lsls(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rm,
                      const Operand &operand);

            virtual void lsr(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rm,
                     const Operand &operand);

            virtual void lsrs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rm,
                      const Operand &operand);

            virtual void mla(Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void mlas(Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void mls(Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void mov(Condition cond,
                     EncodingSize size,
                     Register rd,
                     const Operand &operand);

            virtual void movs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      const Operand &operand);

            virtual void movt(Condition cond, Register rd, const Operand &operand);

            virtual void movw(Condition cond, Register rd, const Operand &operand);

            virtual void mrs(Condition cond, Register rd, SpecialRegister spec_reg);

            virtual void msr(Condition cond,
                     MaskedSpecialRegister spec_reg,
                     const Operand &operand);

            virtual void mul(
                    Condition cond, EncodingSize size, Register rd, Register rn, Register rm);

            virtual void muls(Condition cond, Register rd, Register rn, Register rm);

            virtual void mvn(Condition cond,
                     EncodingSize size,
                     Register rd,
                     const Operand &operand);

            virtual void mvns(Condition cond,
                      EncodingSize size,
                      Register rd,
                      const Operand &operand);

            virtual void nop(Condition cond, EncodingSize size);

            virtual void orn(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void orns(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void orr(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void orrs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void pkhbt(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void pkhtb(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void pld(Condition cond, Location *location);

            virtual void pld(Condition cond, const MemOperand &operand);

            virtual void pldw(Condition cond, const MemOperand &operand);

            virtual void pli(Condition cond, const MemOperand &operand);

            virtual void pli(Condition cond, Location *location);

            virtual void pop(Condition cond, EncodingSize size, RegisterList registers);

            virtual void pop(Condition cond, EncodingSize size, Register rt);

            virtual void push(Condition cond, EncodingSize size, RegisterList registers);

            virtual void push(Condition cond, EncodingSize size, Register rt);

            virtual void qadd(Condition cond, Register rd, Register rm, Register rn);

            virtual void qadd16(Condition cond, Register rd, Register rn, Register rm);

            virtual void qadd8(Condition cond, Register rd, Register rn, Register rm);

            virtual void qasx(Condition cond, Register rd, Register rn, Register rm);

            virtual void qdadd(Condition cond, Register rd, Register rm, Register rn);

            virtual void qdsub(Condition cond, Register rd, Register rm, Register rn);

            virtual void qsax(Condition cond, Register rd, Register rn, Register rm);

            virtual void qsub(Condition cond, Register rd, Register rm, Register rn);

            virtual void qsub16(Condition cond, Register rd, Register rn, Register rm);

            virtual void qsub8(Condition cond, Register rd, Register rn, Register rm);

            virtual void rbit(Condition cond, Register rd, Register rm);

            virtual void rev(Condition cond, EncodingSize size, Register rd, Register rm);

            virtual void rev16(Condition cond, EncodingSize size, Register rd, Register rm);

            virtual void revsh(Condition cond, EncodingSize size, Register rd, Register rm);

            virtual void ror(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rm,
                     const Operand &operand);

            virtual void rors(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rm,
                      const Operand &operand);

            virtual void rrx(Condition cond, Register rd, Register rm);

            virtual void rrxs(Condition cond, Register rd, Register rm);

            virtual void rsb(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void rsbs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void rsc(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void rscs(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void sadd16(Condition cond, Register rd, Register rn, Register rm);

            virtual void sadd8(Condition cond, Register rd, Register rn, Register rm);

            virtual void sasx(Condition cond, Register rd, Register rn, Register rm);

            virtual void sbc(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void sbcs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void sbfx(
                    Condition cond, Register rd, Register rn, uint32_t lsb, uint32_t width);

            virtual void sdiv(Condition cond, Register rd, Register rn, Register rm);

            virtual void sel(Condition cond, Register rd, Register rn, Register rm);

            virtual void shadd16(Condition cond, Register rd, Register rn, Register rm);

            virtual void shadd8(Condition cond, Register rd, Register rn, Register rm);

            virtual void shasx(Condition cond, Register rd, Register rn, Register rm);

            virtual void shsax(Condition cond, Register rd, Register rn, Register rm);

            virtual void shsub16(Condition cond, Register rd, Register rn, Register rm);

            virtual void shsub8(Condition cond, Register rd, Register rn, Register rm);

            virtual void smlabb(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlabt(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlad(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smladx(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlal(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlalbb(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlalbt(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlald(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlaldx(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlals(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlaltb(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlaltt(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlatb(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlatt(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlawb(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlawt(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlsd(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlsdx(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smlsld(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smlsldx(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smmla(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smmlar(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smmls(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smmlsr(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void smmul(Condition cond, Register rd, Register rn, Register rm);

            virtual void smmulr(Condition cond, Register rd, Register rn, Register rm);

            virtual void smuad(Condition cond, Register rd, Register rn, Register rm);

            virtual void smuadx(Condition cond, Register rd, Register rn, Register rm);

            virtual void smulbb(Condition cond, Register rd, Register rn, Register rm);

            virtual void smulbt(Condition cond, Register rd, Register rn, Register rm);

            virtual void smull(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smulls(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void smultb(Condition cond, Register rd, Register rn, Register rm);

            virtual void smultt(Condition cond, Register rd, Register rn, Register rm);

            virtual void smulwb(Condition cond, Register rd, Register rn, Register rm);

            virtual void smulwt(Condition cond, Register rd, Register rn, Register rm);

            virtual void smusd(Condition cond, Register rd, Register rn, Register rm);

            virtual void smusdx(Condition cond, Register rd, Register rn, Register rm);

            virtual void ssat(Condition cond, Register rd, uint32_t imm, const Operand &operand);

            virtual void ssat16(Condition cond, Register rd, uint32_t imm, Register rn);

            virtual void ssax(Condition cond, Register rd, Register rn, Register rm);

            virtual void ssub16(Condition cond, Register rd, Register rn, Register rm);

            virtual void ssub8(Condition cond, Register rd, Register rn, Register rm);

            virtual void stl(Condition cond, Register rt, const MemOperand &operand);

            virtual void stlb(Condition cond, Register rt, const MemOperand &operand);

            virtual void stlex(Condition cond,
                       Register rd,
                       Register rt,
                       const MemOperand &operand);

            virtual void stlexb(Condition cond,
                        Register rd,
                        Register rt,
                        const MemOperand &operand);

            virtual void stlexd(Condition cond,
                        Register rd,
                        Register rt,
                        Register rt2,
                        const MemOperand &operand);

            virtual void stlexh(Condition cond,
                        Register rd,
                        Register rt,
                        const MemOperand &operand);

            virtual void stlh(Condition cond, Register rt, const MemOperand &operand);

            virtual void stm(Condition cond,
                     EncodingSize size,
                     Register rn,
                     WriteBack write_back,
                     RegisterList registers);

            virtual void stmda(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void stmdb(Condition cond,
                       EncodingSize size,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void stmea(Condition cond,
                       EncodingSize size,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void stmed(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void stmfa(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void stmfd(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void stmib(Condition cond,
                       Register rn,
                       WriteBack write_back,
                       RegisterList registers);

            virtual void str(Condition cond,
                     EncodingSize size,
                     Register rt,
                     const MemOperand &operand);

            virtual void strb(Condition cond,
                      EncodingSize size,
                      Register rt,
                      const MemOperand &operand);

            virtual void strd(Condition cond,
                      Register rt,
                      Register rt2,
                      const MemOperand &operand);

            virtual void strex(Condition cond,
                       Register rd,
                       Register rt,
                       const MemOperand &operand);

            virtual void strexb(Condition cond,
                        Register rd,
                        Register rt,
                        const MemOperand &operand);

            virtual void strexd(Condition cond,
                        Register rd,
                        Register rt,
                        Register rt2,
                        const MemOperand &operand);

            virtual void strexh(Condition cond,
                        Register rd,
                        Register rt,
                        const MemOperand &operand);

            virtual void strh(Condition cond,
                      EncodingSize size,
                      Register rt,
                      const MemOperand &operand);

            virtual void sub(Condition cond,
                     EncodingSize size,
                     Register rd,
                     Register rn,
                     const Operand &operand);

            virtual void sub(Condition cond, Register rd, const Operand &operand);

            virtual void subs(Condition cond,
                      EncodingSize size,
                      Register rd,
                      Register rn,
                      const Operand &operand);

            virtual void subs(Register rd, const Operand &operand);

            virtual void subw(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void svc(Condition cond, uint32_t imm);

            virtual void sxtab(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void sxtab16(Condition cond,
                         Register rd,
                         Register rn,
                         const Operand &operand);

            virtual void sxtah(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void sxtb(Condition cond,
                      EncodingSize size,
                      Register rd,
                      const Operand &operand);

            virtual void sxtb16(Condition cond, Register rd, const Operand &operand);

            virtual void sxth(Condition cond,
                      EncodingSize size,
                      Register rd,
                      const Operand &operand);

            virtual void tbb(Condition cond, Register rn, Register rm);

            virtual void tbh(Condition cond, Register rn, Register rm);

            virtual void teq(Condition cond, Register rn, const Operand &operand);

            virtual void tst(Condition cond,
                     EncodingSize size,
                     Register rn,
                     const Operand &operand);

            virtual void uadd16(Condition cond, Register rd, Register rn, Register rm);

            virtual void uadd8(Condition cond, Register rd, Register rn, Register rm);

            virtual void uasx(Condition cond, Register rd, Register rn, Register rm);

            virtual void ubfx(
                    Condition cond, Register rd, Register rn, uint32_t lsb, uint32_t width);

            virtual void udf(Condition cond, EncodingSize size, uint32_t imm);

            virtual void udiv(Condition cond, Register rd, Register rn, Register rm);

            virtual void uhadd16(Condition cond, Register rd, Register rn, Register rm);

            virtual void uhadd8(Condition cond, Register rd, Register rn, Register rm);

            virtual void uhasx(Condition cond, Register rd, Register rn, Register rm);

            virtual void uhsax(Condition cond, Register rd, Register rn, Register rm);

            virtual void uhsub16(Condition cond, Register rd, Register rn, Register rm);

            virtual void uhsub8(Condition cond, Register rd, Register rn, Register rm);

            virtual void umaal(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void umlal(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void umlals(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void umull(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void umulls(
                    Condition cond, Register rdlo, Register rdhi, Register rn, Register rm);

            virtual void uqadd16(Condition cond, Register rd, Register rn, Register rm);

            virtual void uqadd8(Condition cond, Register rd, Register rn, Register rm);

            virtual void uqasx(Condition cond, Register rd, Register rn, Register rm);

            virtual void uqsax(Condition cond, Register rd, Register rn, Register rm);

            virtual void uqsub16(Condition cond, Register rd, Register rn, Register rm);

            virtual void uqsub8(Condition cond, Register rd, Register rn, Register rm);

            virtual void usad8(Condition cond, Register rd, Register rn, Register rm);

            virtual void usada8(
                    Condition cond, Register rd, Register rn, Register rm, Register ra);

            virtual void usat(Condition cond, Register rd, uint32_t imm, const Operand &operand);

            virtual void usat16(Condition cond, Register rd, uint32_t imm, Register rn);

            virtual void usax(Condition cond, Register rd, Register rn, Register rm);

            virtual void usub16(Condition cond, Register rd, Register rn, Register rm);

            virtual void usub8(Condition cond, Register rd, Register rn, Register rm);

            virtual void uxtab(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void uxtab16(Condition cond,
                         Register rd,
                         Register rn,
                         const Operand &operand);

            virtual void uxtah(Condition cond, Register rd, Register rn, const Operand &operand);

            virtual void uxtb(Condition cond,
                      EncodingSize size,
                      Register rd,
                      const Operand &operand);

            virtual void uxtb16(Condition cond, Register rd, const Operand &operand);

            virtual void uxth(Condition cond,
                      EncodingSize size,
                      Register rd,
                      const Operand &operand);

            virtual void vaba(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vaba(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vabal(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vabd(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vabd(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vabdl(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vabs(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vabs(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vabs(Condition cond, DataType dt, SRegister rd, SRegister rm);

            virtual void vacge(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vacge(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vacgt(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vacgt(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vacle(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vacle(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vaclt(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vaclt(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vadd(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vadd(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vadd(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vaddhn(
                    Condition cond, DataType dt, DRegister rd, QRegister rn, QRegister rm);

            virtual void vaddl(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vaddw(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, DRegister rm);

            virtual void vand(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      const DOperand &operand);

            virtual void vand(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      const QOperand &operand);

            virtual void vbic(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      const DOperand &operand);

            virtual void vbic(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      const QOperand &operand);

            virtual void vbif(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vbif(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vbit(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vbit(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vbsl(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vbsl(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vceq(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vceq(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vceq(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vceq(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vcge(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vcge(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vcge(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vcge(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vcgt(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vcgt(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vcgt(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vcgt(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vcle(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vcle(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vcle(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vcle(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vcls(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vcls(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vclt(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vclt(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vclt(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vclt(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vclz(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vclz(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vcmp(Condition cond, DataType dt, SRegister rd, const SOperand &operand);

            virtual void vcmp(Condition cond, DataType dt, DRegister rd, const DOperand &operand);

            virtual void vcmpe(Condition cond,
                       DataType dt,
                       SRegister rd,
                       const SOperand &operand);

            virtual void vcmpe(Condition cond,
                       DataType dt,
                       DRegister rd,
                       const DOperand &operand);

            virtual void vcnt(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vcnt(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, DRegister rd, SRegister rm);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvt(Condition cond,
                      DataType dt1,
                      DataType dt2,
                      DRegister rd,
                      DRegister rm,
                      int32_t fbits);

            virtual void vcvt(Condition cond,
                      DataType dt1,
                      DataType dt2,
                      QRegister rd,
                      QRegister rm,
                      int32_t fbits);

            virtual void vcvt(Condition cond,
                      DataType dt1,
                      DataType dt2,
                      SRegister rd,
                      SRegister rm,
                      int32_t fbits);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, DRegister rd, DRegister rm);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, QRegister rd, QRegister rm);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, DRegister rd, QRegister rm);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, QRegister rd, DRegister rm);

            virtual void vcvt(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvta(DataType dt1, DataType dt2, DRegister rd, DRegister rm);

            virtual void vcvta(DataType dt1, DataType dt2, QRegister rd, QRegister rm);

            virtual void vcvta(DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvta(DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvtb(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvtb(
                    Condition cond, DataType dt1, DataType dt2, DRegister rd, SRegister rm);

            virtual void vcvtb(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvtm(DataType dt1, DataType dt2, DRegister rd, DRegister rm);

            virtual void vcvtm(DataType dt1, DataType dt2, QRegister rd, QRegister rm);

            virtual void vcvtm(DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvtm(DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvtn(DataType dt1, DataType dt2, DRegister rd, DRegister rm);

            virtual void vcvtn(DataType dt1, DataType dt2, QRegister rd, QRegister rm);

            virtual void vcvtn(DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvtn(DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvtp(DataType dt1, DataType dt2, DRegister rd, DRegister rm);

            virtual void vcvtp(DataType dt1, DataType dt2, QRegister rd, QRegister rm);

            virtual void vcvtp(DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvtp(DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvtr(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvtr(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vcvtt(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, SRegister rm);

            virtual void vcvtt(
                    Condition cond, DataType dt1, DataType dt2, DRegister rd, SRegister rm);

            virtual void vcvtt(
                    Condition cond, DataType dt1, DataType dt2, SRegister rd, DRegister rm);

            virtual void vdiv(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vdiv(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vdup(Condition cond, DataType dt, QRegister rd, Register rt);

            virtual void vdup(Condition cond, DataType dt, DRegister rd, Register rt);

            virtual void vdup(Condition cond, DataType dt, DRegister rd, DRegisterLane rm);

            virtual void vdup(Condition cond, DataType dt, QRegister rd, DRegisterLane rm);

            virtual void veor(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void veor(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vext(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vext(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vfma(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vfma(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vfma(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vfms(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vfms(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vfms(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vfnma(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vfnma(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vfnms(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vfnms(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vhadd(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vhadd(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vhsub(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vhsub(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vld1(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vld2(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vld3(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vld3(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const MemOperand &operand);

            virtual void vld4(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vldm(Condition cond,
                      DataType dt,
                      Register rn,
                      WriteBack write_back,
                      DRegisterList dreglist);

            virtual void vldm(Condition cond,
                      DataType dt,
                      Register rn,
                      WriteBack write_back,
                      SRegisterList sreglist);

            virtual void vldmdb(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        DRegisterList dreglist);

            virtual void vldmdb(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        SRegisterList sreglist);

            virtual void vldmia(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        DRegisterList dreglist);

            virtual void vldmia(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        SRegisterList sreglist);

            virtual void vldr(Condition cond, DataType dt, DRegister rd, Location *location);

            virtual void vldr(Condition cond,
                      DataType dt,
                      DRegister rd,
                      const MemOperand &operand);

            virtual void vldr(Condition cond, DataType dt, SRegister rd, Location *location);

            virtual void vldr(Condition cond,
                      DataType dt,
                      SRegister rd,
                      const MemOperand &operand);

            virtual void vmax(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vmax(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vmaxnm(DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vmaxnm(DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vmaxnm(DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vmin(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vmin(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vminnm(DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vminnm(DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vminnm(DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vmla(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      DRegisterLane rm);

            virtual void vmla(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      DRegisterLane rm);

            virtual void vmla(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vmla(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vmla(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vmlal(Condition cond,
                       DataType dt,
                       QRegister rd,
                       DRegister rn,
                       DRegisterLane rm);

            virtual void vmlal(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vmls(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      DRegisterLane rm);

            virtual void vmls(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      DRegisterLane rm);

            virtual void vmls(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vmls(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vmls(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vmlsl(Condition cond,
                       DataType dt,
                       QRegister rd,
                       DRegister rn,
                       DRegisterLane rm);

            virtual void vmlsl(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vmov(Condition cond, Register rt, SRegister rn);

            virtual void vmov(Condition cond, SRegister rn, Register rt);

            virtual void vmov(Condition cond, Register rt, Register rt2, DRegister rm);

            virtual void vmov(Condition cond, DRegister rm, Register rt, Register rt2);

            virtual void vmov(
                    Condition cond, Register rt, Register rt2, SRegister rm, SRegister rm1);

            virtual void vmov(
                    Condition cond, SRegister rm, SRegister rm1, Register rt, Register rt2);

            virtual void vmov(Condition cond, DataType dt, DRegisterLane rd, Register rt);

            virtual void vmov(Condition cond, DataType dt, DRegister rd, const DOperand &operand);

            virtual void vmov(Condition cond, DataType dt, QRegister rd, const QOperand &operand);

            virtual void vmov(Condition cond, DataType dt, SRegister rd, const SOperand &operand);

            virtual void vmov(Condition cond, DataType dt, Register rt, DRegisterLane rn);

            virtual void vmovl(Condition cond, DataType dt, QRegister rd, DRegister rm);

            virtual void vmovn(Condition cond, DataType dt, DRegister rd, QRegister rm);

            virtual void vmrs(Condition cond, RegisterOrAPSR_nzcv rt, SpecialFPRegister spec_reg);

            virtual void vmsr(Condition cond, SpecialFPRegister spec_reg, Register rt);

            virtual void vmul(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      DRegister dm,
                      unsigned index);

            virtual void vmul(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      DRegister dm,
                      unsigned index);

            virtual void vmul(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vmul(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vmul(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vmull(Condition cond,
                       DataType dt,
                       QRegister rd,
                       DRegister rn,
                       DRegister dm,
                       unsigned index);

            virtual void vmull(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vmvn(Condition cond, DataType dt, DRegister rd, const DOperand &operand);

            virtual void vmvn(Condition cond, DataType dt, QRegister rd, const QOperand &operand);

            virtual void vneg(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vneg(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vneg(Condition cond, DataType dt, SRegister rd, SRegister rm);

            virtual void vnmla(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vnmla(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vnmls(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vnmls(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vnmul(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vnmul(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vorn(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      const DOperand &operand);

            virtual void vorn(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      const QOperand &operand);

            virtual void vorr(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rn,
                      const DOperand &operand);

            virtual void vorr(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rn,
                      const QOperand &operand);

            virtual void vpadal(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vpadal(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vpadd(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vpaddl(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vpaddl(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vpmax(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vpmin(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vpop(Condition cond, DataType dt, DRegisterList dreglist);

            virtual void vpop(Condition cond, DataType dt, SRegisterList sreglist);

            virtual void vpush(Condition cond, DataType dt, DRegisterList dreglist);

            virtual void vpush(Condition cond, DataType dt, SRegisterList sreglist);

            virtual void vqabs(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vqabs(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vqadd(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vqadd(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vqdmlal(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vqdmlal(Condition cond,
                         DataType dt,
                         QRegister rd,
                         DRegister rn,
                         DRegister dm,
                         unsigned index);

            virtual void vqdmlsl(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vqdmlsl(Condition cond,
                         DataType dt,
                         QRegister rd,
                         DRegister rn,
                         DRegister dm,
                         unsigned index);

            virtual void vqdmulh(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vqdmulh(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vqdmulh(Condition cond,
                         DataType dt,
                         DRegister rd,
                         DRegister rn,
                         DRegisterLane rm);

            virtual void vqdmulh(Condition cond,
                         DataType dt,
                         QRegister rd,
                         QRegister rn,
                         DRegisterLane rm);

            virtual void vqdmull(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vqdmull(Condition cond,
                         DataType dt,
                         QRegister rd,
                         DRegister rn,
                         DRegisterLane rm);

            virtual void vqmovn(Condition cond, DataType dt, DRegister rd, QRegister rm);

            virtual void vqmovun(Condition cond, DataType dt, DRegister rd, QRegister rm);

            virtual void vqneg(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vqneg(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vqrdmulh(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vqrdmulh(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vqrdmulh(Condition cond,
                          DataType dt,
                          DRegister rd,
                          DRegister rn,
                          DRegisterLane rm);

            virtual void vqrdmulh(Condition cond,
                          DataType dt,
                          QRegister rd,
                          QRegister rn,
                          DRegisterLane rm);

            virtual void vqrshl(
                    Condition cond, DataType dt, DRegister rd, DRegister rm, DRegister rn);

            virtual void vqrshl(
                    Condition cond, DataType dt, QRegister rd, QRegister rm, QRegister rn);

            virtual void vqrshrn(Condition cond,
                         DataType dt,
                         DRegister rd,
                         QRegister rm,
                         const QOperand &operand);

            virtual void vqrshrun(Condition cond,
                          DataType dt,
                          DRegister rd,
                          QRegister rm,
                          const QOperand &operand);

            virtual void vqshl(Condition cond,
                       DataType dt,
                       DRegister rd,
                       DRegister rm,
                       const DOperand &operand);

            virtual void vqshl(Condition cond,
                       DataType dt,
                       QRegister rd,
                       QRegister rm,
                       const QOperand &operand);

            virtual void vqshlu(Condition cond,
                        DataType dt,
                        DRegister rd,
                        DRegister rm,
                        const DOperand &operand);

            virtual void vqshlu(Condition cond,
                        DataType dt,
                        QRegister rd,
                        QRegister rm,
                        const QOperand &operand);

            virtual void vqshrn(Condition cond,
                        DataType dt,
                        DRegister rd,
                        QRegister rm,
                        const QOperand &operand);

            virtual void vqshrun(Condition cond,
                         DataType dt,
                         DRegister rd,
                         QRegister rm,
                         const QOperand &operand);

            virtual void vqsub(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vqsub(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vraddhn(
                    Condition cond, DataType dt, DRegister rd, QRegister rn, QRegister rm);

            virtual void vrecpe(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrecpe(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vrecps(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vrecps(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vrev16(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrev16(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vrev32(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrev32(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vrev64(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrev64(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vrhadd(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vrhadd(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vrinta(DataType dt, DRegister rd, DRegister rm);

            virtual void vrinta(DataType dt, QRegister rd, QRegister rm);

            virtual void vrinta(DataType dt, SRegister rd, SRegister rm);

            virtual void vrintm(DataType dt, DRegister rd, DRegister rm);

            virtual void vrintm(DataType dt, QRegister rd, QRegister rm);

            virtual void vrintm(DataType dt, SRegister rd, SRegister rm);

            virtual void vrintn(DataType dt, DRegister rd, DRegister rm);

            virtual void vrintn(DataType dt, QRegister rd, QRegister rm);

            virtual void vrintn(DataType dt, SRegister rd, SRegister rm);

            virtual void vrintp(DataType dt, DRegister rd, DRegister rm);

            virtual void vrintp(DataType dt, QRegister rd, QRegister rm);

            virtual void vrintp(DataType dt, SRegister rd, SRegister rm);

            virtual void vrintr(Condition cond, DataType dt, SRegister rd, SRegister rm);

            virtual void vrintr(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrintx(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrintx(DataType dt, QRegister rd, QRegister rm);

            virtual void vrintx(Condition cond, DataType dt, SRegister rd, SRegister rm);

            virtual void vrintz(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrintz(DataType dt, QRegister rd, QRegister rm);

            virtual void vrintz(Condition cond, DataType dt, SRegister rd, SRegister rm);

            virtual void vrshl(
                    Condition cond, DataType dt, DRegister rd, DRegister rm, DRegister rn);

            virtual void vrshl(
                    Condition cond, DataType dt, QRegister rd, QRegister rm, QRegister rn);

            virtual void vrshr(Condition cond,
                       DataType dt,
                       DRegister rd,
                       DRegister rm,
                       const DOperand &operand);

            virtual void vrshr(Condition cond,
                       DataType dt,
                       QRegister rd,
                       QRegister rm,
                       const QOperand &operand);

            virtual void vrshrn(Condition cond,
                        DataType dt,
                        DRegister rd,
                        QRegister rm,
                        const QOperand &operand);

            virtual void vrsqrte(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vrsqrte(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vrsqrts(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vrsqrts(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vrsra(Condition cond,
                       DataType dt,
                       DRegister rd,
                       DRegister rm,
                       const DOperand &operand);

            virtual void vrsra(Condition cond,
                       DataType dt,
                       QRegister rd,
                       QRegister rm,
                       const QOperand &operand);

            virtual void vrsubhn(
                    Condition cond, DataType dt, DRegister rd, QRegister rn, QRegister rm);

            virtual void vseleq(DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vseleq(DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vselge(DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vselge(DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vselgt(DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vselgt(DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vselvs(DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vselvs(DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vshl(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vshl(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vshll(Condition cond,
                       DataType dt,
                       QRegister rd,
                       DRegister rm,
                       const DOperand &operand);

            virtual void vshr(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vshr(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vshrn(Condition cond,
                       DataType dt,
                       DRegister rd,
                       QRegister rm,
                       const QOperand &operand);

            virtual void vsli(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vsli(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vsqrt(Condition cond, DataType dt, SRegister rd, SRegister rm);

            virtual void vsqrt(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vsra(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vsra(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vsri(Condition cond,
                      DataType dt,
                      DRegister rd,
                      DRegister rm,
                      const DOperand &operand);

            virtual void vsri(Condition cond,
                      DataType dt,
                      QRegister rd,
                      QRegister rm,
                      const QOperand &operand);

            virtual void vst1(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vst2(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vst3(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vst3(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const MemOperand &operand);

            virtual void vst4(Condition cond,
                      DataType dt,
                      const NeonRegisterList &nreglist,
                      const AlignedMemOperand &operand);

            virtual void vstm(Condition cond,
                      DataType dt,
                      Register rn,
                      WriteBack write_back,
                      DRegisterList dreglist);

            virtual void vstm(Condition cond,
                      DataType dt,
                      Register rn,
                      WriteBack write_back,
                      SRegisterList sreglist);

            virtual void vstmdb(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        DRegisterList dreglist);

            virtual void vstmdb(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        SRegisterList sreglist);

            virtual void vstmia(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        DRegisterList dreglist);

            virtual void vstmia(Condition cond,
                        DataType dt,
                        Register rn,
                        WriteBack write_back,
                        SRegisterList sreglist);

            virtual void vstr(Condition cond,
                      DataType dt,
                      DRegister rd,
                      const MemOperand &operand);

            virtual void vstr(Condition cond,
                      DataType dt,
                      SRegister rd,
                      const MemOperand &operand);

            virtual void vsub(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vsub(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vsub(
                    Condition cond, DataType dt, SRegister rd, SRegister rn, SRegister rm);

            virtual void vsubhn(
                    Condition cond, DataType dt, DRegister rd, QRegister rn, QRegister rm);

            virtual void vsubl(
                    Condition cond, DataType dt, QRegister rd, DRegister rn, DRegister rm);

            virtual void vsubw(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, DRegister rm);

            virtual void vswp(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vswp(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vtbl(Condition cond,
                      DataType dt,
                      DRegister rd,
                      const NeonRegisterList &nreglist,
                      DRegister rm);

            virtual void vtbx(Condition cond,
                      DataType dt,
                      DRegister rd,
                      const NeonRegisterList &nreglist,
                      DRegister rm);

            virtual void vtrn(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vtrn(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vtst(
                    Condition cond, DataType dt, DRegister rd, DRegister rn, DRegister rm);

            virtual void vtst(
                    Condition cond, DataType dt, QRegister rd, QRegister rn, QRegister rm);

            virtual void vuzp(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vuzp(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void vzip(Condition cond, DataType dt, DRegister rd, DRegister rm);

            virtual void vzip(Condition cond, DataType dt, QRegister rd, QRegister rm);

            virtual void yield(Condition cond, EncodingSize size);

            virtual int T32Size(uint32_t instr);

            void DecodeT32(uint32_t instr);

            void DecodeA32(uint32_t instr);
        };

        DataTypeValue Dt_L_imm6_1_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_L_imm6_2_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_L_imm6_3_Decode(uint32_t value);

        DataTypeValue Dt_L_imm6_4_Decode(uint32_t value);

        DataTypeValue Dt_imm6_1_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_imm6_2_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_imm6_3_Decode(uint32_t value);

        DataTypeValue Dt_imm6_4_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_op_U_size_1_Decode(uint32_t value);

        DataTypeValue Dt_op_size_1_Decode(uint32_t value);

        DataTypeValue Dt_op_size_2_Decode(uint32_t value);

        DataTypeValue Dt_op_size_3_Decode(uint32_t value);

        DataTypeValue Dt_U_imm3H_1_Decode(uint32_t value);

        DataTypeValue Dt_U_opc1_opc2_1_Decode(uint32_t value, unsigned *lane);

        DataTypeValue Dt_opc1_opc2_1_Decode(uint32_t value, unsigned *lane);

        DataTypeValue Dt_imm4_1_Decode(uint32_t value, unsigned *lane);

        DataTypeValue Dt_B_E_1_Decode(uint32_t value);

        DataTypeValue Dt_op_1_Decode1(uint32_t value);

        DataTypeValue Dt_op_1_Decode2(uint32_t value);

        DataTypeValue Dt_op_2_Decode(uint32_t value);

        DataTypeValue Dt_op_3_Decode(uint32_t value);

        DataTypeValue Dt_U_sx_1_Decode(uint32_t value);

        DataTypeValue Dt_op_U_1_Decode1(uint32_t value);

        DataTypeValue Dt_op_U_1_Decode2(uint32_t value);

        DataTypeValue Dt_sz_1_Decode(uint32_t value);

        DataTypeValue Dt_F_size_1_Decode(uint32_t value);

        DataTypeValue Dt_F_size_2_Decode(uint32_t value);

        DataTypeValue Dt_F_size_3_Decode(uint32_t value);

        DataTypeValue Dt_F_size_4_Decode(uint32_t value);

        DataTypeValue Dt_U_size_1_Decode(uint32_t value);

        DataTypeValue Dt_U_size_2_Decode(uint32_t value);

        DataTypeValue Dt_U_size_3_Decode(uint32_t value);

        DataTypeValue Dt_size_1_Decode(uint32_t value);

        DataTypeValue Dt_size_2_Decode(uint32_t value);

        DataTypeValue Dt_size_3_Decode(uint32_t value);

        DataTypeValue Dt_size_4_Decode(uint32_t value);

        DataTypeValue Dt_size_5_Decode(uint32_t value);

        DataTypeValue Dt_size_6_Decode(uint32_t value);

        DataTypeValue Dt_size_7_Decode(uint32_t value);

        DataTypeValue Dt_size_8_Decode(uint32_t value);

        DataTypeValue Dt_size_9_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_size_10_Decode(uint32_t value);

        DataTypeValue Dt_size_11_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_size_12_Decode(uint32_t value, uint32_t type_value);

        DataTypeValue Dt_size_13_Decode(uint32_t value);

        DataTypeValue Dt_size_14_Decode(uint32_t value);

        DataTypeValue Dt_size_15_Decode(uint32_t value);

        DataTypeValue Dt_size_16_Decode(uint32_t value);

        DataTypeValue Dt_size_17_Decode(uint32_t value);
// End of generated code.

        class PrintDisassembler : public Disassembler {
        public:
            explicit PrintDisassembler(std::ostream &os,  // NOLINT(runtime/references)
                                       uint32_t code_address = 0)
                    : Disassembler(os, code_address) {}

            explicit PrintDisassembler(DisassemblerStream *os, uint32_t code_address = 0)
                    : Disassembler(os, code_address) {}

            virtual void PrintCodeAddress(uint32_t code_address) {
                os() << "0x" << std::hex << std::setw(8) << std::setfill('0')
                     << code_address << "\t";
            }

            virtual void PrintOpcode16(uint32_t opcode) {
                os() << std::hex << std::setw(4) << std::setfill('0') << opcode << "    "
                     << std::dec << "\t";
            }

            virtual void PrintOpcode32(uint32_t opcode) {
                os() << std::hex << std::setw(8) << std::setfill('0') << opcode << std::dec
                     << "\t";
            }

            const uint32_t *DecodeA32At(const uint32_t *instruction_address) {
                DecodeA32(*instruction_address);
                return instruction_address + 1;
            }

            // Returns the address of the next instruction.
            const uint16_t *DecodeT32At(const uint16_t *instruction_address,
                                        const uint16_t *buffer_end);

            void DecodeT32(uint32_t instruction);

            void DecodeA32(uint32_t instruction);

            void DisassembleA32Buffer(const uint32_t *buffer, size_t size_in_bytes);

            void DisassembleT32Buffer(const uint16_t *buffer, size_t size_in_bytes);
        };

    }  // namespace aarch32
}  // namespace vixl

#endif  // VIXL_DISASM_AARCH32_H_
