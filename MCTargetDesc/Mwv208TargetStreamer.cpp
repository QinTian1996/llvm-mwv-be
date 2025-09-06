//===-- Mwv208TargetStreamer.cpp - Mwv208 Target Streamer Methods
//-----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Mwv208 specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "Mwv208TargetStreamer.h"
#include "Mwv208InstPrinter.h"
#include "Mwv208MCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

static unsigned getEFlagsForFeatureSet(const MCSubtargetInfo &STI) {
  unsigned EFlags = 0;

  if (STI.hasFeature(Mwv208::FeatureV8Plus))
    EFlags |= ELF::EF_SPARC_32PLUS;

  return EFlags;
}

// pin vtable to this file
Mwv208TargetStreamer::Mwv208TargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {}

void Mwv208TargetStreamer::anchor() {}

Mwv208TargetAsmStreamer::Mwv208TargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : Mwv208TargetStreamer(S), OS(OS) {}

void Mwv208TargetAsmStreamer::emitMwv208RegisterIgnore(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(Mwv208InstPrinter::getRegisterName(reg)).lower()
     << ", #ignore\n";
}

void Mwv208TargetAsmStreamer::emitMwv208RegisterScratch(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(Mwv208InstPrinter::getRegisterName(reg)).lower()
     << ", #scratch\n";
}

Mwv208TargetELFStreamer::Mwv208TargetELFStreamer(MCStreamer &S,
                                                 const MCSubtargetInfo &STI)
    : Mwv208TargetStreamer(S) {
  ELFObjectWriter &W = getStreamer().getWriter();
  unsigned EFlags = W.getELFHeaderEFlags();

  EFlags |= getEFlagsForFeatureSet(STI);

  W.setELFHeaderEFlags(EFlags);
}

MCELFStreamer &Mwv208TargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
