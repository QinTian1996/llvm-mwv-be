//===-- Mwv208FrameLowering.cpp - Mwv208 Frame Information
//------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Mwv208 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Mwv208FrameLowering.h"
#include "Mwv208InstrInfo.h"
#include "Mwv208MachineFunctionInfo.h"
#include "Mwv208Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

static cl::opt<bool>
    DisableLeafProc("disable-mwv208-leaf-proc", cl::init(false),
                    cl::desc("Disable Mwv208 leaf procedure optimization."),
                    cl::Hidden);

Mwv208FrameLowering::Mwv208FrameLowering(const Mwv208Subtarget &ST)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown,
                          ST.is64Bit() ? Align(16) : Align(8), 0,
                          ST.is64Bit() ? Align(16) : Align(8),
                          /*StackRealignable=*/false) {}

void Mwv208FrameLowering::emitSPAdjustment(MachineFunction &MF,
                                           MachineBasicBlock &MBB,
                                           MachineBasicBlock::iterator MBBI,
                                           int NumBytes, unsigned ADDrr,
                                           unsigned ADDri) const {

  DebugLoc dl;
  const Mwv208InstrInfo &TII =
      *static_cast<const Mwv208InstrInfo *>(MF.getSubtarget().getInstrInfo());

  if (NumBytes >= -4096 && NumBytes < 4096) {
    BuildMI(MBB, MBBI, dl, TII.get(ADDri), JJ::O6)
        .addReg(JJ::O6)
        .addImm(NumBytes);
    return;
  }

  // Emit this the hard way.  This clobbers G1 which we always know is
  // available here.
  if (NumBytes >= 0) {
    // Emit nonnegative numbers with sethi + or.
    // sethi %hi(NumBytes), %g1
    // or %g1, %lo(NumBytes), %g1
    // add %sp, %g1, %sp
    BuildMI(MBB, MBBI, dl, TII.get(JJ::SETHIi), JJ::G1).addImm(HI22(NumBytes));
    BuildMI(MBB, MBBI, dl, TII.get(JJ::ORri), JJ::G1)
        .addReg(JJ::G1)
        .addImm(LO10(NumBytes));
    BuildMI(MBB, MBBI, dl, TII.get(ADDrr), JJ::O6)
        .addReg(JJ::O6)
        .addReg(JJ::G1);
    return;
  }

  // Emit negative numbers with sethi + xor.
  // sethi %hix(NumBytes), %g1
  // xor %g1, %lox(NumBytes), %g1
  // add %sp, %g1, %sp
  BuildMI(MBB, MBBI, dl, TII.get(JJ::SETHIi), JJ::G1).addImm(HIX22(NumBytes));
  BuildMI(MBB, MBBI, dl, TII.get(JJ::XORri), JJ::G1)
      .addReg(JJ::G1)
      .addImm(LOX10(NumBytes));
  BuildMI(MBB, MBBI, dl, TII.get(ADDrr), JJ::O6).addReg(JJ::O6).addReg(JJ::G1);
}

