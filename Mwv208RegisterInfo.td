//===-- Mwv208RegisterInfo.td - Mwv208 Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Mwv208 register file
//===----------------------------------------------------------------------===//

class Mwv208Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "JJ";
}

class Mwv208CtrlReg<bits<16> Enc, string n,
                   list<string> altNames = []>: Register<n, altNames> {
  let HWEncoding = Enc;
  let Namespace = "JJ";
}

let Namespace = "JJ" in {
def sub_even : SubRegIndex<32>;
def sub_odd  : SubRegIndex<32, 32>;
def sub_even64 : SubRegIndex<64>;
def sub_odd64  : SubRegIndex<64, 64>;
}

let Namespace = "JJ",
    FallbackRegAltNameIndex = NoRegAltName in {
  def RegNamesStateReg : RegAltNameIndex;
}

// Registers are identified with 5-bit ID numbers.
// Ri - 32-bit integer registers
class Ri<bits<16> Enc, string n> : Mwv208Reg<Enc, n>;

// Rdi - pairs of 32-bit integer registers
class Rdi<bits<16> Enc, string n, list<Register> subregs> : Mwv208Reg<Enc, n> {
  let SubRegs = subregs;
  let SubRegIndices = [sub_even, sub_odd];
  let CoveredBySubRegs = 1;
}
// Rf - 32-bit floating-point registers
class Rf<bits<16> Enc, string n> : Mwv208Reg<Enc, n>;

// Rd - Slots in the FP register file for 64-bit floating-point values.
class Rd<bits<16> Enc, string n, list<Register> subregs> : Mwv208Reg<Enc, n> {
  let SubRegs = subregs;
  let SubRegIndices = [sub_even, sub_odd];
  let CoveredBySubRegs = 1;
}

// Rq - Slots in the FP register file for 128-bit floating-point values.
class Rq<bits<16> Enc, string n, list<Register> subregs> : Mwv208Reg<Enc, n> {
  let SubRegs = subregs;
  let SubRegIndices = [sub_even64, sub_odd64];
  let CoveredBySubRegs = 1;
}

// Control Registers
def ICC : Mwv208CtrlReg<0, "icc">; // This represents icc and xcc in 64-bit code.
foreach I = 0-3 in
  def FCC#I : Mwv208CtrlReg<I, "fcc"#I>;

def FSR  : Mwv208CtrlReg<0, "fsr">; // Floating-point state register.
def FQ   : Mwv208CtrlReg<0, "fq">;  // Floating-point deferred-trap queue.
def CPSR : Mwv208CtrlReg<0, "csr">; // Co-processor state register.
def CPQ  : Mwv208CtrlReg<0, "cq">;  // Co-processor queue.

