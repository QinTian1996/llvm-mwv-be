//===-- Mwv208ISelLowering.cpp - Mwv208 DAG Lowering Implementation
//---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that Mwv208 uses to lower LLVM code into
// a selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Mwv208ISelLowering.h"
#include "MCTargetDesc/Mwv208MCExpr.h"
#include "MCTargetDesc/Mwv208MCTargetDesc.h"
#include "Mwv208MachineFunctionInfo.h"
#include "Mwv208RegisterInfo.h"
#include "Mwv208TargetMachine.h"
#include "Mwv208TargetObjectFile.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/KnownBits.h"
using namespace llvm;

#define DEBUG_TYPE "mwv208-isellowering"
#define DEBUG

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

// 暂时用PTX的内置CC先占位, 初级阶段一律inline, 不走fn call

Register
Mwv208TargetLowering::getRegisterByName(const char *RegName, LLT VT,
                                        const MachineFunction &MF) const {
  Register Reg = StringSwitch<Register>(RegName)
                     .Case("r0", MWV208::r0)
                     .Case("r1", MWV208::r1)
                     .Case("r2", MWV208::r2)
                     .Case("r3", MWV208::r3)
                     .Case("r4", MWV208::r4)
                     .Case("r5", MWV208::r5)
                     .Case("r6", MWV208::r6)
                     .Case("r7", MWV208::r7)
                     .Case("r8", MWV208::r8)
                     .Case("r9", MWV208::r9)
                     .Case("r10", MWV208::r10)
                     .Case("r11", MWV208::r11)
                     .Case("r12", MWV208::r12)
                     .Case("r13", MWV208::r13)
                     .Case("r14", MWV208::r14)
                     .Case("r15", MWV208::r15)
                     .Case("r16", MWV208::r16)
                     .Case("r17", MWV208::r17)
                     .Case("r18", MWV208::r18)
                     .Case("r19", MWV208::r19)
                     .Case("r20", MWV208::r20)
                     .Case("r21", MWV208::r21)
                     .Case("r22", MWV208::r22)
                     .Case("r23", MWV208::r23)
                     .Case("r24", MWV208::r24)
                     .Case("r25", MWV208::r25)
                     .Case("r26", MWV208::r26)
                     .Case("r27", MWV208::r27)
                     .Case("r28", MWV208::r28)
                     .Case("r29", MWV208::r29)
                     .Case("r30", MWV208::r30)
                     .Case("r31", MWV208::r31)
                     .Case("c0", MWV208::c0)
                     .Case("c1", MWV208::c1)
                     .Case("c2", MWV208::c2)
                     .Case("c3", MWV208::c3)
                     .Case("c4", MWV208::c4)
                     .Case("c5", MWV208::c5)
                     .Case("c6", MWV208::c6)
                     .Case("c7", MWV208::c7)
                     .Default(0);

  if (Reg)
    return Reg;

  report_fatal_error("Invalid register name global variable");
}

//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

Mwv208TargetLowering::Mwv208TargetLowering(const TargetMachine &TM,
                                           const Mwv208Subtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {}

bool Mwv208TargetLowering::useSoftFloat() const { return false; }

const char *Mwv208TargetLowering::getTargetNodeName(unsigned Opcode) const {
  return "todo";
}

void Mwv208TargetLowering::computeKnownBitsForTargetNode(
    const SDValue Op, KnownBits &Known, const APInt &DemandedElts,
    const SelectionDAG &DAG, unsigned Depth) const {}