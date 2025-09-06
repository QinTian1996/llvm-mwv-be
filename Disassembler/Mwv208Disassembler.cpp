//===- Mwv208Disassembler.cpp - Disassembler for Mwv208 -----------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is part of the Mwv208 Disassembler.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Mwv208MCTargetDesc.h"
#include "TargetInfo/Mwv208TargetInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "mwv208-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// A disassembler class for Mwv208.
class Mwv208Disassembler : public MCDisassembler {
public:
  Mwv208Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  virtual ~Mwv208Disassembler() = default;

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
} // namespace

static MCDisassembler *createMwv208Disassembler(const Target &T,
                                                const MCSubtargetInfo &STI,
                                                MCContext &Ctx) {
  return new Mwv208Disassembler(STI, Ctx);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMwv208Disassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(getTheMwv208Target(),
                                         createMwv208Disassembler);
  TargetRegistry::RegisterMCDisassembler(getTheMwv208V9Target(),
                                         createMwv208Disassembler);
  TargetRegistry::RegisterMCDisassembler(getTheMwv208elTarget(),
                                         createMwv208Disassembler);
}

static const unsigned IntRegDecoderTable[] = {
    JJ::G0, JJ::G1, JJ::G2, JJ::G3, JJ::G4, JJ::G5, JJ::G6, JJ::G7,
    JJ::O0, JJ::O1, JJ::O2, JJ::O3, JJ::O4, JJ::O5, JJ::O6, JJ::O7,
    JJ::L0, JJ::L1, JJ::L2, JJ::L3, JJ::L4, JJ::L5, JJ::L6, JJ::L7,
    JJ::I0, JJ::I1, JJ::I2, JJ::I3, JJ::I4, JJ::I5, JJ::I6, JJ::I7};

static const unsigned FPRegDecoderTable[] = {
    JJ::F0,  JJ::F1,  JJ::F2,  JJ::F3,  JJ::F4,  JJ::F5,  JJ::F6,  JJ::F7,
    JJ::F8,  JJ::F9,  JJ::F10, JJ::F11, JJ::F12, JJ::F13, JJ::F14, JJ::F15,
    JJ::F16, JJ::F17, JJ::F18, JJ::F19, JJ::F20, JJ::F21, JJ::F22, JJ::F23,
    JJ::F24, JJ::F25, JJ::F26, JJ::F27, JJ::F28, JJ::F29, JJ::F30, JJ::F31};

static const unsigned DFPRegDecoderTable[] = {
    JJ::D0,  JJ::D16, JJ::D1,  JJ::D17, JJ::D2,  JJ::D18, JJ::D3,  JJ::D19,
    JJ::D4,  JJ::D20, JJ::D5,  JJ::D21, JJ::D6,  JJ::D22, JJ::D7,  JJ::D23,
    JJ::D8,  JJ::D24, JJ::D9,  JJ::D25, JJ::D10, JJ::D26, JJ::D11, JJ::D27,
    JJ::D12, JJ::D28, JJ::D13, JJ::D29, JJ::D14, JJ::D30, JJ::D15, JJ::D31};

static const unsigned QFPRegDecoderTable[] = {
    JJ::Q0, JJ::Q8,  ~0U, ~0U, JJ::Q1, JJ::Q9,  ~0U, ~0U,
    JJ::Q2, JJ::Q10, ~0U, ~0U, JJ::Q3, JJ::Q11, ~0U, ~0U,
    JJ::Q4, JJ::Q12, ~0U, ~0U, JJ::Q5, JJ::Q13, ~0U, ~0U,
    JJ::Q6, JJ::Q14, ~0U, ~0U, JJ::Q7, JJ::Q15, ~0U, ~0U};

static const unsigned FCCRegDecoderTable[] = {JJ::FCC0, JJ::FCC1, JJ::FCC2,
                                              JJ::FCC3};

static const unsigned ASRRegDecoderTable[] = {
    JJ::Y,     JJ::ASR1,  JJ::ASR2,  JJ::ASR3,  JJ::ASR4,  JJ::ASR5,  JJ::ASR6,
    JJ::ASR7,  JJ::ASR8,  JJ::ASR9,  JJ::ASR10, JJ::ASR11, JJ::ASR12, JJ::ASR13,
    JJ::ASR14, JJ::ASR15, JJ::ASR16, JJ::ASR17, JJ::ASR18, JJ::ASR19, JJ::ASR20,
    JJ::ASR21, JJ::ASR22, JJ::ASR23, JJ::ASR24, JJ::ASR25, JJ::ASR26, JJ::ASR27,
    JJ::ASR28, JJ::ASR29, JJ::ASR30, JJ::ASR31};

static const unsigned PRRegDecoderTable[] = {
    JJ::TPC,     JJ::TNPC,       JJ::TSTATE,   JJ::TT,       JJ::TICK,
    JJ::TBA,     JJ::PSTATE,     JJ::TL,       JJ::PIL,      JJ::CWP,
    JJ::CANSAVE, JJ::CANRESTORE, JJ::CLEANWIN, JJ::OTHERWIN, JJ::WSTATE};

static const uint16_t IntPairDecoderTable[] = {
    JJ::G0_G1, JJ::G2_G3, JJ::G4_G5, JJ::G6_G7, JJ::O0_O1, JJ::O2_O3,
    JJ::O4_O5, JJ::O6_O7, JJ::L0_L1, JJ::L2_L3, JJ::L4_L5, JJ::L6_L7,
    JJ::I0_I1, JJ::I2_I3, JJ::I4_I5, JJ::I6_I7,
};

