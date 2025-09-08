//===-- Mwv208InstrInfo.h - Mwv208 Instruction Information --------*- C++
//-*-===//
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

#ifndef LLVM_LIB_TARGET_MWV208_MWV208INSTRINFO_H
#define LLVM_LIB_TARGET_MWV208_MWV208INSTRINFO_H

#include "Mwv208RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "Mwv208GenInstrInfo.inc"

namespace llvm {

class Mwv208Subtarget;

class Mwv208InstrInfo : public Mwv208GenInstrInfo {
  const Mwv208RegisterInfo RI;
  virtual void anchor();

public:
  explicit Mwv208InstrInfo(Mwv208Subtarget &ST);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  const Mwv208RegisterInfo &getRegisterInfo() const { return RI; }

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;
};

} // namespace llvm

#endif
