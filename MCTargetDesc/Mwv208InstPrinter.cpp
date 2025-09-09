//===-- Mwv208InstPrinter.cpp - Convert Mwv208 MCInst to assembly syntax
//-----==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an Mwv208 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "Mwv208InstPrinter.h"
#include "Mwv208.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// The generated AsmMatcher Mwv208GenAsmWriter uses "Mwv208" as the target
// namespace. But MWV208 backend uses "JJ" as its namespace.
namespace llvm {
namespace Mwv208 {
using namespace JJ;
}
} // namespace llvm

#define GET_INSTRUCTION_NAME
#define PRINT_ALIAS_INSTR
#include "Mwv208GenAsmWriter.inc"

bool Mwv208InstPrinter::isV9(const MCSubtargetInfo &STI) const {
  return (STI.hasFeature(Mwv208::FeatureV9)) != 0;
}

void Mwv208InstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  OS << '%' << getRegisterName(Reg);
}

void Mwv208InstPrinter::printRegName(raw_ostream &OS, MCRegister Reg,
                                     unsigned AltIdx) const {
  OS << '%' << getRegisterName(Reg, AltIdx);
}

void Mwv208InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                  StringRef Annot, const MCSubtargetInfo &STI,
                                  raw_ostream &O) {
  if (!printAliasInstr(MI, Address, STI, O) &&
      !printMwv208AliasInstr(MI, STI, O))
    printInstruction(MI, Address, STI, O);
  printAnnotation(O, Annot);
}

bool Mwv208InstPrinter::printMwv208AliasInstr(const MCInst *MI,
                                              const MCSubtargetInfo &STI,
                                              raw_ostream &O) {
  switch (MI->getOpcode()) {
  default:
    return false;
  case MWV208::JMPLrr:
  case MWV208::JMPLri: {
    if (MI->getNumOperands() != 3)
      return false;
    if (!MI->getOperand(0).isReg())
      return false;
    switch (MI->getOperand(0).getReg()) {
    default:
      return false;
    case MWV208::G0: // jmp $addr | ret | retl
      if (MI->getOperand(2).isImm() && MI->getOperand(2).getImm() == 8) {
        switch (MI->getOperand(1).getReg()) {
        default:
          break;
        case MWV208::I7:
          O << "\tret";
          return true;
        case MWV208::O7:
          O << "\tretl";
          return true;
        }
      }
      O << "\tjmp ";
      printMemOperand(MI, 1, STI, O);
      return true;
    case MWV208::O7: // call $addr
      O << "\tcall ";
      printMemOperand(MI, 1, STI, O);
      return true;
    }
  }
  case MWV208::V9FCMPS:
  case MWV208::V9FCMPD:
  case MWV208::V9FCMPQ:
  case MWV208::V9FCMPES:
  case MWV208::V9FCMPED:
  case MWV208::V9FCMPEQ: {
    if ((MI->getNumOperands() != 3) || (!MI->getOperand(0).isReg()) ||
        (MI->getOperand(0).getReg() != MWV208::FCC0))
      return false;
    // if V8, skip printing %fcc0.
    switch (MI->getOpcode()) {
    default:
    case MWV208::V9FCMPS:
      O << "\tfcmps ";
      break;
    case MWV208::V9FCMPD:
      O << "\tfcmpd ";
      break;
    case MWV208::V9FCMPQ:
      O << "\tfcmpq ";
      break;
    case MWV208::V9FCMPES:
      O << "\tfcmpes ";
      break;
    case MWV208::V9FCMPED:
      O << "\tfcmped ";
      break;
    case MWV208::V9FCMPEQ:
      O << "\tfcmpeq ";
      break;
    }
    printOperand(MI, 1, STI, O);
    O << ", ";
    printOperand(MI, 2, STI, O);
    return true;
  }
  }
}

void Mwv208InstPrinter::printOperand(const MCInst *MI, int opNum,
                                     const MCSubtargetInfo &STI,
                                     raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);

  if (MO.isReg()) {
    unsigned Reg = MO.getReg();
    printRegName(O, Reg);
    return;
  }

  if (MO.isImm()) {
    switch (MI->getOpcode()) {
    default:
      O << (int)MO.getImm();
      return;

    case MWV208::TICCri: // Fall through
    case MWV208::TICCrr: // Fall through
    case MWV208::TRAPri: // Fall through
    case MWV208::TRAPrr: // Fall through
    case MWV208::TXCCri: // Fall through
    case MWV208::TXCCrr: // Fall through
      // Only seven-bit values up to 127.
      O << ((int)MO.getImm() & 0x7f);
      return;
    }
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}

