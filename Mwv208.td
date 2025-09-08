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

def FeatureSoftMulDiv
  : SubtargetFeature<"soft-mul-div", "UseSoftMulDiv", "true",
                     "Use software emulation for integer multiply and divide">;

def FeatureNoFSMULD
  : SubtargetFeature<"no-fsmuld", "HasNoFSMULD", "true",
                     "Disable the fsmuld instruction.">;
def FeatureNoFMULS
  : SubtargetFeature<"no-fmuls", "HasNoFMULS", "true",
                     "Disable the fmuls instruction.">;

def FeatureV9
  : SubtargetFeature<"v9", "IsV9", "true",
                     "Enable MWV208-V9 instructions">;
def FeatureV8Plus
  : SubtargetFeature<"v8plus", "IsV8Plus", "true",
                     "Enable V8+ mode, allowing use of 64-bit V9 instructions in 32-bit code">;
def FeatureV8Deprecated
  : SubtargetFeature<"deprecated-v8", "UseV8DeprecatedInsts", "true",
                     "Enable deprecated V8 instructions in V9 mode">;
def FeatureVIS
  : SubtargetFeature<"vis", "IsVIS", "true",
                     "Enable UltraMWV208 Visual Instruction Set extensions">;
def FeatureVIS2
  : SubtargetFeature<"vis2", "IsVIS2", "true",
                     "Enable Visual Instruction Set extensions II">;
def FeatureVIS3
  : SubtargetFeature<"vis3", "IsVIS3", "true",
                     "Enable Visual Instruction Set extensions III">;
def FeatureLeon
  : SubtargetFeature<"leon", "IsLeon", "true",
                     "Enable LEON extensions">;
def FeaturePWRPSR
  : SubtargetFeature<"leonpwrpsr", "HasPWRPSR", "true",
                     "Enable the PWRPSR instruction">;

def FeatureHardQuad
  : SubtargetFeature<"hard-quad-float", "HasHardQuad", "true",
                     "Enable quad-word floating point instructions">;

def UsePopc : SubtargetFeature<"popc", "UsePopc", "true",
                               "Use the popc (population count) instruction">;

def FeatureSoftFloat : SubtargetFeature<"soft-float", "UseSoftFloat", "true",
                              "Use software emulation for floating point">;

//===----------------------------------------------------------------------===//
// MWV208 Subtarget tuning features.
//

def TuneSlowRDPC : SubtargetFeature<"slow-rdpc", "HasSlowRDPC", "true",
                                    "rd %pc, %XX is slow", [FeatureV9]>;

//==== Features added predmoninantly for LEON subtarget support
//include "LeonFeatures.td"

//==== Register allocation tweaks needed by some low-level software
foreach i = 1 ... 7  in
    def FeatureReserveG#i : SubtargetFeature<"reserve-g"#i, "ReserveRegister["#i#" + JJ::G0]", "true",
                                             "Reserve G"#i#", making it unavailable as a GPR">;
foreach i = 0 ... 5 in
    def FeatureReserveO#i : SubtargetFeature<"reserve-o"#i, "ReserveRegister["#i#" + JJ::O0]", "true",
                                             "Reserve O"#i#", making it unavailable as a GPR">;
foreach i = 0 ... 7 in
    def FeatureReserveL#i : SubtargetFeature<"reserve-l"#i, "ReserveRegister["#i#" + JJ::L0]", "true",
                                             "Reserve L"#i#", making it unavailable as a GPR">;
foreach i = 0 ... 5 in
    def FeatureReserveI#i : SubtargetFeature<"reserve-i"#i, "ReserveRegister["#i#" + JJ::I0]", "true",
                                             "Reserve I"#i#", making it unavailable as a GPR">;

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "Mwv208ASITags.td"
include "Mwv208PrefetchTags.td"
include "Mwv208RegisterInfo.td"
include "Mwv208CallingConv.td"
include "Mwv208Schedule.td"
include "Mwv208InstrInfo.td"

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
def : Proc<"v8",              []>;



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
