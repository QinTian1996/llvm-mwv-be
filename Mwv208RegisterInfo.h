//===-- Mwv208RegisterInfo.h - Mwv208 Register Information Impl ---*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Mwv208 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MWV208REGISTERINFO_H
#define LLVM_LIB_TARGET_MWV208_MWV208REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "Mwv208GenRegisterInfo.inc"

namespace llvm {
struct Mwv208RegisterInfo : public Mwv208GenRegisterInfo {
  Mwv208RegisterInfo();
};

} // end namespace llvm

#endif
