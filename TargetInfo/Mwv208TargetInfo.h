//===-- Mwv208TargetInfo.h - Mwv208 Target Implementation ---------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_TARGETINFO_MWV208TARGETINFO_H
#define LLVM_LIB_TARGET_MWV208_TARGETINFO_MWV208TARGETINFO_H

namespace llvm {

class Target;

Target &getTheMwv208Target();
Target &getTheMwv208V9Target();
Target &getTheMwv208elTarget();

} // namespace llvm

#endif // LLVM_LIB_TARGET_MWV208_TARGETINFO_MWV208TARGETINFO_H
