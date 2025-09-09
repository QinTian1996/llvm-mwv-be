//===-- Mwv208RegisterInfo.td - Mwv208 Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


///////////////////////////////////////////////////////////////////////////////
// MWV208 Register Types
///////////////////////////////////////////////////////////////////////////////

class Mwv208Reg<string n> : Register<n> {
  let Namespace = "MWV208";
}



///////////////////////////////////////////////////////////////////////////////
// MWV208 Register Declarations
///////////////////////////////////////////////////////////////////////////////
/* 
def subx : SubRegIndex<32>;  // 第0个int（低32位）
def suby : SubRegIndex<32, 32>;  // 第1个int（32~63位）
def subz : SubRegIndex<32, 64>;  // 第2个int（64~95位）
def subw : SubRegIndex<32, 96>;  // 第3个int（96~127位）
// 32个通用寄存器的32位分量，vec4i32类型, 名称为r0x, r0y, r0z, r0w, ..., r31x, r31y, r31z, r31w
foreach i = 0...31 in {
  // 寄存器名称为"r" + 索引（如r0, r1, ..., r31）
  def rx#i : Mwv208Reg<"rx"#i>;
  def ry#i : Mwv208Reg<"ry"#i>;
  def rz#i : Mwv208Reg<"rz"#i>;
  def rw#i : Mwv208Reg<"rw"#i>;

  def r#i : Mwv208Reg<"r"#i> {
    let SubRegs = [(rx#i), (ry#i), (rz#i), (rw#i)];           // 关联分量子寄存器
    let SubRegIndices = [subx, suby, subz, subw];     // 绑定x/y/z/w索引
  }
}

//32个constant寄存器, 格式同上
foreach i = 0-31 in {
  // 寄存器名称为"r" + 索引（如c0, c1, ..., c31）
  def cx#i : Mwv208Reg<"cx"#i>;
  def cy#i : Mwv208Reg<"cy"#i>;
  def cz#i : Mwv208Reg<"cz"#i>;
  def cw#i : Mwv208Reg<"cw"#i>;

  def c#i : Mwv208Reg<"c"#i> {
    let SubRegs = [(cx#i), (cy#i), (cz#i), (cw#i)];           // 关联分量子寄存器
    let SubRegIndices = [subx, suby, subz, subw];     // 绑定x/y/z/w索引
  }
}
// 32位分量寄存器类
def Component32 : RegisterClass<"MWV208", [i32], 32,
  (add (sequence "rx%u", 0, 31), (sequence "rx%u", 0, 31), (sequence "rz%u", 0, 31)
    , (sequence "rw%u", 0, 31), (sequence "cx%u", 0, 31), (sequence "cy%u", 0, 31)
    , (sequence "cz%u", 0, 31), (sequence "cw%u", 0, 31))>;
*/
// 目前看没必要使用i32分量, 直接在codegen时处理好src0/1/2.swizzle即可

foreach i = 0...31 in {
  // r->TempRegClass
  // c->ConstRegClass
  def r#i : Mwv208Reg<"r"#i>;
  def c#i : Mwv208Reg<"c"#i>;
}

class MWV208RegClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName> : RegisterClass <namespace, regTypes, alignment,
                      regList, idx>;

// 128位向量寄存器类（支持所有类型, 啥都往里装）
def TempRegClass  : MWV208RegClass<"MWV208", [i8, i16, i32, i64, f16, f32, f64, v4i32], 128, (add (sequence "r%u", 0, 31))>;
def ConstRegClass : MWV208RegClass<"MWV208", [i8, i16, i32, i64, f16, f32, f64, v4i32], 128, (add (sequence "c%u", 0, 31))>;

//ref: isa文档, 第四章Register Types
//TODO: other temp types, A/B type, PC, FACE, RETURNSTACK