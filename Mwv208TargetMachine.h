//===-- Mwv208TargetMachine.h - Define TargetMachine for Mwv208 ---*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Mwv208 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MWV208TARGETMACHINE_H
#define LLVM_LIB_TARGET_MWV208_MWV208TARGETMACHINE_H

#include "Mwv208InstrInfo.h"
#include "Mwv208Subtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class Mwv208TargetMachine : public CodeGenTargetMachineImpl {
  /* use this to gen fatbin file*/
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

  /* the only subtarget*/
  mutable std::unique_ptr<Mwv208Subtarget> DefaultSubtarget;

public:
  Mwv208TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      std::optional<Reloc::Model> RM,
                      std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                      bool JIT, bool is64bit);
  ~Mwv208TargetMachine() override;

  const TargetSubtargetInfo *getSubtargetImpl(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

/// Mwv208 32-bit target machine
///
class Mwv208V8TargetMachine : public Mwv208TargetMachine {
  virtual void anchor();

public:
  Mwv208V8TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                        StringRef FS, const TargetOptions &Options,
                        std::optional<Reloc::Model> RM,
                        std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                        bool JIT);
};

} // end namespace llvm

#endif
