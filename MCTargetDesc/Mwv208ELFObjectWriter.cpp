//===-- Mwv208ELFObjectWriter.cpp - Mwv208 ELF Writer
//-----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Mwv208FixupKinds.h"
#include "MCTargetDesc/Mwv208MCExpr.h"
#include "MCTargetDesc/Mwv208MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class Mwv208ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  Mwv208ELFObjectWriter(bool Is64Bit, bool IsV8Plus, uint8_t OSABI)
      : MCELFObjectTargetWriter(
            Is64Bit, OSABI,
            Is64Bit ? ELF::EM_SPARCV9
                    : (IsV8Plus ? ELF::EM_SPARC32PLUS : ELF::EM_SPARC),
            /*HasRelocationAddend*/ true) {}

  ~Mwv208ELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;

  bool needsRelocateWithSymbol(const MCValue &Val, const MCSymbol &Sym,
                               unsigned Type) const override;
};
} // namespace

unsigned Mwv208ELFObjectWriter::getRelocType(MCContext &Ctx,
                                             const MCValue &Target,
                                             const MCFixup &Fixup,
                                             bool IsPCRel) const {
  MCFixupKind Kind = Fixup.getKind();
  if (Kind >= FirstLiteralRelocationKind)
    return Kind - FirstLiteralRelocationKind;

  if (const Mwv208MCExpr *SExpr = dyn_cast<Mwv208MCExpr>(Fixup.getValue())) {
    if (SExpr->getKind() == Mwv208MCExpr::VK_Mwv208_R_DISP32)
      return ELF::R_SPARC_DISP32;
  }

  if (IsPCRel) {
    switch (Fixup.getTargetKind()) {
    default:
      llvm_unreachable("Unimplemented fixup -> relocation");
    case FK_Data_1:
      return ELF::R_SPARC_DISP8;
    case FK_Data_2:
      return ELF::R_SPARC_DISP16;
    case FK_Data_4:
      return ELF::R_SPARC_DISP32;
    case FK_Data_8:
      return ELF::R_SPARC_DISP64;
    case Mwv208::fixup_mwv208_call30:
      return ELF::R_SPARC_WDISP30;
    case Mwv208::fixup_mwv208_br22:
      return ELF::R_SPARC_WDISP22;
    case Mwv208::fixup_mwv208_br19:
      return ELF::R_SPARC_WDISP19;
    case Mwv208::fixup_mwv208_br16:
      return ELF::R_SPARC_WDISP16;
    case Mwv208::fixup_mwv208_pc22:
      return ELF::R_SPARC_PC22;
    case Mwv208::fixup_mwv208_pc10:
      return ELF::R_SPARC_PC10;
    case Mwv208::fixup_mwv208_wplt30:
      return ELF::R_SPARC_WPLT30;
    }
  }

  switch (Fixup.getTargetKind()) {
  default:
    llvm_unreachable("Unimplemented fixup -> relocation");
  case FK_NONE:
    return ELF::R_SPARC_NONE;
  case FK_Data_1:
    return ELF::R_SPARC_8;
  case FK_Data_2:
    return ((Fixup.getOffset() % 2) ? ELF::R_SPARC_UA16 : ELF::R_SPARC_16);
  case FK_Data_4:
    return ((Fixup.getOffset() % 4) ? ELF::R_SPARC_UA32 : ELF::R_SPARC_32);
  case FK_Data_8:
    return ((Fixup.getOffset() % 8) ? ELF::R_SPARC_UA64 : ELF::R_SPARC_64);
  case Mwv208::fixup_mwv208_13:
    return ELF::R_SPARC_13;
  case Mwv208::fixup_mwv208_hi22:
    return ELF::R_SPARC_HI22;
  case Mwv208::fixup_mwv208_lo10:
    return ELF::R_SPARC_LO10;
  case Mwv208::fixup_mwv208_h44:
    return ELF::R_SPARC_H44;
  case Mwv208::fixup_mwv208_m44:
    return ELF::R_SPARC_M44;
  case Mwv208::fixup_mwv208_l44:
    return ELF::R_SPARC_L44;
  case Mwv208::fixup_mwv208_hh:
    return ELF::R_SPARC_HH22;
  case Mwv208::fixup_mwv208_hm:
    return ELF::R_SPARC_HM10;
  case Mwv208::fixup_mwv208_lm:
    return ELF::R_SPARC_LM22;
  case Mwv208::fixup_mwv208_got22:
    return ELF::R_SPARC_GOT22;
  case Mwv208::fixup_mwv208_got10:
    return ELF::R_SPARC_GOT10;
  case Mwv208::fixup_mwv208_got13:
    return ELF::R_SPARC_GOT13;
  case Mwv208::fixup_mwv208_tls_gd_hi22:
    return ELF::R_SPARC_TLS_GD_HI22;
  case Mwv208::fixup_mwv208_tls_gd_lo10:
    return ELF::R_SPARC_TLS_GD_LO10;
  case Mwv208::fixup_mwv208_tls_gd_add:
    return ELF::R_SPARC_TLS_GD_ADD;
  case Mwv208::fixup_mwv208_tls_gd_call:
    return ELF::R_SPARC_TLS_GD_CALL;
  case Mwv208::fixup_mwv208_tls_ldm_hi22:
    return ELF::R_SPARC_TLS_LDM_HI22;
  case Mwv208::fixup_mwv208_tls_ldm_lo10:
    return ELF::R_SPARC_TLS_LDM_LO10;
  case Mwv208::fixup_mwv208_tls_ldm_add:
    return ELF::R_SPARC_TLS_LDM_ADD;
  case Mwv208::fixup_mwv208_tls_ldm_call:
    return ELF::R_SPARC_TLS_LDM_CALL;
  case Mwv208::fixup_mwv208_tls_ldo_hix22:
    return ELF::R_SPARC_TLS_LDO_HIX22;
  case Mwv208::fixup_mwv208_tls_ldo_lox10:
    return ELF::R_SPARC_TLS_LDO_LOX10;
  case Mwv208::fixup_mwv208_tls_ldo_add:
    return ELF::R_SPARC_TLS_LDO_ADD;
  case Mwv208::fixup_mwv208_tls_ie_hi22:
    return ELF::R_SPARC_TLS_IE_HI22;
  case Mwv208::fixup_mwv208_tls_ie_lo10:
    return ELF::R_SPARC_TLS_IE_LO10;
  case Mwv208::fixup_mwv208_tls_ie_ld:
    return ELF::R_SPARC_TLS_IE_LD;
  case Mwv208::fixup_mwv208_tls_ie_ldx:
    return ELF::R_SPARC_TLS_IE_LDX;
  case Mwv208::fixup_mwv208_tls_ie_add:
    return ELF::R_SPARC_TLS_IE_ADD;
  case Mwv208::fixup_mwv208_tls_le_hix22:
    return ELF::R_SPARC_TLS_LE_HIX22;
  case Mwv208::fixup_mwv208_tls_le_lox10:
    return ELF::R_SPARC_TLS_LE_LOX10;
  case Mwv208::fixup_mwv208_hix22:
    return ELF::R_SPARC_HIX22;
  case Mwv208::fixup_mwv208_lox10:
    return ELF::R_SPARC_LOX10;
  case Mwv208::fixup_mwv208_gotdata_hix22:
    return ELF::R_SPARC_GOTDATA_HIX22;
  case Mwv208::fixup_mwv208_gotdata_lox10:
    return ELF::R_SPARC_GOTDATA_LOX10;
  case Mwv208::fixup_mwv208_gotdata_op:
    return ELF::R_SPARC_GOTDATA_OP;
  }

  return ELF::R_SPARC_NONE;
}

bool Mwv208ELFObjectWriter::needsRelocateWithSymbol(const MCValue &,
                                                    const MCSymbol &,
                                                    unsigned Type) const {
  switch (Type) {
  default:
    return false;

  // All relocations that use a GOT need a symbol, not an offset, as
  // the offset of the symbol within the section is irrelevant to
  // where the GOT entry is. Don't need to list all the TLS entries,
  // as they're all marked as requiring a symbol anyways.
  case ELF::R_SPARC_GOT10:
  case ELF::R_SPARC_GOT13:
  case ELF::R_SPARC_GOT22:
  case ELF::R_SPARC_GOTDATA_HIX22:
  case ELF::R_SPARC_GOTDATA_LOX10:
  case ELF::R_SPARC_GOTDATA_OP_HIX22:
  case ELF::R_SPARC_GOTDATA_OP_LOX10:
    return true;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createMwv208ELFObjectWriter(bool Is64Bit, bool IsV8Plus, uint8_t OSABI) {
  return std::make_unique<Mwv208ELFObjectWriter>(Is64Bit, IsV8Plus, OSABI);
}
