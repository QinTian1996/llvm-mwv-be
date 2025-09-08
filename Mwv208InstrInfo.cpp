//===-- Mwv208InstrInfo.cpp - Mwv208 Instruction Information
//----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Mwv208 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Mwv208InstrInfo.h"
#include "Mwv208.h"
#include "Mwv208MachineFunctionInfo.h"
#include "Mwv208Subtarget.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#include <iostream>

#define GET_INSTRINFO_CTOR_DTOR
#include "Mwv208GenInstrInfo.inc"

// Pin the vtable to this file.
void Mwv208InstrInfo::anchor() {}

Mwv208InstrInfo::Mwv208InstrInfo(Mwv208Subtarget &ST)
    : Mwv208GenInstrInfo(JJ::ADJCALLSTACKDOWN, JJ::ADJCALLSTACKUP), RI() {}

void Mwv208InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, MCRegister DestReg,
                                  MCRegister SrcReg, bool KillSrc,
                                  bool RenamableDest, bool RenamableSrc) const {
  if (JJ::IntRegsRegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, I, DL, get(JJ::ORrr), DestReg)
        .addReg(JJ::G0)
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else
    llvm_unreachable("Impossible reg-to-reg copy");
}