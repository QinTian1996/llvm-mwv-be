//===-- Mwv208AsmPrinter.cpp - Mwv208 LLVM assembly writer
//------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format MWV208 assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Mwv208InstPrinter.h"
#include "MCTargetDesc/Mwv208MCExpr.h"
#include "MCTargetDesc/Mwv208MCTargetDesc.h"
#include "MCTargetDesc/Mwv208TargetStreamer.h"
#include "Mwv208.h"
#include "Mwv208InstrInfo.h"
#include "Mwv208TargetMachine.h"
#include "TargetInfo/Mwv208TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class Mwv208AsmPrinter : public AsmPrinter {
  Mwv208TargetStreamer &getTargetStreamer() {
    return static_cast<Mwv208TargetStreamer &>(
        *OutStreamer->getTargetStreamer());
  }

public:
  explicit Mwv208AsmPrinter(TargetMachine &TM,
                            std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "Mwv208 Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  static const char *getRegisterName(MCRegister Reg) {
    return Mwv208InstPrinter::getRegisterName(Reg);
  }
};
} // end of anonymous namespace

void Mwv208AsmPrinter::emitInstruction(const MachineInstr *MI) {
  Mwv208_MC::verifyInstructionPredicates(MI->getOpcode(),
                                         getSubtargetInfo().getFeatureBits());

  MachineBasicBlock::const_instr_iterator I = MI->getIterator();
  MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();
  do {
    MCInst TmpInst;
    LowerMwv208MachineInstrToMCInst(&*I, TmpInst, *this);
    EmitToStreamer(*OutStreamer, TmpInst);
  } while ((++I != E) && I->isInsideBundle()); // <--bundle: 并行指令组
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMwv208AsmPrinter() {
  RegisterAsmPrinter<Mwv208AsmPrinter> X(getTheMwv208Target());
}
