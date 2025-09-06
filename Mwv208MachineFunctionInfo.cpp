//===-- Mwv208MachineFunctionInfo.cpp - Mwv208 Machine Function Info
//--------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Mwv208MachineFunctionInfo.h"

using namespace llvm;

void Mwv208MachineFunctionInfo::anchor() {}

MachineFunctionInfo *Mwv208MachineFunctionInfo::clone(
    BumpPtrAllocator &Allocator, MachineFunction &DestMF,
    const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
    const {
  return DestMF.cloneInfo<Mwv208MachineFunctionInfo>(*this);
}