// Y register
def Y : Mwv208CtrlReg<0, "y">, DwarfRegNum<[64]>;
// Ancillary state registers (implementation defined)
def ASR1 : Mwv208CtrlReg<1, "asr1">;
let RegAltNameIndices = [RegNamesStateReg] in {
// FIXME: Currently this results in the assembler accepting
// the alternate names (%ccr, %asi, etc.) when targeting V8.
// Make sure that the alternate names are available for V9 only:
// %asr2-asr6      : valid on both V8 and V9.
// %ccr, %asi, etc.: valid on V9, returns "no such register" error on V8.
def ASR2 : Mwv208CtrlReg<2, "asr2", ["ccr"]>;
def ASR3 : Mwv208CtrlReg<3, "asr3", ["asi"]>;
def ASR4 : Mwv208CtrlReg<4, "asr4", ["tick"]>;
def ASR5 : Mwv208CtrlReg<5, "asr5", ["pc"]>;
def ASR6 : Mwv208CtrlReg<6, "asr6", ["fprs"]>;
}
def ASR7  : Mwv208CtrlReg< 7, "asr7">;
def ASR8  : Mwv208CtrlReg< 8, "asr8">;
def ASR9  : Mwv208CtrlReg< 9, "asr9">;
def ASR10 : Mwv208CtrlReg<10, "asr10">;
def ASR11 : Mwv208CtrlReg<11, "asr11">;
def ASR12 : Mwv208CtrlReg<12, "asr12">;
def ASR13 : Mwv208CtrlReg<13, "asr13">;
def ASR14 : Mwv208CtrlReg<14, "asr14">;
def ASR15 : Mwv208CtrlReg<15, "asr15">;
def ASR16 : Mwv208CtrlReg<16, "asr16">;
def ASR17 : Mwv208CtrlReg<17, "asr17">;
def ASR18 : Mwv208CtrlReg<18, "asr18">;
def ASR19 : Mwv208CtrlReg<19, "asr19">;
def ASR20 : Mwv208CtrlReg<20, "asr20">;
def ASR21 : Mwv208CtrlReg<21, "asr21">;
def ASR22 : Mwv208CtrlReg<22, "asr22">;
def ASR23 : Mwv208CtrlReg<23, "asr23">;
def ASR24 : Mwv208CtrlReg<24, "asr24">;
def ASR25 : Mwv208CtrlReg<25, "asr25">;
def ASR26 : Mwv208CtrlReg<26, "asr26">;
def ASR27 : Mwv208CtrlReg<27, "asr27">;
def ASR28 : Mwv208CtrlReg<28, "asr28">;
def ASR29 : Mwv208CtrlReg<29, "asr29">;
def ASR30 : Mwv208CtrlReg<30, "asr30">;
def ASR31 : Mwv208CtrlReg<31, "asr31">;

// Note that PSR, WIM, and TBR don't exist on the Mwv208V9, only the V8.
def PSR : Mwv208CtrlReg<0, "psr">;
def WIM : Mwv208CtrlReg<0, "wim">;
def TBR : Mwv208CtrlReg<0, "tbr">;

// Privileged V9 state registers
def TPC        : Mwv208CtrlReg< 0, "tpc">;
def TNPC       : Mwv208CtrlReg< 1, "tnpc">;
def TSTATE     : Mwv208CtrlReg< 2, "tstate">;
def TT         : Mwv208CtrlReg< 3, "tt">;
def TICK       : Mwv208CtrlReg< 4, "tick">;
def TBA        : Mwv208CtrlReg< 5, "tba">;
def PSTATE     : Mwv208CtrlReg< 6, "pstate">;
def TL         : Mwv208CtrlReg< 7, "tl">;
def PIL        : Mwv208CtrlReg< 8, "pil">;
def CWP        : Mwv208CtrlReg< 9, "cwp">;
def CANSAVE    : Mwv208CtrlReg<10, "cansave">;
def CANRESTORE : Mwv208CtrlReg<11, "canrestore">;
def CLEANWIN   : Mwv208CtrlReg<12, "cleanwin">;
def OTHERWIN   : Mwv208CtrlReg<13, "otherwin">;
def WSTATE     : Mwv208CtrlReg<14, "wstate">;
def GL         : Mwv208CtrlReg<16, "gl">;
def VER        : Mwv208CtrlReg<31, "ver">;

