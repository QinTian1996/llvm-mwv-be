//===-- Mwv208RegisterInfo.cpp - MWV208 Register Information
//----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the MWV208 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "Mwv208RegisterInfo.h"
#include "Mwv208.h"
#include "Mwv208Subtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "Mwv208GenRegisterInfo.inc"

static cl::opt<bool>
    ReserveAppRegisters("mwv208-reserve-app-registers", cl::Hidden,
                        cl::init(false),
                        cl::desc("Reserve application registers (%g2-%g4)"));

Mwv208RegisterInfo::Mwv208RegisterInfo() : Mwv208GenRegisterInfo(JJ::O7) {}

const MCPhysReg *
Mwv208RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

const uint32_t *
Mwv208RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                         CallingConv::ID CC) const {
  return CSR_RegMask;
}

const uint32_t *
Mwv208RegisterInfo::getRTCallPreservedMask(CallingConv::ID CC) const {
  return RTCSR_RegMask;
}

BitVector Mwv208RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  const Mwv208Subtarget &Subtarget = MF.getSubtarget<Mwv208Subtarget>();
  // FIXME: G1 reserved for now for large imm generation by frame code.
  Reserved.set(JJ::G1);

  // G1-G4 can be used in applications.
  if (ReserveAppRegisters) {
    Reserved.set(JJ::G2);
    Reserved.set(JJ::G3);
    Reserved.set(JJ::G4);
  }
  // G5 is not reserved in 64 bit mode.
  if (!Subtarget.is64Bit())
    Reserved.set(JJ::G5);

  Reserved.set(JJ::O6);
  Reserved.set(JJ::I6);
  Reserved.set(JJ::I7);
  Reserved.set(JJ::G0);
  Reserved.set(JJ::G6);
  Reserved.set(JJ::G7);

  // Also reserve the register pair aliases covering the above
  // registers, with the same conditions.
  Reserved.set(JJ::G0_G1);
  if (ReserveAppRegisters)
    Reserved.set(JJ::G2_G3);
  if (ReserveAppRegisters || !Subtarget.is64Bit())
    Reserved.set(JJ::G4_G5);

  Reserved.set(JJ::O6_O7);
  Reserved.set(JJ::I6_I7);
  Reserved.set(JJ::G6_G7);

  // Unaliased double registers are not available in non-V9 targets.
  if (!Subtarget.isV9()) {
    for (unsigned n = 0; n != 16; ++n) {
      for (MCRegAliasIterator AI(JJ::D16 + n, this, true); AI.isValid(); ++AI)
        Reserved.set(*AI);
    }
  }

  // Reserve ASR1-ASR31
  for (unsigned n = 0; n < 31; n++)
    Reserved.set(JJ::ASR1 + n);

  for (TargetRegisterClass::iterator i = JJ::IntRegsRegClass.begin();
       i != JJ::IntRegsRegClass.end(); ++i) {
    if (MF.getSubtarget<Mwv208Subtarget>().isRegisterReserved(*i))
      markSuperRegs(Reserved, *i);
  }

  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

bool Mwv208RegisterInfo::isReservedReg(const MachineFunction &MF,
                                       MCRegister Reg) const {
  return getReservedRegs(MF)[Reg];
}

const TargetRegisterClass *
Mwv208RegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                       unsigned Kind) const {
  return &JJ::IntRegsRegClass;
}