void Mwv208FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  Mwv208MachineFunctionInfo *FuncInfo = MF.getInfo<Mwv208MachineFunctionInfo>();

  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const Mwv208Subtarget &Subtarget = MF.getSubtarget<Mwv208Subtarget>();
  const Mwv208InstrInfo &TII =
      *static_cast<const Mwv208InstrInfo *>(Subtarget.getInstrInfo());
  const Mwv208RegisterInfo &RegInfo =
      *static_cast<const Mwv208RegisterInfo *>(Subtarget.getRegisterInfo());
  MachineBasicBlock::iterator MBBI = MBB.begin();
  // Debug location must be unknown since the first debug location is used
  // to determine the end of the prologue.
  DebugLoc dl;

  // Get the number of bytes to allocate from the FrameInfo
  int NumBytes = (int)MFI.getStackSize();

  unsigned SAVEri = JJ::SAVEri;
  unsigned SAVErr = JJ::SAVErr;
  if (FuncInfo->isLeafProc()) {
    if (NumBytes == 0)
      return;
    SAVEri = JJ::ADDri;
    SAVErr = JJ::ADDrr;
  }

  // The MWV208 ABI is a bit odd in that it requires a reserved 92-byte
  // (128 in v9) area in the user's stack, starting at %sp. Thus, the
  // first part of the stack that can actually be used is located at
  // %sp + 92.
  //
  // We therefore need to add that offset to the total stack size
  // after all the stack objects are placed by
  // PrologEpilogInserter calculateFrameObjectOffsets. However, since the stack
  // needs to be aligned *after* the extra size is added, we need to disable
  // calculateFrameObjectOffsets's built-in stack alignment, by having
  // targetHandlesStackFrameRounding return true.

  // Add the extra call frame stack size, if needed. (This is the same
  // code as in PrologEpilogInserter, but also gets disabled by
  // targetHandlesStackFrameRounding)
  if (MFI.adjustsStack() && hasReservedCallFrame(MF))
    NumBytes += MFI.getMaxCallFrameSize();

  // Adds the MWV208 subtarget-specific spill area to the stack
  // size. Also ensures target-required alignment.
  NumBytes = Subtarget.getAdjustedFrameSize(NumBytes);

  // Finally, ensure that the size is sufficiently aligned for the
  // data on the stack.
  NumBytes = alignTo(NumBytes, MFI.getMaxAlign());

  // Update stack size with corrected value.
  MFI.setStackSize(NumBytes);

  emitSPAdjustment(MF, MBB, MBBI, -NumBytes, SAVErr, SAVEri);

  unsigned regFP = RegInfo.getDwarfRegNum(JJ::I6, true);

  // Emit ".cfi_def_cfa_register 30".
  unsigned CFIIndex =
      MF.addFrameInst(MCCFIInstruction::createDefCfaRegister(nullptr, regFP));
  BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex);

  // Emit ".cfi_window_save".
  CFIIndex = MF.addFrameInst(MCCFIInstruction::createWindowSave(nullptr));
  BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex);

  unsigned regInRA = RegInfo.getDwarfRegNum(JJ::I7, true);
  unsigned regOutRA = RegInfo.getDwarfRegNum(JJ::O7, true);
  // Emit ".cfi_register 15, 31".
  CFIIndex = MF.addFrameInst(
      MCCFIInstruction::createRegister(nullptr, regOutRA, regInRA));
  BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex);
}

MachineBasicBlock::iterator Mwv208FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  if (!hasReservedCallFrame(MF)) {
    MachineInstr &MI = *I;
    int Size = MI.getOperand(0).getImm();
    if (MI.getOpcode() == JJ::ADJCALLSTACKDOWN)
      Size = -Size;

    if (Size)
      emitSPAdjustment(MF, MBB, I, Size, JJ::ADDrr, JJ::ADDri);
  }
  return MBB.erase(I);
}

void Mwv208FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  Mwv208MachineFunctionInfo *FuncInfo = MF.getInfo<Mwv208MachineFunctionInfo>();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const Mwv208InstrInfo &TII =
      *static_cast<const Mwv208InstrInfo *>(MF.getSubtarget().getInstrInfo());
  DebugLoc dl = MBBI->getDebugLoc();
  assert((MBBI->getOpcode() == JJ::RETL || MBBI->getOpcode() == JJ::TAIL_CALL ||
          MBBI->getOpcode() == JJ::TAIL_CALLri) &&
         "Can only put epilog before 'retl' or 'tail_call' instruction!");
  if (!FuncInfo->isLeafProc()) {
    BuildMI(MBB, MBBI, dl, TII.get(JJ::RESTORErr), JJ::G0)
        .addReg(JJ::G0)
        .addReg(JJ::G0);
    return;
  }
  MachineFrameInfo &MFI = MF.getFrameInfo();

  int NumBytes = (int)MFI.getStackSize();
  if (NumBytes != 0)
    emitSPAdjustment(MF, MBB, MBBI, NumBytes, JJ::ADDrr, JJ::ADDri);

  // Preserve return address in %o7
  if (MBBI->getOpcode() == JJ::TAIL_CALL) {
    MBB.addLiveIn(JJ::O7);
    BuildMI(MBB, MBBI, dl, TII.get(JJ::ORrr), JJ::G1)
        .addReg(JJ::G0)
        .addReg(JJ::O7);
    BuildMI(MBB, MBBI, dl, TII.get(JJ::ORrr), JJ::O7)
        .addReg(JJ::G0)
        .addReg(JJ::G1);
  }
}

bool Mwv208FrameLowering::hasReservedCallFrame(
    const MachineFunction &MF) const {
  // Reserve call frame if there are no variable sized objects on the stack.
  return !MF.getFrameInfo().hasVarSizedObjects();
}

// hasFPImpl - Return true if the specified function should have a dedicated
// frame pointer register.  This is true if the function has variable sized
// allocas or if frame pointer elimination is disabled.
bool Mwv208FrameLowering::hasFPImpl(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken();
}