// Integer registers
def G0 : Ri< 0, "g0">, DwarfRegNum<[0]> {
  let isConstant = true;
}
def G1 : Ri< 1, "g1">, DwarfRegNum<[1]>;
def G2 : Ri< 2, "g2">, DwarfRegNum<[2]>;
def G3 : Ri< 3, "g3">, DwarfRegNum<[3]>;
def G4 : Ri< 4, "g4">, DwarfRegNum<[4]>;
def G5 : Ri< 5, "g5">, DwarfRegNum<[5]>;
def G6 : Ri< 6, "g6">, DwarfRegNum<[6]>;
def G7 : Ri< 7, "g7">, DwarfRegNum<[7]>;
def O0 : Ri< 8, "o0">, DwarfRegNum<[8]>;
def O1 : Ri< 9, "o1">, DwarfRegNum<[9]>;
def O2 : Ri<10, "o2">, DwarfRegNum<[10]>;
def O3 : Ri<11, "o3">, DwarfRegNum<[11]>;
def O4 : Ri<12, "o4">, DwarfRegNum<[12]>;
def O5 : Ri<13, "o5">, DwarfRegNum<[13]>;
def O6 : Ri<14, "sp">, DwarfRegNum<[14]>;
def O7 : Ri<15, "o7">, DwarfRegNum<[15]>;
def L0 : Ri<16, "l0">, DwarfRegNum<[16]>;
def L1 : Ri<17, "l1">, DwarfRegNum<[17]>;
def L2 : Ri<18, "l2">, DwarfRegNum<[18]>;
def L3 : Ri<19, "l3">, DwarfRegNum<[19]>;
def L4 : Ri<20, "l4">, DwarfRegNum<[20]>;
def L5 : Ri<21, "l5">, DwarfRegNum<[21]>;
def L6 : Ri<22, "l6">, DwarfRegNum<[22]>;
def L7 : Ri<23, "l7">, DwarfRegNum<[23]>;
def I0 : Ri<24, "i0">, DwarfRegNum<[24]>;
def I1 : Ri<25, "i1">, DwarfRegNum<[25]>;
def I2 : Ri<26, "i2">, DwarfRegNum<[26]>;
def I3 : Ri<27, "i3">, DwarfRegNum<[27]>;
def I4 : Ri<28, "i4">, DwarfRegNum<[28]>;
def I5 : Ri<29, "i5">, DwarfRegNum<[29]>;
def I6 : Ri<30, "fp">, DwarfRegNum<[30]>;
def I7 : Ri<31, "i7">, DwarfRegNum<[31]>;

// Floating-point registers
def F0  : Rf< 0, "f0">,  DwarfRegNum<[32]>;
def F1  : Rf< 1, "f1">,  DwarfRegNum<[33]>;
def F2  : Rf< 2, "f2">,  DwarfRegNum<[34]>;
def F3  : Rf< 3, "f3">,  DwarfRegNum<[35]>;
def F4  : Rf< 4, "f4">,  DwarfRegNum<[36]>;
def F5  : Rf< 5, "f5">,  DwarfRegNum<[37]>;
def F6  : Rf< 6, "f6">,  DwarfRegNum<[38]>;
def F7  : Rf< 7, "f7">,  DwarfRegNum<[39]>;
def F8  : Rf< 8, "f8">,  DwarfRegNum<[40]>;
def F9  : Rf< 9, "f9">,  DwarfRegNum<[41]>;
def F10 : Rf<10, "f10">, DwarfRegNum<[42]>;
def F11 : Rf<11, "f11">, DwarfRegNum<[43]>;
def F12 : Rf<12, "f12">, DwarfRegNum<[44]>;
def F13 : Rf<13, "f13">, DwarfRegNum<[45]>;
def F14 : Rf<14, "f14">, DwarfRegNum<[46]>;
def F15 : Rf<15, "f15">, DwarfRegNum<[47]>;
def F16 : Rf<16, "f16">, DwarfRegNum<[48]>;
def F17 : Rf<17, "f17">, DwarfRegNum<[49]>;
def F18 : Rf<18, "f18">, DwarfRegNum<[50]>;
def F19 : Rf<19, "f19">, DwarfRegNum<[51]>;
def F20 : Rf<20, "f20">, DwarfRegNum<[52]>;
def F21 : Rf<21, "f21">, DwarfRegNum<[53]>;
def F22 : Rf<22, "f22">, DwarfRegNum<[54]>;
def F23 : Rf<23, "f23">, DwarfRegNum<[55]>;
def F24 : Rf<24, "f24">, DwarfRegNum<[56]>;
def F25 : Rf<25, "f25">, DwarfRegNum<[57]>;
def F26 : Rf<26, "f26">, DwarfRegNum<[58]>;
def F27 : Rf<27, "f27">, DwarfRegNum<[59]>;
def F28 : Rf<28, "f28">, DwarfRegNum<[60]>;
def F29 : Rf<29, "f29">, DwarfRegNum<[61]>;
def F30 : Rf<30, "f30">, DwarfRegNum<[62]>;
def F31 : Rf<31, "f31">, DwarfRegNum<[63]>;

