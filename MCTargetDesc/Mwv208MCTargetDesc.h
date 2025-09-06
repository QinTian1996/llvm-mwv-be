//===-- Mwv208MCTargetDesc.h - Mwv208 Target Descriptions ---------*- C++
//-*-===//
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

#ifndef LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCTARGETDESC_H
#define LLVM_LIB_TARGET_MWV208_MCTARGETDESC_MWV208MCTARGETDESC_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCCodeEmitter *createMwv208MCCodeEmitter(const MCInstrInfo &MCII,
                                         MCContext &Ctx);
MCAsmBackend *createMwv208AsmBackend(const Target &T,
                                     const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const MCTargetOptions &Options);
std::unique_ptr<MCObjectTargetWriter>
createMwv208ELFObjectWriter(bool Is64Bit, bool IsV8Plus, uint8_t OSABI);

// Defines symbolic names for Mwv208 v9 ASI tag names.
namespace Mwv208ASITag {
struct ASITag {
  const char *Name;
  const char *AltName;
  unsigned Encoding;
};

#define GET_ASITagsList_DECL
#include "Mwv208GenSearchableTables.inc"
} // end namespace Mwv208ASITag

// Defines symbolic names for Mwv208 v9 prefetch tag names.
namespace Mwv208PrefetchTag {
struct PrefetchTag {
  const char *Name;
  unsigned Encoding;
};

#define GET_PrefetchTagsList_DECL
#include "Mwv208GenSearchableTables.inc"
} // end namespace Mwv208PrefetchTag
} // namespace llvm

// Defines symbolic names for Mwv208 registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "Mwv208GenRegisterInfo.inc"

// Defines symbolic names for the Mwv208 instructions.
//
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "Mwv208GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "Mwv208GenSubtargetInfo.inc"

#endif
