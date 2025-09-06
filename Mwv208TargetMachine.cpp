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
#include "LeonPasses.h"
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
  RegisterTargetMachine<Mwv208V9TargetMachine> Y(getTheMwv208V9Target());
  RegisterTargetMachine<Mwv208elTargetMachine> Z(getTheMwv208elTarget());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeMwv208DAGToDAGISelLegacyPass(PR);
  initializeErrataWorkaroundPass(PR);
}

static cl::opt<bool>
    BranchRelaxation("mwv208-enable-branch-relax", cl::Hidden, cl::init(true),
                     cl::desc("Relax out of range conditional branches"));

static std::string computeDataLayout(const Triple &T, bool is64Bit) {
  // Mwv208 is typically big endian, but some are little.
  std::string Ret = T.getArch() == Triple::sparcel ? "e" : "E";
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

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      text+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.
static CodeModel::Model
getEffectiveMwv208CodeModel(std::optional<CodeModel::Model> CM, Reloc::Model RM,
                            bool Is64Bit, bool JIT) {
  if (CM) {
    if (*CM == CodeModel::Tiny)
      report_fatal_error("Target does not support the tiny CodeModel", false);
    if (*CM == CodeModel::Kernel)
      report_fatal_error("Target does not support the kernel CodeModel", false);
    return *CM;
  }
  if (Is64Bit) {
    if (JIT)
      return CodeModel::Large;
    return RM == Reloc::PIC_ ? CodeModel::Small : CodeModel::Medium;
  }
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
      TLOF(std::make_unique<Mwv208ELFTargetObjectFile>()), is64Bit(is64bit) {
  initAsmInfo();
}

Mwv208TargetMachine::~Mwv208TargetMachine() = default;

const Mwv208Subtarget *
Mwv208TargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute TuneAttr = F.getFnAttribute("tune-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU =
      CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  std::string TuneCPU =
      TuneAttr.isValid() ? TuneAttr.getValueAsString().str() : CPU;
  std::string FS =
      FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  // FIXME: This is related to the code below to reset the target options,
  // we need to know whether or not the soft float flag is set on the
  // function, so we can enable it as a subtarget feature.
  bool softFloat = F.getFnAttribute("use-soft-float").getValueAsBool();

  if (softFloat)
    FS += FS.empty() ? "+soft-float" : ",+soft-float";

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<Mwv208Subtarget>(CPU, TuneCPU, FS, *this,
                                          this->is64Bit);
  }
  return I.get();
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

void Mwv208PassConfig::addPreEmitPass() {
  if (BranchRelaxation)
    addPass(&BranchRelaxationPassID);

  addPass(createMwv208DelaySlotFillerPass());
  addPass(new InsertNOPLoad());
  addPass(new DetectRoundChange());
  addPass(new FixAllFDIVSQRT());
  addPass(new ErrataWorkaround());
}

void Mwv208V8TargetMachine::anchor() {}

Mwv208V8TargetMachine::Mwv208V8TargetMachine(const Target &T, const Triple &TT,
                                             StringRef CPU, StringRef FS,
                                             const TargetOptions &Options,
                                             std::optional<Reloc::Model> RM,
                                             std::optional<CodeModel::Model> CM,
                                             CodeGenOptLevel OL, bool JIT)
    : Mwv208TargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}

void Mwv208V9TargetMachine::anchor() {}

Mwv208V9TargetMachine::Mwv208V9TargetMachine(const Target &T, const Triple &TT,
                                             StringRef CPU, StringRef FS,
                                             const TargetOptions &Options,
                                             std::optional<Reloc::Model> RM,
                                             std::optional<CodeModel::Model> CM,
                                             CodeGenOptLevel OL, bool JIT)
    : Mwv208TargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

void Mwv208elTargetMachine::anchor() {}

Mwv208elTargetMachine::Mwv208elTargetMachine(const Target &T, const Triple &TT,
                                             StringRef CPU, StringRef FS,
                                             const TargetOptions &Options,
                                             std::optional<Reloc::Model> RM,
                                             std::optional<CodeModel::Model> CM,
                                             CodeGenOptLevel OL, bool JIT)
    : Mwv208TargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}
