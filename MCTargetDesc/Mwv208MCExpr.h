//====- Mwv208MCExpr.h - Mwv208 specific MC expression classes --*- C++
//-*-=====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes Mwv208-specific MCExprs, used for modifiers like
// "%hi" or "%lo" etc.,
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCEXPR_H
#define LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCEXPR_H

#include "Mwv208FixupKinds.h"
#include "llvm/MC/MCExpr.h"

namespace llvm {

class StringRef;
class Mwv208MCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    VK_Mwv208_None,
    VK_Mwv208_LO,
    VK_Mwv208_HI,
    VK_Mwv208_H44,
    VK_Mwv208_M44,
    VK_Mwv208_L44,
    VK_Mwv208_HH,
    VK_Mwv208_HM,
    VK_Mwv208_LM,
    VK_Mwv208_PC22,
    VK_Mwv208_PC10,
    VK_Mwv208_GOT22,
    VK_Mwv208_GOT10,
    VK_Mwv208_GOT13,
    VK_Mwv208_13,
    VK_Mwv208_WPLT30,
    VK_Mwv208_WDISP30,
    VK_Mwv208_R_DISP32,
    VK_Mwv208_TLS_GD_HI22,
    VK_Mwv208_TLS_GD_LO10,
    VK_Mwv208_TLS_GD_ADD,
    VK_Mwv208_TLS_GD_CALL,
    VK_Mwv208_TLS_LDM_HI22,
    VK_Mwv208_TLS_LDM_LO10,
    VK_Mwv208_TLS_LDM_ADD,
    VK_Mwv208_TLS_LDM_CALL,
    VK_Mwv208_TLS_LDO_HIX22,
    VK_Mwv208_TLS_LDO_LOX10,
    VK_Mwv208_TLS_LDO_ADD,
    VK_Mwv208_TLS_IE_HI22,
    VK_Mwv208_TLS_IE_LO10,
    VK_Mwv208_TLS_IE_LD,
    VK_Mwv208_TLS_IE_LDX,
    VK_Mwv208_TLS_IE_ADD,
    VK_Mwv208_TLS_LE_HIX22,
    VK_Mwv208_TLS_LE_LOX10,
    VK_Mwv208_HIX22,
    VK_Mwv208_LOX10,
    VK_Mwv208_GOTDATA_HIX22,
    VK_Mwv208_GOTDATA_LOX10,
    VK_Mwv208_GOTDATA_OP,
  };

private:
  const VariantKind Kind;
  const MCExpr *Expr;

  explicit Mwv208MCExpr(VariantKind Kind, const MCExpr *Expr)
      : Kind(Kind), Expr(Expr) {}

public:
  /// @name Construction
  /// @{

  static const Mwv208MCExpr *create(VariantKind Kind, const MCExpr *Expr,
                                    MCContext &Ctx);
  /// @}
  /// @name Accessors
  /// @{

  /// getOpcode - Get the kind of this expression.
  VariantKind getKind() const { return Kind; }

  /// getSubExpr - Get the child of this expression.
  const MCExpr *getSubExpr() const { return Expr; }

  /// getFixupKind - Get the fixup kind of this expression.
  Mwv208::Fixups getFixupKind() const { return getFixupKind(Kind); }

  /// @}
  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAssembler *Asm,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCFragment *findAssociatedFragment() const override {
    return getSubExpr()->findAssociatedFragment();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  static VariantKind parseVariantKind(StringRef name);
  static bool printVariantKind(raw_ostream &OS, VariantKind Kind);
  static Mwv208::Fixups getFixupKind(VariantKind Kind);
};

} // end namespace llvm.

#endif
