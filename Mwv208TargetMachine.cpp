//===-- Mwv208TargetMachine.cpp - Define TargetMachine for Mwv208
//-----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "Mwv208TargetMachine.h"
#include "Mwv208.h"
#include "Mwv208MachineFunctionInfo.h"
#include "Mwv208TargetObjectFile.h"
#include "TargetInfo/Mwv208TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include <optional>
using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMwv208Target() {
  // Register the target.
  RegisterTargetMachine<Mwv208V8TargetMachine> X(getTheMwv208Target());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeMwv208DAGToDAGISelLegacyPass(PR);
}

static std::string computeDataLayout(const Triple &T, bool is64Bit) {
  // Mwv208 is typically big endian, but some are little.
  std::string Ret = "E";
  Ret += "-m:e";

  // Some ABIs have 32bit pointers.
  if (!is64Bit)
    Ret += "-p:32:32";

  // Alignments for 64 bit integers.
  Ret += "-i64:64";

  // Alignments for 128 bit integers.
  // This is not specified in the ABI document but is the de facto standard.
  Ret += "-i128:128";

  // On Mwv208V9 128 floats are aligned to 128 bits, on others only to 64.
  // On Mwv208V9 registers can hold 64 or 32 bits, on others only 32.
  if (is64Bit)
    Ret += "-n32:64";
  else
    Ret += "-f128:64-n32";

  if (is64Bit)
    Ret += "-S128";
  else
    Ret += "-S64";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

static CodeModel::Model
getEffectiveMwv208CodeModel(std::optional<CodeModel::Model> CM, Reloc::Model RM,
                            bool Is64Bit, bool JIT) {
  return CodeModel::Small;
}

/// Create an ILP32 architecture model
Mwv208TargetMachine::Mwv208TargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         std::optional<Reloc::Model> RM,
                                         std::optional<CodeModel::Model> CM,
                                         CodeGenOptLevel OL, bool JIT,
                                         bool is64bit)
    : CodeGenTargetMachineImpl(
          T, computeDataLayout(TT, is64bit), TT, CPU, FS, Options,
          getEffectiveRelocModel(RM),
          getEffectiveMwv208CodeModel(CM, getEffectiveRelocModel(RM), is64bit,
                                      JIT),
          OL),
      TLOF(std::make_unique<Mwv208ELFTargetObjectFile>()) {
  initAsmInfo();
}

Mwv208TargetMachine::~Mwv208TargetMachine() = default;

const TargetSubtargetInfo *
Mwv208TargetMachine::getSubtargetImpl(const Function &F) const {
  std::string CPU = "v8";
  std::string FS = "";

  if (!DefaultSubtarget) {
    DefaultSubtarget =
        std::make_unique<TargetSubtargetInfo>(CPU, CPU, FS, *this, false);
  }
  return DefaultSubtarget.get();
}

MachineFunctionInfo *Mwv208TargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return Mwv208MachineFunctionInfo::create<Mwv208MachineFunctionInfo>(Allocator,
                                                                      F, STI);
}

namespace {
/// Mwv208 Code Generator Pass Configuration Options.
class Mwv208PassConfig : public TargetPassConfig {
public:
  Mwv208PassConfig(Mwv208TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  Mwv208TargetMachine &getMwv208TargetMachine() const {
    return getTM<Mwv208TargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *Mwv208TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Mwv208PassConfig(*this, PM);
}

void Mwv208PassConfig::addIRPasses() {
  addPass(createAtomicExpandLegacyPass());

  TargetPassConfig::addIRPasses();
}

bool Mwv208PassConfig::addInstSelector() {
  addPass(createMwv208ISelDag(getMwv208TargetMachine()));
  return false;
}

void Mwv208PassConfig::addPreEmitPass() {}

void Mwv208V8TargetMachine::anchor() {}

Mwv208V8TargetMachine::Mwv208V8TargetMachine(const Target &T, const Triple &TT,
                                             StringRef CPU, StringRef FS,
                                             const TargetOptions &Options,
                                             std::optional<Reloc::Model> RM,
                                             std::optional<CodeModel::Model> CM,
                                             CodeGenOptLevel OL, bool JIT)
    : Mwv208TargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}