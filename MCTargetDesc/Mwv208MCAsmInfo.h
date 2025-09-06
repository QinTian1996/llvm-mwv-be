//===- Mwv208MCAsmInfo.h - Mwv208 asm properties -----------------*- C++
//-*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Mwv208MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCASMINFO_H
#define LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class Mwv208ELFMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit Mwv208ELFMCAsmInfo(const Triple &TheTriple);

  const MCExpr *
  getExprForPersonalitySymbol(const MCSymbol *Sym, unsigned Encoding,
                              MCStreamer &Streamer) const override;
  const MCExpr *getExprForFDESymbol(const MCSymbol *Sym, unsigned Encoding,
                                    MCStreamer &Streamer) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCASMINFO_H
