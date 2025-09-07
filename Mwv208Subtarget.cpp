//===-- Mwv208Subtarget.cpp - MWV208 Subtarget Information
//------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the MWV208 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "Mwv208Subtarget.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

#define DEBUG_TYPE "mwv208-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Mwv208GenSubtargetInfo.inc"

void Mwv208Subtarget::anchor() {}

Mwv208Subtarget &Mwv208Subtarget::initializeSubtargetDependencies(
    StringRef CPU, StringRef TuneCPU, StringRef FS) {
  // Determine default and user specified characteristics
  std::string CPUName = std::string(CPU);

  // Parse features string.
  ParseSubtargetFeatures(CPUName, CPUName, FS);

  return *this;
}

Mwv208Subtarget::Mwv208Subtarget(const StringRef &CPU, const StringRef &TuneCPU,
                                 const StringRef &FS, const TargetMachine &TM,
                                 bool is64Bit)
    : Mwv208GenSubtargetInfo(TM.getTargetTriple(), CPU, TuneCPU, FS),
      ReserveRegister(TM.getMCRegisterInfo()->getNumRegs()),
      TargetTriple(TM.getTargetTriple()), Is64Bit(is64Bit),
      InstrInfo(initializeSubtargetDependencies(CPU, TuneCPU, FS)),
      TLInfo(TM, *this), FrameLowering(*this) {}

int Mwv208Subtarget::getAdjustedFrameSize(int frameSize) const {
  // Emit the correct save instruction based on the number of bytes in
  // the frame. Minimum stack frame size according to V8 ABI is:
  //   16 words for register window spill
  //    1 word for address of returned aggregate-value
  // +  6 words for passing parameters on the stack
  // ----------
  //   23 words * 4 bytes per word = 92 bytes
  frameSize += 92;

  // Round up to next doubleword boundary -- a double-word boundary
  // is required by the ABI.
  frameSize = alignTo(frameSize, 8);
  return frameSize;
}

bool Mwv208Subtarget::enableMachineScheduler() const { return true; }
