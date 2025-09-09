//===-- Mwv208InstrFormats.td - Mwv208 Instruction Formats ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class MWV208Inst<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
  : Instruction {
  let Namespace = "MWV208";
  let OutOperandList = outs;  // 输出操作数
  let InOperandList = ins;   // 输入操作数
  let AsmString = asmstr;    // 汇编格式
  let Pattern = pattern;     // 对应的DAG模式

  // 指令长度：128位（16字节）
  let Size = 16;
  field bits<128> Inst; // 指令编码字段

  // General Format Inst Word 0
  bits<6> OP_CODE = opcode;  // 操作码，占6位
  bits<5> CONDITION_CODE = 0; // 条件码，占5位
  bits<1> SATURATE = 0; // 饱和标志，占1位
  bits<1> DEST_VALID = 0; // 目的操作数有效标志，占2位
  bits<3> DEST_REL_ADR = 0; // 目的操作数相对地址，占3位
  bits<7> DEST_ADR = 0; // 目的操作数地址，占7位
  bits<4> DEST_WRITE_ENABLE = 0; // 目的操作数写使能，占4位
  bits<5> SAMPLER_NUM = 0; // 采样器编号，占5位 也用作destMask4:8
  let Inst{5-0}   = OP_CODE;
  let Inst{10-6}  = CONDITION_CODE;
  let Inst{11-11} = SATURATE;
  let Inst{12-12} = DEST_VALID;
  let Inst{15-13} = DEST_REL_ADR;
  let Inst{22-16} = DEST_ADR;
  let Inst{26-23} = DEST_WRITE_ENABLE;
  let Inst{31-27} = SAMPLER_NUM;

  // General Format Inst Word 1
  bits<3> SAMPLER_REL_ADR = 0; // 采样器相对地址，占3位
  bits<8> SAMPLER_SWIZZLE = 0; // 采样器swizzle，占8位
  bits<1> SRC0_VALID = 0; // 源操作数0有效标志，占1位
  bits<9> SRC0_ADR = 0; // 源操作数0地址，占9位
  bits<1> INST_TYPE_0 = 0; // 指令类型，占1位
  bits<8> SRC0_SWIZZLE = 0; // 源操作数0swizzle，占8位
  bits<1> SRC0_MODIFIER_NEG = 0; // 源操作数0取反标志，占1位
  bits<1> SRC0_MODIFIER_ABS = 0; // 源操作数0取绝对值标志，占1位
  let Inst{34-32} = SAMPLER_REL_ADR;
  let Inst{42-35} = SAMPLER_SWIZZLE;
  let Inst{43-43} = SRC0_VALID;
  let Inst{52-44} = SRC0_ADR;
  let Inst{53-53} = INST_TYPE_0;
  let Inst{61-54} = SRC0_SWIZZLE;
  let Inst{62-62} = SRC0_MODIFIER_NEG;
  let Inst{63-63} = SRC0_MODIFIER_ABS;

  // General Format Inst Word 2
  bits<3> SRC0_REL_ADR = 0; // 源操作数0相对地址，占3位
  bits<3> SRC0_type = 0; // 源操作数0类型，占3位
  bits<1> SRC1_VALID = 0; // 源操作数1有效标志，占1位
  bits<9> SRC1_ADR = 0; // 源操作数1地址，占9位
  bits<1> OP_CODE_MSB6 = 0; // ???，占1位\
  bits<8> SRC1_SWIZZLE = 0; // 源操作数1swizzle，占8位
  bits<1> SRC1_MODIFIER_NEG = 0; // 源操作数1取反标志，占1位
  bits<1> SRC1_MODIFIER_ABS = 0; // 源操作数1取绝对值标志，占1位
  bits<3> SRC1_REL_ADR = 0; // 源操作数1相对地址，占3位
  bits<2> INST_TYPE_1 = 0; // 指令类型，占2位
  let Inst{66-64} = SRC0_REL_ADR;
  let Inst{69-67} = SRC0_type;
  let Inst{70-70} = SRC1_VALID;
  let Inst{79-71} = SRC1_ADR;
  let Inst{80-80} = OP_CODE_MSB6;
  let Inst{88-81} = SRC1_SWIZZLE;
  let Inst{89-89} = SRC1_MODIFIER_NEG;
  let Inst{90-90} = SRC1_MODIFIER_ABS;
  let Inst{93-91} = SRC1_REL_ADR;
  let Inst{95-94} = INST_TYPE_1;
  
}

/* Gerneral Format Inst*/
class MWV208GFInst<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
  : MWV208Inst<outs, ins, asmstr, pattern, opcode> {
  // General Format Inst Word 3
  bits<3> SRC1_TYPE = 0; // 源操作数1类型，占3位
  bits<1> SRC2_VALID = 0; // 源操作数2有效标志，占1位
  bits<9> SRC2_ADR = 0; // 源操作数2地址, 占9位
  bits<1> DEST_ADR_MSB7 = 0; // 目的操作数地址高位，占1位
  bits<8> SRC2_SWIZZLE = 0; // 源操作数2swizzle，占8位
  bits<1> SRC2_MODIFIER_NEG = 0; // 源操作数2取反标志，占1位
  bits<1> SRC2_MODIFIER_ABS = 0; // 源操作数2取绝对值标志，占1位
  bits<1> DEST_ADR_MSB8 = 0; // 目的操作数地址更高位，占1位
  bits<3> SRC2_REL_ADR = 0; // 源操作数2相对地址，占3位
  bits<3> SRC2_TYPE = 0; // 源操作数2类型，占3位
  bits<1> DEST_SOURCE = 0; // 目的操作数来源，占1位
  let Inst{98-96} =   SRC1_TYPE;
  let Inst{99-99} =   SRC2_VALID;
  let Inst{108-100} = SRC2_ADR;
  let Inst{109-109} = DEST_ADR_MSB7;
  let Inst{117-110} = SRC2_SWIZZLE;
  let Inst{118-118} = SRC2_MODIFIER_NEG;
  let Inst{119-119} = SRC2_MODIFIER_ABS;
  let Inst{120-120} = DEST_ADR_MSB8;
  let Inst{123-121} = SRC2_REL_ADR;
  let Inst{126-124} = SRC2_TYPE;
  let Inst{127-127} = DEST_SOURCE;

}

/* Control Flow Format Inst*/
class MWV208FCFInst<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
  : MWV208Inst<outs, ins, asmstr, pattern, opcode> {
  // Control Flow Format Inst Word 3
  bits<3> SRC1_TYPE = 0; // 源操作数1类型，占3位
  bits<1> SRC2_VALID = 0; // 源操作数2有效标志，占1位
  bits<1> LOOP_OP = 0; // 循环操作码，占1位
  bits<2> RESERVED = 0; // 保留位，占2位
  bits<20>Target = 0; // 目标地址，占20位
  let Inst{98-96} = SRC1_TYPE;
  let Inst{99-99} = SRC2_VALID;
  let Inst{100-100} = LOOP_OP;
  let Inst{102-101} = RESERVED;
  let Inst{122-103} = Target;

}