void Mwv208InstPrinter::printMemOperand(const MCInst *MI, int opNum,
                                        const MCSubtargetInfo &STI,
                                        raw_ostream &O) {
  const MCOperand &Op1 = MI->getOperand(opNum);
  const MCOperand &Op2 = MI->getOperand(opNum + 1);

  bool PrintedFirstOperand = false;
  if (Op1.isReg() && Op1.getReg() != MWV208::G0) {
    printOperand(MI, opNum, STI, O);
    PrintedFirstOperand = true;
  }

  // Skip the second operand iff it adds nothing (literal 0 or %g0) and we've
  // already printed the first one
  const bool SkipSecondOperand =
      PrintedFirstOperand && ((Op2.isReg() && Op2.getReg() == MWV208::G0) ||
                              (Op2.isImm() && Op2.getImm() == 0));

  if (!SkipSecondOperand) {
    if (PrintedFirstOperand)
      O << '+';
    printOperand(MI, opNum + 1, STI, O);
  }
}

void Mwv208InstPrinter::printCCOperand(const MCInst *MI, int opNum,
                                       const MCSubtargetInfo &STI,
                                       raw_ostream &O) {
  int CC = (int)MI->getOperand(opNum).getImm();
  switch (MI->getOpcode()) {
  default:
    break;
  case MWV208::FBCOND:
  case MWV208::FBCONDA:
  case MWV208::FBCOND_V9:
  case MWV208::FBCONDA_V9:
  case MWV208::BPFCC:
  case MWV208::BPFCCA:
  case MWV208::BPFCCNT:
  case MWV208::BPFCCANT:
  case MWV208::MOVFCCrr:
  case MWV208::V9MOVFCCrr:
  case MWV208::MOVFCCri:
  case MWV208::V9MOVFCCri:
  case MWV208::FMOVS_FCC:
  case MWV208::V9FMOVS_FCC:
  case MWV208::FMOVD_FCC:
  case MWV208::V9FMOVD_FCC:
  case MWV208::FMOVQ_FCC:
  case MWV208::V9FMOVQ_FCC:
    // Make sure CC is a fp conditional flag.
    CC = (CC < JJCC::FCC_BEGIN) ? (CC + JJCC::FCC_BEGIN) : CC;
    break;
  case MWV208::CBCOND:
  case MWV208::CBCONDA:
    // Make sure CC is a cp conditional flag.
    CC = (CC < JJCC::CPCC_BEGIN) ? (CC + JJCC::CPCC_BEGIN) : CC;
    break;
  case MWV208::BPR:
  case MWV208::BPRA:
  case MWV208::BPRNT:
  case MWV208::BPRANT:
  case MWV208::MOVRri:
  case MWV208::MOVRrr:
  case MWV208::FMOVRS:
  case MWV208::FMOVRD:
  case MWV208::FMOVRQ:
    // Make sure CC is a register conditional flag.
    CC = (CC < JJCC::REG_BEGIN) ? (CC + JJCC::REG_BEGIN) : CC;
    break;
  }
  O << MWV208CondCodeToString((JJCC::CondCodes)CC);
}

bool Mwv208InstPrinter::printGetPCX(const MCInst *MI, unsigned opNum,
                                    const MCSubtargetInfo &STI,
                                    raw_ostream &O) {
  llvm_unreachable("FIXME: Implement Mwv208InstPrinter::printGetPCX.");
  return true;
}

void Mwv208InstPrinter::printMembarTag(const MCInst *MI, int opNum,
                                       const MCSubtargetInfo &STI,
                                       raw_ostream &O) {
  static const char *const TagNames[] = {
      "#LoadLoad",  "#StoreLoad", "#LoadStore", "#StoreStore",
      "#Lookaside", "#MemIssue",  "#Sync"};

  unsigned Imm = MI->getOperand(opNum).getImm();

  if (Imm > 127) {
    O << Imm;
    return;
  }

  bool First = true;
  for (unsigned i = 0; i < std::size(TagNames); i++) {
    if (Imm & (1 << i)) {
      O << (First ? "" : " | ") << TagNames[i];
      First = false;
    }
  }
}

void Mwv208InstPrinter::printASITag(const MCInst *MI, int opNum,
                                    const MCSubtargetInfo &STI,
                                    raw_ostream &O) {
  unsigned Imm = MI->getOperand(opNum).getImm();
  auto ASITag = Mwv208ASITag::lookupASITagByEncoding(Imm);
  O << Imm;
}

void Mwv208InstPrinter::printPrefetchTag(const MCInst *MI, int opNum,
                                         const MCSubtargetInfo &STI,
                                         raw_ostream &O) {
  unsigned Imm = MI->getOperand(opNum).getImm();
  auto PrefetchTag = Mwv208PrefetchTag::lookupPrefetchTagByEncoding(Imm);
  if (PrefetchTag)
    O << '#' << PrefetchTag->Name;
  else
    O << Imm;
}
