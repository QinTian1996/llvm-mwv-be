//===-- Mwv208CallingConv.td - Calling Conventions Mwv208 ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Mwv208 architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// MWV208 32-bit.
//===----------------------------------------------------------------------===//

def CC_Mwv20832 : CallingConv<[
  // Custom assign SRet to [sp+64].
  CCIfSRet<CCCustom<"CC_Mwv208_Assign_SRet">>,
  // i32 f32 arguments get passed in integer registers if there is space.
  CCIfType<[i32, f32], CCAssignToReg<[I0, I1, I2, I3, I4, I5]>>,
  // f64 arguments are split and passed through registers or through stack.
  CCIfType<[f64], CCCustom<"CC_Mwv208_Assign_Split_64">>,
  // As are v2i32 arguments (this would be the default behavior for
  // v2i32 if it wasn't allocated to the IntPair register-class)
  CCIfType<[v2i32], CCCustom<"CC_Mwv208_Assign_Split_64">>,


  // Alternatively, they are assigned to the stack in 4-byte aligned units.
  CCAssignToStack<4, 4>
]>;

def RetCC_Mwv20832 : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[I0, I1, I2, I3, I4, I5]>>,
  CCIfType<[f32], CCAssignToReg<[F0, F1, F2, F3]>>,
  CCIfType<[f64], CCAssignToReg<[D0, D1]>>,
  CCIfType<[v2i32], CCCustom<"CC_Mwv208_Assign_Ret_Split_64">>
]>;

// Callee-saved registers are handled by the register window mechanism.
def CSR : CalleeSavedRegs<(add)> {
  let OtherPreserved = (add (sequence "I%u", 0, 7),
                            (sequence "L%u", 0, 7), O6);
}

// Callee-saved registers for calls with ReturnsTwice attribute.
def RTCSR : CalleeSavedRegs<(add)> {
  let OtherPreserved = (add I6, I7);
}
