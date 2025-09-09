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

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "Mwv208GenRegisterInfo.inc"