static const unsigned CPRegDecoderTable[] = {
    JJ::C0,  JJ::C1,  JJ::C2,  JJ::C3,  JJ::C4,  JJ::C5,  JJ::C6,  JJ::C7,
    JJ::C8,  JJ::C9,  JJ::C10, JJ::C11, JJ::C12, JJ::C13, JJ::C14, JJ::C15,
    JJ::C16, JJ::C17, JJ::C18, JJ::C19, JJ::C20, JJ::C21, JJ::C22, JJ::C23,
    JJ::C24, JJ::C25, JJ::C26, JJ::C27, JJ::C28, JJ::C29, JJ::C30, JJ::C31};

static const uint16_t CPPairDecoderTable[] = {
    JJ::C0_C1,   JJ::C2_C3,   JJ::C4_C5,   JJ::C6_C7,
    JJ::C8_C9,   JJ::C10_C11, JJ::C12_C13, JJ::C14_C15,
    JJ::C16_C17, JJ::C18_C19, JJ::C20_C21, JJ::C22_C23,
    JJ::C24_C25, JJ::C26_C27, JJ::C28_C29, JJ::C30_C31};

static DecodeStatus DecodeIntRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  unsigned Reg = IntRegDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeI64RegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  return DecodeIntRegsRegisterClass(Inst, RegNo, Address, Decoder);
}

// This is used for the type "ptr_rc", which is either IntRegs or I64Regs
// depending on Mwv208RegisterInfo::getPointerRegClass.
static DecodeStatus DecodePointerLikeRegClass0(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  return DecodeIntRegsRegisterClass(Inst, RegNo, Address, Decoder);
}

static DecodeStatus DecodeFPRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  unsigned Reg = FPRegDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeDFPRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  unsigned Reg = DFPRegDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeQFPRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;

  unsigned Reg = QFPRegDecoderTable[RegNo];
  if (Reg == ~0U)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus
DecodeCoprocRegsRegisterClass(MCInst &Inst, unsigned RegNo, uint64_t Address,
                              const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  unsigned Reg = CPRegDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeFCCRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  if (RegNo > 3)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(FCCRegDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeASRRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(ASRRegDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

static DecodeStatus DecodePRRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  if (RegNo >= std::size(PRRegDecoderTable))
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(PRRegDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeIntPairRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  DecodeStatus S = MCDisassembler::Success;

  if (RegNo > 31)
    return MCDisassembler::Fail;

  if ((RegNo & 1))
    S = MCDisassembler::SoftFail;

  unsigned RegisterPair = IntPairDecoderTable[RegNo / 2];
  Inst.addOperand(MCOperand::createReg(RegisterPair));
  return S;
}

static DecodeStatus
DecodeCoprocPairRegisterClass(MCInst &Inst, unsigned RegNo, uint64_t Address,
                              const MCDisassembler *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;

  unsigned RegisterPair = CPPairDecoderTable[RegNo / 2];
  Inst.addOperand(MCOperand::createReg(RegisterPair));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeCall(MCInst &Inst, unsigned insn, uint64_t Address,
                               const MCDisassembler *Decoder);
static DecodeStatus DecodeSIMM13(MCInst &Inst, unsigned insn, uint64_t Address,
                                 const MCDisassembler *Decoder);

#include "Mwv208GenDisassemblerTables.inc"

/// Read four bytes from the ArrayRef and return 32 bit word.
static DecodeStatus readInstruction32(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint32_t &Insn,
                                      bool IsLittleEndian) {
  // We want to read exactly 4 Bytes of data.
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Insn = IsLittleEndian ? (Bytes[0] << 0) | (Bytes[1] << 8) | (Bytes[2] << 16) |
                              (Bytes[3] << 24)
                        : (Bytes[3] << 0) | (Bytes[2] << 8) | (Bytes[1] << 16) |
                              (Bytes[0] << 24);

  return MCDisassembler::Success;
}

DecodeStatus Mwv208Disassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                                ArrayRef<uint8_t> Bytes,
                                                uint64_t Address,
                                                raw_ostream &CStream) const {
  uint32_t Insn;
  bool isLittleEndian = getContext().getAsmInfo()->isLittleEndian();
  DecodeStatus Result =
      readInstruction32(Bytes, Address, Size, Insn, isLittleEndian);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Calling the auto-generated decoder function.

  if (STI.hasFeature(Mwv208::FeatureV9)) {
    Result = decodeInstruction(DecoderTableMwv208V932, Instr, Insn, Address,
                               this, STI);
  } else {
    Result = decodeInstruction(DecoderTableMwv208V832, Instr, Insn, Address,
                               this, STI);
  }
  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  Result =
      decodeInstruction(DecoderTableMwv20832, Instr, Insn, Address, this, STI);

  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  return MCDisassembler::Fail;
}

static bool tryAddingSymbolicOperand(int64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t Width, MCInst &MI,
                                     const MCDisassembler *Decoder) {
  return Decoder->tryAddingSymbolicOperand(MI, Value, Address, isBranch, Offset,
                                           Width, /*InstSize=*/4);
}

static DecodeStatus DecodeCall(MCInst &MI, unsigned insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  unsigned tgt = fieldFromInstruction(insn, 0, 30);
  tgt <<= 2;
  if (!tryAddingSymbolicOperand(tgt + Address, false, Address, 0, 30, MI,
                                Decoder))
    MI.addOperand(MCOperand::createImm(tgt));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeSIMM13(MCInst &MI, unsigned insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {
  assert(isUInt<13>(insn));
  MI.addOperand(MCOperand::createImm(SignExtend64<13>(insn)));
  return MCDisassembler::Success;
}
