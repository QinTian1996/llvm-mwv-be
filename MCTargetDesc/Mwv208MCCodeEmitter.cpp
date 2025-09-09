//===-- Mwv208MCCodeEmitter.cpp - Convert Mwv208 code to machine code
//-------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Mwv208MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Mwv208FixupKinds.h"
#include "Mwv208MCExpr.h"
#include "Mwv208MCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace {

class Mwv208MCCodeEmitter : public MCCodeEmitter {
  MCContext &Ctx;

public:
  Mwv208MCCodeEmitter(const MCInstrInfo &, MCContext &ctx) : Ctx(ctx) {}
  Mwv208MCCodeEmitter(const Mwv208MCCodeEmitter &) = delete;
  Mwv208MCCodeEmitter &operator=(const Mwv208MCCodeEmitter &) = delete;
  ~Mwv208MCCodeEmitter() override = default;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;
  unsigned getCallTargetOpValue(const MCInst &MI, unsigned OpNo,
                                SmallVectorImpl<MCFixup> &Fixups,
                                const MCSubtargetInfo &STI) const;
  unsigned getBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &STI) const;
  unsigned getSImm13OpValue(const MCInst &MI, unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const;
  unsigned getBranchPredTargetOpValue(const MCInst &MI, unsigned OpNo,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const;
  unsigned getBranchOnRegTargetOpValue(const MCInst &MI, unsigned OpNo,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const;
};

} // end anonymous namespace

void Mwv208MCCodeEmitter::encodeInstruction(const MCInst &MI,
                                            SmallVectorImpl<char> &CB,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  unsigned Bits = getBinaryCodeForInstr(MI, Fixups, STI);
  support::endian::write(CB, Bits,
                         Ctx.getAsmInfo()->isLittleEndian()
                             ? llvm::endianness::little
                             : llvm::endianness::big);

  // Some instructions have phantom operands that only contribute a fixup entry.
  unsigned SymOpNo = 0;
  switch (MI.getOpcode()) {
  default:
    break;
  case MWV208::TLS_CALL:
    SymOpNo = 1;
    break;
  case MWV208::GDOP_LDrr:
  case MWV208::GDOP_LDXrr:
  case MWV208::TLS_ADDrr:
  case MWV208::TLS_LDrr:
  case MWV208::TLS_LDXrr:
    SymOpNo = 3;
    break;
  }
  if (SymOpNo != 0) {
    const MCOperand &MO = MI.getOperand(SymOpNo);
    uint64_t op = getMachineOpValue(MI, MO, Fixups, STI);
    assert(op == 0 && "Unexpected operand value!");
    (void)op; // suppress warning.
  }

  ++MCNumEmitted; // Keep track of the # of mi's emitted.
}

unsigned
Mwv208MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr());
  const MCExpr *Expr = MO.getExpr();
  if (const Mwv208MCExpr *SExpr = dyn_cast<Mwv208MCExpr>(Expr)) {
    MCFixupKind Kind = (MCFixupKind)SExpr->getFixupKind();
    Fixups.push_back(MCFixup::create(0, Expr, Kind));
    return 0;
  }

  int64_t Res;
  if (Expr->evaluateAsAbsolute(Res))
    return Res;

  llvm_unreachable("Unhandled expression!");
  return 0;
}

unsigned
Mwv208MCCodeEmitter::getSImm13OpValue(const MCInst &MI, unsigned OpNo,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr() &&
         "getSImm13OpValue expects only expressions or an immediate");

  const MCExpr *Expr = MO.getExpr();

  // Constant value, no fixup is needed
  if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
    return CE->getValue();

  MCFixupKind Kind;
  if (const Mwv208MCExpr *SExpr = dyn_cast<Mwv208MCExpr>(Expr)) {
    Kind = MCFixupKind(SExpr->getFixupKind());
  } else {
    bool IsPic = Ctx.getObjectFileInfo()->isPositionIndependent();
    Kind = IsPic ? MCFixupKind(Mwv208::fixup_mwv208_got13)
                 : MCFixupKind(Mwv208::fixup_mwv208_13);
  }

  Fixups.push_back(MCFixup::create(0, Expr, Kind));
  return 0;
}

unsigned
Mwv208MCCodeEmitter::getCallTargetOpValue(const MCInst &MI, unsigned OpNo,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  const MCExpr *Expr = MO.getExpr();
  const Mwv208MCExpr *SExpr = dyn_cast<Mwv208MCExpr>(Expr);

  if (MI.getOpcode() == MWV208::TLS_CALL) {
    // No fixups for __tls_get_addr. Will emit for fixups for tls_symbol in
    // encodeInstruction.
#ifndef NDEBUG
    // Verify that the callee is actually __tls_get_addr.
    assert(SExpr && SExpr->getSubExpr()->getKind() == MCExpr::SymbolRef &&
           "Unexpected expression in TLS_CALL");
    const MCSymbolRefExpr *SymExpr = cast<MCSymbolRefExpr>(SExpr->getSubExpr());
    assert(SymExpr->getSymbol().getName() == "__tls_get_addr" &&
           "Unexpected function for TLS_CALL");
#endif
    return 0;
  }

  MCFixupKind Kind = MCFixupKind(SExpr->getFixupKind());
  Fixups.push_back(MCFixup::create(0, Expr, Kind));
  return 0;
}

unsigned
Mwv208MCCodeEmitter::getBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isReg() || MO.isImm())
    return getMachineOpValue(MI, MO, Fixups, STI);

  Fixups.push_back(
      MCFixup::create(0, MO.getExpr(), (MCFixupKind)Mwv208::fixup_mwv208_br22));
  return 0;
}

unsigned Mwv208MCCodeEmitter::getBranchPredTargetOpValue(
    const MCInst &MI, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isReg() || MO.isImm())
    return getMachineOpValue(MI, MO, Fixups, STI);

  Fixups.push_back(
      MCFixup::create(0, MO.getExpr(), (MCFixupKind)Mwv208::fixup_mwv208_br19));
  return 0;
}

unsigned Mwv208MCCodeEmitter::getBranchOnRegTargetOpValue(
    const MCInst &MI, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isReg() || MO.isImm())
    return getMachineOpValue(MI, MO, Fixups, STI);

  Fixups.push_back(
      MCFixup::create(0, MO.getExpr(), (MCFixupKind)Mwv208::fixup_mwv208_br16));

  return 0;
}

#include "Mwv208GenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createMwv208MCCodeEmitter(const MCInstrInfo &MCII,
                                               MCContext &Ctx) {
  return new Mwv208MCCodeEmitter(MCII, Ctx);
}