// Aliases of the F* registers used to hold 64-bit fp values (doubles)
def D0  : Rd< 0, "f0",  [F0,   F1]>, DwarfRegNum<[72]>;
def D1  : Rd< 2, "f2",  [F2,   F3]>, DwarfRegNum<[73]>;
def D2  : Rd< 4, "f4",  [F4,   F5]>, DwarfRegNum<[74]>;
def D3  : Rd< 6, "f6",  [F6,   F7]>, DwarfRegNum<[75]>;
def D4  : Rd< 8, "f8",  [F8,   F9]>, DwarfRegNum<[76]>;
def D5  : Rd<10, "f10", [F10, F11]>, DwarfRegNum<[77]>;
def D6  : Rd<12, "f12", [F12, F13]>, DwarfRegNum<[78]>;
def D7  : Rd<14, "f14", [F14, F15]>, DwarfRegNum<[79]>;
def D8  : Rd<16, "f16", [F16, F17]>, DwarfRegNum<[80]>;
def D9  : Rd<18, "f18", [F18, F19]>, DwarfRegNum<[81]>;
def D10 : Rd<20, "f20", [F20, F21]>, DwarfRegNum<[82]>;
def D11 : Rd<22, "f22", [F22, F23]>, DwarfRegNum<[83]>;
def D12 : Rd<24, "f24", [F24, F25]>, DwarfRegNum<[84]>;
def D13 : Rd<26, "f26", [F26, F27]>, DwarfRegNum<[85]>;
def D14 : Rd<28, "f28", [F28, F29]>, DwarfRegNum<[86]>;
def D15 : Rd<30, "f30", [F30, F31]>, DwarfRegNum<[87]>;

// Co-processor registers
def C0  : Ri< 0, "c0">;
def C1  : Ri< 1, "c1">;
def C2  : Ri< 2, "c2">;
def C3  : Ri< 3, "c3">;
def C4  : Ri< 4, "c4">;
def C5  : Ri< 5, "c5">;
def C6  : Ri< 6, "c6">;
def C7  : Ri< 7, "c7">;
def C8  : Ri< 8, "c8">;
def C9  : Ri< 9, "c9">;
def C10 : Ri<10, "c10">;
def C11 : Ri<11, "c11">;
def C12 : Ri<12, "c12">;
def C13 : Ri<13, "c13">;
def C14 : Ri<14, "c14">;
def C15 : Ri<15, "c15">;
def C16 : Ri<16, "c16">;
def C17 : Ri<17, "c17">;
def C18 : Ri<18, "c18">;
def C19 : Ri<19, "c19">;
def C20 : Ri<20, "c20">;
def C21 : Ri<21, "c21">;
def C22 : Ri<22, "c22">;
def C23 : Ri<23, "c23">;
def C24 : Ri<24, "c24">;
def C25 : Ri<25, "c25">;
def C26 : Ri<26, "c26">;
def C27 : Ri<27, "c27">;
def C28 : Ri<28, "c28">;
def C29 : Ri<29, "c29">;
def C30 : Ri<30, "c30">;
def C31 : Ri<31, "c31">;

