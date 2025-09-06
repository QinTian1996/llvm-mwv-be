//===-- Mwv208TargetInfo.cpp - Mwv208 Target Implementation
//-----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/Mwv208TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheMwv208Target() {
  static Target TheMwv208Target;
  return TheMwv208Target;
}
Target &llvm::getTheMwv208V9Target() {
  static Target TheMwv208V9Target;
  return TheMwv208V9Target;
}
Target &llvm::getTheMwv208elTarget() {
  static Target TheMwv208elTarget;
  return TheMwv208elTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMwv208TargetInfo() {
  RegisterTarget<Triple::sparc, /*HasJIT=*/false> X(
      getTheMwv208Target(), "mwv208", "Mwv208", "Mwv208");
  RegisterTarget<Triple::sparcv9, /*HasJIT=*/false> Y(
      getTheMwv208V9Target(), "mwv208v9", "Mwv208 V9", "Mwv208");
  RegisterTarget<Triple::sparcel, /*HasJIT=*/false> Z(
      getTheMwv208elTarget(), "mwv208el", "Mwv208 LE", "Mwv208");
}