StackOffset
Mwv208FrameLowering::getFrameIndexReference(const MachineFunction &MF, int FI,
                                            Register &FrameReg) const {
  const Mwv208Subtarget &Subtarget = MF.getSubtarget<Mwv208Subtarget>();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const Mwv208RegisterInfo *RegInfo = Subtarget.getRegisterInfo();
  const Mwv208MachineFunctionInfo *FuncInfo =
      MF.getInfo<Mwv208MachineFunctionInfo>();
  bool isFixed = MFI.isFixedObjectIndex(FI);

  // Addressable stack objects are accessed using neg. offsets from
  // %fp, or positive offsets from %sp.
  bool UseFP;

  // Mwv208 uses FP-based references in general, even when "hasFP" is
  // false. That function is rather a misnomer, because %fp is
  // actually always available, unless isLeafProc.
  if (FuncInfo->isLeafProc()) {
    // If there's a leaf proc, all offsets need to be %sp-based,
    // because we haven't caused %fp to actually point to our frame.
    UseFP = false;
  } else if (isFixed) {
    // Otherwise, argument access should always use %fp.
    UseFP = true;
  } else {
    // Finally, default to using %fp.
    UseFP = true;
  }

  int64_t FrameOffset =
      MF.getFrameInfo().getObjectOffset(FI) + Subtarget.getStackPointerBias();

  if (UseFP) {
    FrameReg = RegInfo->getFrameRegister(MF);
    return StackOffset::getFixed(FrameOffset);
  } else {
    FrameReg = JJ::O6; // %sp
    return StackOffset::getFixed(FrameOffset +
                                 MF.getFrameInfo().getStackSize());
  }
}

static bool LLVM_ATTRIBUTE_UNUSED
verifyLeafProcRegUse(MachineRegisterInfo *MRI) {

  for (unsigned reg = JJ::I0; reg <= JJ::I7; ++reg)
    if (MRI->isPhysRegUsed(reg))
      return false;

  for (unsigned reg = JJ::L0; reg <= JJ::L7; ++reg)
    if (MRI->isPhysRegUsed(reg))
      return false;

  return true;
}

bool Mwv208FrameLowering::isLeafProc(MachineFunction &MF) const {

  MachineRegisterInfo &MRI = MF.getRegInfo();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  return !(MFI.hasCalls()               // has calls
           || MRI.isPhysRegUsed(JJ::L0) // Too many registers needed
           || MRI.isPhysRegUsed(JJ::O6) // %sp is used
           || hasFP(MF)                 // need %fp
           || MF.hasInlineAsm());       // has inline assembly
}

void Mwv208FrameLowering::remapRegsForLeafProc(MachineFunction &MF) const {
  MachineRegisterInfo &MRI = MF.getRegInfo();
  // Remap %i[0-7] to %o[0-7].
  for (unsigned reg = JJ::I0; reg <= JJ::I7; ++reg) {
    if (!MRI.isPhysRegUsed(reg))
      continue;

    unsigned mapped_reg = reg - JJ::I0 + JJ::O0;

    // Replace I register with O register.
    MRI.replaceRegWith(reg, mapped_reg);

    // Also replace register pair super-registers.
    if ((reg - JJ::I0) % 2 == 0) {
      unsigned preg = (reg - JJ::I0) / 2 + JJ::I0_I1;
      unsigned mapped_preg = preg - JJ::I0_I1 + JJ::O0_O1;
      MRI.replaceRegWith(preg, mapped_preg);
    }
  }

  // Rewrite MBB's Live-ins.
  for (MachineBasicBlock &MBB : MF) {
    for (unsigned reg = JJ::I0_I1; reg <= JJ::I6_I7; ++reg) {
      if (!MBB.isLiveIn(reg))
        continue;
      MBB.removeLiveIn(reg);
      MBB.addLiveIn(reg - JJ::I0_I1 + JJ::O0_O1);
    }
    for (unsigned reg = JJ::I0; reg <= JJ::I7; ++reg) {
      if (!MBB.isLiveIn(reg))
        continue;
      MBB.removeLiveIn(reg);
      MBB.addLiveIn(reg - JJ::I0 + JJ::O0);
    }
  }

  assert(verifyLeafProcRegUse(&MRI));
#ifdef EXPENSIVE_CHECKS
  MF.verify(0, "After LeafProc Remapping");
#endif
}

void Mwv208FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                               BitVector &SavedRegs,
                                               RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  if (!DisableLeafProc && isLeafProc(MF)) {
    Mwv208MachineFunctionInfo *MFI = MF.getInfo<Mwv208MachineFunctionInfo>();
    MFI->setLeafProc(true);

    remapRegsForLeafProc(MF);
  }
}
