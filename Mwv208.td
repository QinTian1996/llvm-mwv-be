//===-- Mwv208.td - Describe the Mwv208 Target Machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"
include "llvm/TableGen/SearchableTable.td"

//===----------------------------------------------------------------------===//
// MWV208 Subtarget features.
//

//===----------------------------------------------------------------------===//
// MWV208 Subtarget tuning features.
//

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "Mwv208RegisterInfo.td"
include "Mwv208Schedule.td"
include "Mwv208InstrInfo.td"
//Use PTX calling convention for now, will define our own later

def Mwv208InstrInfo : InstrInfo;

def Mwv208AsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = true;
  let AllowDuplicateRegisterNames = true;
}

def Mwv208AsmParserVariant : AsmParserVariant {
  let RegisterPrefix = "%";
}

//===----------------------------------------------------------------------===//
// MWV208 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features,
           list<SubtargetFeature> TuneFeatures = []>
 : Processor<Name, NoItineraries, Features, TuneFeatures>;

def : Proc<"generic",         []>;


//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def Mwv208AsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  int PassSubtarget = 1;
  int Variant = 0;
}

def Mwv208 : Target {
  // Pull in Instruction Info:
  let InstructionSet = Mwv208InstrInfo;
  let AssemblyParsers  = [Mwv208AsmParser];
  let AssemblyParserVariants = [Mwv208AsmParserVariant];
  let AssemblyWriters = [Mwv208AsmWriter];
  let AllowRegisterRenaming = 1;
}