// Unaliased double precision floating point registers.
// FIXME: Define DwarfRegNum for these registers.
def D16 : Mwv208Reg< 1, "f32">;
def D17 : Mwv208Reg< 3, "f34">;
def D18 : Mwv208Reg< 5, "f36">;
def D19 : Mwv208Reg< 7, "f38">;
def D20 : Mwv208Reg< 9, "f40">;
def D21 : Mwv208Reg<11, "f42">;
def D22 : Mwv208Reg<13, "f44">;
def D23 : Mwv208Reg<15, "f46">;
def D24 : Mwv208Reg<17, "f48">;
def D25 : Mwv208Reg<19, "f50">;
def D26 : Mwv208Reg<21, "f52">;
def D27 : Mwv208Reg<23, "f54">;
def D28 : Mwv208Reg<25, "f56">;
def D29 : Mwv208Reg<27, "f58">;
def D30 : Mwv208Reg<29, "f60">;
def D31 : Mwv208Reg<31, "f62">;

// Aliases of the F* registers used to hold 128-bit for values (long doubles).
def Q0  : Rq< 0, "f0",  [D0,  D1]>;
def Q1  : Rq< 4, "f4",  [D2,  D3]>;
def Q2  : Rq< 8, "f8",  [D4,  D5]>;
def Q3  : Rq<12, "f12", [D6,  D7]>;
def Q4  : Rq<16, "f16", [D8,  D9]>;
def Q5  : Rq<20, "f20", [D10, D11]>;
def Q6  : Rq<24, "f24", [D12, D13]>;
def Q7  : Rq<28, "f28", [D14, D15]>;
def Q8  : Rq< 1, "f32", [D16, D17]>;
def Q9  : Rq< 5, "f36", [D18, D19]>;
def Q10 : Rq< 9, "f40", [D20, D21]>;
def Q11 : Rq<13, "f44", [D22, D23]>;
def Q12 : Rq<17, "f48", [D24, D25]>;
def Q13 : Rq<21, "f52", [D26, D27]>;
def Q14 : Rq<25, "f56", [D28, D29]>;
def Q15 : Rq<29, "f60", [D30, D31]>;

// Aliases of the integer registers used for LDD/STD double-word operations
def G0_G1 : Rdi< 0, "g0", [G0, G1]>;
def G2_G3 : Rdi< 2, "g2", [G2, G3]>;
def G4_G5 : Rdi< 4, "g4", [G4, G5]>;
def G6_G7 : Rdi< 6, "g6", [G6, G7]>;
def O0_O1 : Rdi< 8, "o0", [O0, O1]>;
def O2_O3 : Rdi<10, "o2", [O2, O3]>;
def O4_O5 : Rdi<12, "o4", [O4, O5]>;
def O6_O7 : Rdi<14, "o6", [O6, O7]>;
def L0_L1 : Rdi<16, "l0", [L0, L1]>;
def L2_L3 : Rdi<18, "l2", [L2, L3]>;
def L4_L5 : Rdi<20, "l4", [L4, L5]>;
def L6_L7 : Rdi<22, "l6", [L6, L7]>;
def I0_I1 : Rdi<24, "i0", [I0, I1]>;
def I2_I3 : Rdi<26, "i2", [I2, I3]>;
def I4_I5 : Rdi<28, "i4", [I4, I5]>;
def I6_I7 : Rdi<30, "i6", [I6, I7]>;

// Aliases of the co-processor registers used for LDD/STD double-word operations
def C0_C1   : Rdi< 0, "c0",  [C0,  C1]>;
def C2_C3   : Rdi< 2, "c2",  [C2,  C3]>;
def C4_C5   : Rdi< 4, "c4",  [C4,  C5]>;
def C6_C7   : Rdi< 6, "c6",  [C6,  C7]>;
def C8_C9   : Rdi< 8, "c8",  [C8,  C9]>;
def C10_C11 : Rdi<10, "c10", [C10, C11]>;
def C12_C13 : Rdi<12, "c12", [C12, C13]>;
def C14_C15 : Rdi<14, "c14", [C14, C15]>;
def C16_C17 : Rdi<16, "c16", [C16, C17]>;
def C18_C19 : Rdi<18, "c18", [C18, C19]>;
def C20_C21 : Rdi<20, "c20", [C20, C21]>;
def C22_C23 : Rdi<22, "c22", [C22, C23]>;
def C24_C25 : Rdi<24, "c24", [C24, C25]>;
def C26_C27 : Rdi<26, "c26", [C26, C27]>;
def C28_C29 : Rdi<28, "c28", [C28, C29]>;
def C30_C31 : Rdi<30, "c30", [C30, C31]>;

