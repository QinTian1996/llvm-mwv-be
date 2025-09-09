//===-- Mwv208MCTargetDesc.cpp - Mwv208 Target Descriptions
//-----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Mwv208 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "Mwv208MCTargetDesc.h"
#include "Mwv208InstPrinter.h"
#include "Mwv208MCAsmInfo.h"
#include "Mwv208TargetStreamer.h"
#include "TargetInfo/Mwv208TargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
namespace Mwv208ASITag {
#define GET_ASITagsList_IMPL
#include "Mwv208GenSearchableTables.inc"
} // end namespace Mwv208ASITag

namespace Mwv208PrefetchTag {
#define GET_PrefetchTagsList_IMPL
#include "Mwv208GenSearchableTables.inc"
} // end namespace Mwv208PrefetchTag
} // end namespace llvm

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "Mwv208GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Mwv208GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "Mwv208GenRegisterInfo.inc"

static MCAsmInfo *createMwv208MCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new Mwv208ELFMCAsmInfo(TT);
  unsigned Reg = MRI.getDwarfRegNum(MWV208::O6, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, Reg, 0);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCAsmInfo *createMwv208V9MCAsmInfo(const MCRegisterInfo &MRI,
                                          const Triple &TT,
                                          const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new Mwv208ELFMCAsmInfo(TT);
  unsigned Reg = MRI.getDwarfRegNum(MWV208::O6, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, Reg, 2047);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCInstrInfo *createMwv208MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitMwv208MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createMwv208MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitMwv208MCRegisterInfo(X, MWV208::O7);
  return X;
}

static MCSubtargetInfo *
createMwv208MCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  if (CPU.empty())
    CPU = "v8";
  return createMwv208MCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCTargetStreamer *
createObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new Mwv208TargetELFStreamer(S, STI);
}

static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS,
                                                 MCInstPrinter *InstPrint) {
  return new Mwv208TargetAsmStreamer(S, OS);
}

static MCTargetStreamer *createNullTargetStreamer(MCStreamer &S) {
  return new Mwv208TargetStreamer(S);
}

static MCInstPrinter *createMwv208MCInstPrinter(const Triple &T,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new Mwv208InstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMwv208TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(getTheMwv208Target(), createMwv208MCAsmInfo);
  RegisterMCAsmInfoFn Y(getTheMwv208V9Target(), createMwv208V9MCAsmInfo);
  RegisterMCAsmInfoFn Z(getTheMwv208elTarget(), createMwv208MCAsmInfo);

  for (Target *T : {&getTheMwv208Target(), &getTheMwv208V9Target(),
                    &getTheMwv208elTarget()}) {
    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createMwv208MCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createMwv208MCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createMwv208MCSubtargetInfo);

    // Register the MC Code Emitter.
    TargetRegistry::RegisterMCCodeEmitter(*T, createMwv208MCCodeEmitter);

    // Register the asm backend.
    TargetRegistry::RegisterMCAsmBackend(*T, createMwv208AsmBackend);

    // Register the object target streamer.
    TargetRegistry::RegisterObjectTargetStreamer(*T,
                                                 createObjectTargetStreamer);

    // Register the asm streamer.
    TargetRegistry::RegisterAsmTargetStreamer(*T, createTargetAsmStreamer);

    // Register the null streamer.
    TargetRegistry::RegisterNullTargetStreamer(*T, createNullTargetStreamer);

    // Register the MCInstPrinter
    TargetRegistry::RegisterMCInstPrinter(*T, createMwv208MCInstPrinter);
  }
}
