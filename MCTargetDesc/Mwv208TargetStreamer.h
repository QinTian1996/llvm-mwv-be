//===-- Mwv208TargetStreamer.h - Mwv208 Target Streamer ----------*- C++
//-*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208TARGETSTREAMER_H
#define LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208TARGETSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {

class formatted_raw_ostream;

class Mwv208TargetStreamer : public MCTargetStreamer {
  virtual void anchor();

public:
  Mwv208TargetStreamer(MCStreamer &S);
  /// Emit ".register <reg>, #ignore".
  virtual void emitMwv208RegisterIgnore(unsigned reg) {};
  /// Emit ".register <reg>, #scratch".
  virtual void emitMwv208RegisterScratch(unsigned reg) {};
};

// This part is for ascii assembly output
class Mwv208TargetAsmStreamer : public Mwv208TargetStreamer {
  formatted_raw_ostream &OS;

public:
  Mwv208TargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
  void emitMwv208RegisterIgnore(unsigned reg) override;
  void emitMwv208RegisterScratch(unsigned reg) override;
};

// This part is for ELF object output
class Mwv208TargetELFStreamer : public Mwv208TargetStreamer {
public:
  Mwv208TargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI);
  MCELFStreamer &getStreamer();
  void emitMwv208RegisterIgnore(unsigned reg) override {}
  void emitMwv208RegisterScratch(unsigned reg) override {}
};
} // end namespace llvm

#endif
