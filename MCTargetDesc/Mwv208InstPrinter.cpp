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
  case JJ::JMPLrr:
  case JJ::JMPLri: {
    if (MI->getNumOperands() != 3)
      return false;
    if (!MI->getOperand(0).isReg())
      return false;
    switch (MI->getOperand(0).getReg()) {
    default:
      return false;
    case JJ::G0: // jmp $addr | ret | retl
      if (MI->getOperand(2).isImm() && MI->getOperand(2).getImm() == 8) {
        switch (MI->getOperand(1).getReg()) {
        default:
          break;
        case JJ::I7:
          O << "\tret";
          return true;
        case JJ::O7:
          O << "\tretl";
          return true;
        }
      }
      O << "\tjmp ";
      printMemOperand(MI, 1, STI, O);
      return true;
    case JJ::O7: // call $addr
      O << "\tcall ";
      printMemOperand(MI, 1, STI, O);
      return true;
    }
  }
  case JJ::V9FCMPS:
  case JJ::V9FCMPD:
  case JJ::V9FCMPQ:
  case JJ::V9FCMPES:
  case JJ::V9FCMPED:
  case JJ::V9FCMPEQ: {
    if (isV9(STI) || (MI->getNumOperands() != 3) ||
        (!MI->getOperand(0).isReg()) ||
        (MI->getOperand(0).getReg() != JJ::FCC0))
      return false;
    // if V8, skip printing %fcc0.
    switch (MI->getOpcode()) {
    default:
    case JJ::V9FCMPS:
      O << "\tfcmps ";
      break;
    case JJ::V9FCMPD:
      O << "\tfcmpd ";
      break;
    case JJ::V9FCMPQ:
      O << "\tfcmpq ";
      break;
    case JJ::V9FCMPES:
      O << "\tfcmpes ";
      break;
    case JJ::V9FCMPED:
      O << "\tfcmped ";
      break;
    case JJ::V9FCMPEQ:
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
    if (isV9(STI))
      printRegName(O, Reg, JJ::RegNamesStateReg);
    else
      printRegName(O, Reg);
    return;
  }

  if (MO.isImm()) {
    switch (MI->getOpcode()) {
    default:
      O << (int)MO.getImm();
      return;

    case JJ::TICCri: // Fall through
    case JJ::TICCrr: // Fall through
    case JJ::TRAPri: // Fall through
    case JJ::TRAPrr: // Fall through
    case JJ::TXCCri: // Fall through
    case JJ::TXCCrr: // Fall through
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
  if (Op1.isReg() && Op1.getReg() != JJ::G0) {
    printOperand(MI, opNum, STI, O);
    PrintedFirstOperand = true;
  }

  // Skip the second operand iff it adds nothing (literal 0 or %g0) and we've
  // already printed the first one
  const bool SkipSecondOperand =
      PrintedFirstOperand && ((Op2.isReg() && Op2.getReg() == JJ::G0) ||
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
  case JJ::FBCOND:
  case JJ::FBCONDA:
  case JJ::FBCOND_V9:
  case JJ::FBCONDA_V9:
  case JJ::BPFCC:
  case JJ::BPFCCA:
  case JJ::BPFCCNT:
  case JJ::BPFCCANT:
  case JJ::MOVFCCrr:
  case JJ::V9MOVFCCrr:
  case JJ::MOVFCCri:
  case JJ::V9MOVFCCri:
  case JJ::FMOVS_FCC:
  case JJ::V9FMOVS_FCC:
  case JJ::FMOVD_FCC:
  case JJ::V9FMOVD_FCC:
  case JJ::FMOVQ_FCC:
  case JJ::V9FMOVQ_FCC:
    // Make sure CC is a fp conditional flag.
    CC = (CC < JJCC::FCC_BEGIN) ? (CC + JJCC::FCC_BEGIN) : CC;
    break;
  case JJ::CBCOND:
  case JJ::CBCONDA:
    // Make sure CC is a cp conditional flag.
    CC = (CC < JJCC::CPCC_BEGIN) ? (CC + JJCC::CPCC_BEGIN) : CC;
    break;
  case JJ::BPR:
  case JJ::BPRA:
  case JJ::BPRNT:
  case JJ::BPRANT:
  case JJ::MOVRri:
  case JJ::MOVRrr:
  case JJ::FMOVRS:
  case JJ::FMOVRD:
  case JJ::FMOVRQ:
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
  if (isV9(STI) && ASITag)
    O << '#' << ASITag->Name;
  else
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
