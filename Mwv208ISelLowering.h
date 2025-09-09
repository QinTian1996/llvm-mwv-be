//===-- Mwv208ISelLowering.h - Mwv208 DAG Lowering Interface ------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Mwv208 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MWV208ISELLOWERING_H
#define LLVM_LIB_TARGET_MWV208_MWV208ISELLOWERING_H

#include "Mwv208.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class Mwv208Subtarget;

namespace SPISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  CMPICC,     // Compare two GPR operands, set icc+xcc.
  CMPFCC,     // Compare two FP operands, set fcc.
  CMPFCC_V9,  // Compare two FP operands, set fcc (v9 variant).
  BRICC,      // Branch to dest on icc condition
  BRFCC,      // Branch to dest on fcc condition
  BRFCC_V9,   // Branch to dest on fcc condition (v9 variant).
  BR_REG,     // Branch to dest using the comparison of a register with zero.
  SELECT_ICC, // Select between two values using the current ICC flags.
  SELECT_XCC, // Select between two values using the current XCC flags.
  SELECT_FCC, // Select between two values using the current FCC flags.
  SELECT_REG, // Select between two values using the comparison of a register
              // with zero.

  Hi,
  Lo, // Hi/Lo operations, typically on a global address.

  FTOI, // FP to Int within a FP register.
  ITOF, // Int to FP within a FP register.
  FTOX, // FP to Int64 within a FP register.
  XTOF, // Int64 to FP within a FP register.

  CALL,            // A call instruction.
  RET_GLUE,        // Return with a glue operand.
  GLOBAL_BASE_REG, // Global base reg for PIC.
  FLUSHW,          // FLUSH register windows to stack.

  TAIL_CALL, // Tail call

  TLS_ADD, // For Thread Local Storage (TLS).
  TLS_LD,
  TLS_CALL,

  LOAD_GDOP, // Load operation w/ gdop relocation.
};
}

class Mwv208TargetLowering : public TargetLowering {
  const Mwv208Subtarget *Subtarget;

public:
  Mwv208TargetLowering(const TargetMachine &TM, const Mwv208Subtarget &STI);
  // SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  bool useSoftFloat() const override;

  /// computeKnownBitsForTargetNode - Determine which of the bits specified
  /// in Mask are known to be either zero or one and return them in the
  /// KnownZero/KnownOne bitsets.
  void computeKnownBitsForTargetNode(const SDValue Op, KnownBits &Known,
                                     const APInt &DemandedElts,
                                     const SelectionDAG &DAG,
                                     unsigned Depth = 0) const override;

  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *MBB) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;

  ConstraintType getConstraintType(StringRef Constraint) const override;
  ConstraintWeight
  getSingleConstraintMatchWeight(AsmOperandInfo &info,
                                 const char *constraint) const override;

  Register getRegisterByName(const char *RegName, LLT VT,
                             const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_MWV208_MWV208ISELLOWERING_H