// Register classes.
//
// FIXME: the register order should be defined in terms of the preferred
// allocation order...
//
// This register class should not be used to hold i64 values, use the I64Regs
// register class for that. The i64 type is included here to allow i64 patterns
// using the integer instructions.
def IntRegs : RegisterClass<"JJ", [i32, i64], 32,
                            (add (sequence "I%u", 0, 7),
                                 (sequence "G%u", 0, 7),
                                 (sequence "L%u", 0, 7),
                                 (sequence "O%u", 0, 7))>;

// Should be in the same order as IntRegs.
def IntPair : RegisterClass<"JJ", [v2i32], 64,
    (add I0_I1, I2_I3, I4_I5, I6_I7,
         G0_G1, G2_G3, G4_G5, G6_G7,
         L0_L1, L2_L3, L4_L5, L6_L7,
         O0_O1, O2_O3, O4_O5, O6_O7)>;

// Register class for 64-bit mode, with a 64-bit spill slot size.
// These are the same as the 32-bit registers, so TableGen will consider this
// to be a sub-class of IntRegs. That works out because requiring a 64-bit
// spill slot is a stricter constraint than only requiring a 32-bit spill slot.
def I64Regs : RegisterClass<"JJ", [i64], 64, (add IntRegs)>;

// Floating point register classes.
def FPRegs : RegisterClass<"JJ", [f32], 32, (sequence "F%u", 0, 31)>;
def DFPRegs : RegisterClass<"JJ", [f64], 64, (sequence "D%u", 0, 31)>;
def QFPRegs : RegisterClass<"JJ", [f128], 128, (sequence "Q%u", 0, 15)>;

// The Low?FPRegs classes are used only for inline-asm constraints.
def LowDFPRegs : RegisterClass<"JJ", [f64], 64, (sequence "D%u", 0, 15)>;
def LowQFPRegs : RegisterClass<"JJ", [f128], 128, (sequence "Q%u", 0, 7)>;

// Floating point control register classes.
def FCCRegs : RegisterClass<"JJ", [i1], 1, (sequence "FCC%u", 0, 3)>;

// GPR argument registers.
def GPROutgoingArg : RegisterClass<"JJ", [i32, i64], 32, (sequence "O%u", 0, 5)>;
def GPRIncomingArg : RegisterClass<"JJ", [i32, i64], 32, (sequence "I%u", 0, 5)>;

let isAllocatable = 0 in {
  // Ancillary state registers
  // FIXME: TICK is special-cased here as it can be accessed
  // from the ASR (as ASR4) or the privileged register set.
  // For now this is required for the parser to work.
  def ASRRegs : RegisterClass<"JJ", [i32], 32,
                              (add Y, TICK, (sequence "ASR%u", 1, 31))>;

  // This register class should not be used to hold i64 values.
  def CoprocRegs : RegisterClass<"JJ", [i32], 32,
                                (add (sequence "C%u", 0, 31))>;

  // Should be in the same order as CoprocRegs.
  def CoprocPair : RegisterClass<"JJ", [v2i32], 64,
    (add C0_C1,   C2_C3,   C4_C5,   C6_C7,
         C8_C9,   C10_C11, C12_C13, C14_C15,
         C16_C17, C18_C19, C20_C21, C22_C23,
         C24_C25, C26_C27, C28_C29, C30_C31)>;
}

// Privileged Registers
def PRRegs : RegisterClass<"JJ", [i64], 64,
    (add TPC, TNPC, TSTATE, TT, TICK, TBA, PSTATE, TL, PIL, CWP,
         CANSAVE, CANRESTORE, CLEANWIN, OTHERWIN, WSTATE, GL, VER)>;
