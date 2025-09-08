//===- Mwv208MachineFunctionInfo.h - Mwv208 Machine Function Info -*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares  Mwv208 specific per-machine-function information.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_MWV208_MWV208MACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_MWV208_MWV208MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class Mwv208MachineFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

private:
  Register GlobalBaseReg;

  /// VarArgsFrameOffset - Frame offset to start of varargs area.
  int VarArgsFrameOffset;

  /// SRetReturnReg - Holds the virtual register into which the sret
  /// argument is passed.
  Register SRetReturnReg;

  /// IsLeafProc - True if the function is a leaf procedure.
  bool IsLeafProc;

public:
  Mwv208MachineFunctionInfo()
      : GlobalBaseReg(0), VarArgsFrameOffset(0), SRetReturnReg(0),
        IsLeafProc(false) {}
  Mwv208MachineFunctionInfo(const Function &F, const TargetSubtargetInfo *STI)
      : GlobalBaseReg(0), VarArgsFrameOffset(0), SRetReturnReg(0),
        IsLeafProc(false) {}

  MachineFunctionInfo *
  clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
        const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
      const override;

  int getVarArgsFrameOffset() const { return VarArgsFrameOffset; }
  void setVarArgsFrameOffset(int Offset) { VarArgsFrameOffset = Offset; }

  Register getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(Register Reg) { SRetReturnReg = Reg; }

  void setLeafProc(bool rhs) { IsLeafProc = rhs; }
  bool isLeafProc() const { return IsLeafProc; }
};
} // namespace llvm

#endif
