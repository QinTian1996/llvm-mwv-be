//===-- Mwv208.h - Top-level interface for Mwv208 representation --*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Mwv208 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MWV208_H
#define LLVM_LIB_TARGET_MWV208_MWV208_H

#include "MCTargetDesc/Mwv208MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class AsmPrinter;
class FunctionPass;
class MCInst;
class MachineInstr;
class PassRegistry;
class Mwv208TargetMachine;

FunctionPass *createMwv208ISelDag(Mwv208TargetMachine &TM);

void LowerMwv208MachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                     AsmPrinter &AP);
void initializeMwv208DAGToDAGISelLegacyPass(PassRegistry &);
} // namespace llvm

#endif