static void replaceFI(MachineFunction &MF, MachineBasicBlock::iterator II,
                      MachineInstr &MI, const DebugLoc &dl,
                      unsigned FIOperandNum, int Offset, unsigned FramePtr) {
  // Replace frame index with a frame pointer reference.
  if (Offset >= -4096 && Offset <= 4095) {
    // If the offset is small enough to fit in the immediate field, directly
    // encode it.
    MI.getOperand(FIOperandNum).ChangeToRegister(FramePtr, false);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
    return;
  }

  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

  // FIXME: it would be better to scavenge a register here instead of
  // reserving G1 all of the time.
  if (Offset >= 0) {
    // Emit nonnegaive immediates with sethi + or.
    // sethi %hi(Offset), %g1
    // add %g1, %fp, %g1
    // Insert G1+%lo(offset) into the user.
    BuildMI(*MI.getParent(), II, dl, TII.get(JJ::SETHIi), JJ::G1)
        .addImm(HI22(Offset));

    // Emit G1 = G1 + I6
    BuildMI(*MI.getParent(), II, dl, TII.get(JJ::ADDrr), JJ::G1)
        .addReg(JJ::G1)
        .addReg(FramePtr);
    // Insert: G1+%lo(offset) into the user.
    MI.getOperand(FIOperandNum).ChangeToRegister(JJ::G1, false);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(LO10(Offset));
    return;
  }

  // Emit Negative numbers with sethi + xor
  // sethi %hix(Offset), %g1
  // xor  %g1, %lox(offset), %g1
  // add %g1, %fp, %g1
  // Insert: G1 + 0 into the user.
  BuildMI(*MI.getParent(), II, dl, TII.get(JJ::SETHIi), JJ::G1)
      .addImm(HIX22(Offset));
  BuildMI(*MI.getParent(), II, dl, TII.get(JJ::XORri), JJ::G1)
      .addReg(JJ::G1)
      .addImm(LOX10(Offset));

  BuildMI(*MI.getParent(), II, dl, TII.get(JJ::ADDrr), JJ::G1)
      .addReg(JJ::G1)
      .addReg(FramePtr);
  // Insert: G1+%lo(offset) into the user.
  MI.getOperand(FIOperandNum).ChangeToRegister(JJ::G1, false);
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(0);
}

bool Mwv208RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  MachineInstr &MI = *II;
  DebugLoc dl = MI.getDebugLoc();
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  MachineFunction &MF = *MI.getParent()->getParent();
  const Mwv208Subtarget &Subtarget = MF.getSubtarget<Mwv208Subtarget>();
  const Mwv208FrameLowering *TFI = getFrameLowering(MF);

  Register FrameReg;
  int Offset;
  Offset = TFI->getFrameIndexReference(MF, FrameIndex, FrameReg).getFixed();

  Offset += MI.getOperand(FIOperandNum + 1).getImm();

  if (!Subtarget.isV9() || !Subtarget.hasHardQuad()) {
    if (MI.getOpcode() == JJ::STQFri) {
      const TargetInstrInfo &TII = *Subtarget.getInstrInfo();
      Register SrcReg = MI.getOperand(2).getReg();
      Register SrcEvenReg = getSubReg(SrcReg, JJ::sub_even64);
      Register SrcOddReg = getSubReg(SrcReg, JJ::sub_odd64);
      MachineInstr *StMI = BuildMI(*MI.getParent(), II, dl, TII.get(JJ::STDFri))
                               .addReg(FrameReg)
                               .addImm(0)
                               .addReg(SrcEvenReg);
      replaceFI(MF, *StMI, *StMI, dl, 0, Offset, FrameReg);
      MI.setDesc(TII.get(JJ::STDFri));
      MI.getOperand(2).setReg(SrcOddReg);
      Offset += 8;
    } else if (MI.getOpcode() == JJ::LDQFri) {
      const TargetInstrInfo &TII = *Subtarget.getInstrInfo();
      Register DestReg = MI.getOperand(0).getReg();
      Register DestEvenReg = getSubReg(DestReg, JJ::sub_even64);
      Register DestOddReg = getSubReg(DestReg, JJ::sub_odd64);
      MachineInstr *LdMI =
          BuildMI(*MI.getParent(), II, dl, TII.get(JJ::LDDFri), DestEvenReg)
              .addReg(FrameReg)
              .addImm(0);
      replaceFI(MF, *LdMI, *LdMI, dl, 1, Offset, FrameReg);

      MI.setDesc(TII.get(JJ::LDDFri));
      MI.getOperand(0).setReg(DestOddReg);
      Offset += 8;
    }
  }

  replaceFI(MF, II, MI, dl, FIOperandNum, Offset, FrameReg);
  // replaceFI never removes II
  return false;
}

Register Mwv208RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return JJ::I6;
}
