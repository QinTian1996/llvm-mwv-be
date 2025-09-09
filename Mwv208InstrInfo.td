//===-- Mwv208InstrInfo.td - Target Description for Mwv208 Target -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the Mwv208 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "Mwv208InstrFormats.td"

//===----------------------------------------------------------------------===//
// Feature predicates.
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// Instruction Pattern Stuff
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//
/* 
// 抄的NVPTX的

// Helper class to aid conversion between ValueType and a matching RegisterClass.

class ValueToRegClass<ValueType T> {
   string name = !cast<string>(T);
   TempRegClass ret = !cond(
     !eq(name, "i1"): Int1Regs,
     !eq(name, "i16"): Int16Regs,
     !eq(name, "v2i16"): Int32Regs,
     !eq(name, "i32"): Int32Regs,
     !eq(name, "i64"): Int64Regs,
     !eq(name, "f16"): Int16Regs,
     !eq(name, "v2f16"): Int32Regs,
     !eq(name, "bf16"): Int16Regs,
     !eq(name, "v2bf16"): Int32Regs,
     !eq(name, "f32"): Float32Regs,
     !eq(name, "f64"): Float64Regs,
     !eq(name, "ai32"): Int32ArgRegs,
     !eq(name, "ai64"): Int64ArgRegs,
     !eq(name, "af32"): Float32ArgRegs,
     !eq(name, "if64"): Float64ArgRegs,
    );
}
// Utility class to wrap up information about a register and DAG type for more
// convenient iteration and parameterization
class RegTyInfo<ValueType ty, Mwv208Reg rc, Operand imm> {
  ValueType Ty = ty;
  Mwv208Reg RC = rc;
  Operand Imm = imm;
  int Size = ty.Size;
}

//all type goes to GPR, so no other reg class(MWV208I32REG) is needed
def I8RT  : RegTyInfo<i8 , Mwv208Reg, i8imm >;
def I16RT : RegTyInfo<i16, Mwv208Reg, i16imm>;
def I32RT : RegTyInfo<i32, Mwv208Reg, i32imm>;

// Template for instructions which take three int64, int32, or int16 args.
// The instructions are named "<OpcStr><Width>" (e.g. "add.s64").
multiclass I3<string OpcStr, SDNode OpNode, bit commutative, bits<5> opcode> {
  foreach t = [I8RT, I16RT, I32RT] in {
    defvar asmstr = OpcStr # t.Size # " \t$dst, $a, $b;";

     def t.Ty # rr :
      MWV208GFInst<(outs t.RC:$dst), (ins t.RC:$a, t.RC:$b),
                asmstr,
                [(set t.Ty:$dst, (OpNode t.Ty:$a, t.Ty:$b))]> {
        let OP_CODE = opcode;
      };
    def t.Ty # ri :
      MWV208GFInst<(outs t.RC:$dst), (ins t.RC:$a, t.Imm:$b),
                asmstr,
                [(set t.Ty:$dst, (OpNode t.RC:$a, imm:$b))]> {
        let OP_CODE = opcode;
      };
    if !not(commutative) then
      def t.Ty # ir :
        MWV208GFInst<(outs t.RC:$dst), (ins t.Imm:$a, t.RC:$b),
                  asmstr,
                  [(set t.Ty:$dst, (OpNode imm:$a, t.RC:$b))]> {
          let OP_CODE = opcode;
        };
    }
}
// int16, int32, and int8 signed addition.
defm ADD : I3<"add.s", add, /*commutative=*/ true, 0x1>;
 */

//include "Mwv208RegisterInfo.td"

class RegTyInfo<ValueType ty, MWV208RegClass rc, Operand imm> {
  ValueType Ty = ty;
  MWV208RegClass RC = rc;
  Operand Imm = imm;
  int Size = ty.Size;
}

def I16RT : RegTyInfo<i16, TempRegClass, i16imm>;
def I32RT : RegTyInfo<i32, TempRegClass, i32imm>;
def I64RT : RegTyInfo<i64, TempRegClass, i64imm>;

// Template for instructions which take three int64, int32, or int16 args.
// The instructions are named "<OpcStr><Width>" (e.g. "add.s64").
/* multiclass I3<string OpcStr, SDNode OpNode, bit commutative, bits<6> opcode> {
  foreach t = [I32RT] in {
    defvar asmstr = OpcStr # t.Size # " \t$dst, $a, $b;";

    def t.Ty # rr :
      MWV208GFInst<(outs t.RC:$dst), (ins t.RC:$a, t.RC:$b),
                asmstr,
                [(set t.Ty:$dst, (OpNode t.Ty:$a, t.Ty:$b))], opcode> {
                  
                }
     def t.Ty # ri :
      MWV208GFInst<(outs t.RC:$dst), (ins t.RC:$a, t.Imm:$b),
                asmstr,
                [(set t.Ty:$dst, (OpNode t.RC:$a, imm:$b))], opcode> {
                  let SRC2_TYPE = 0x7; // 立即数
                }
    if !not(commutative) then
      def t.Ty # ir :
        MWV208GFInst<(outs t.RC:$dst), (ins t.Imm:$a, t.RC:$b),
                  asmstr,
                  [(set t.Ty:$dst, (OpNode imm:$a, t.RC:$b))], opcode> {
                    let SRC0_TYPE = 0x7; // 立即数
                  } 
  }
} */

// 定义需要支持的数据类型
/* def TypeList : list<TypeInfo> = [
  TypeInfo<i8,  TempRegClass,  "i8">,   // 8位整数
  TypeInfo<i16, TempRegClass, "i16">,   // 16位整数
  TypeInfo<i32, TempRegClass, "i32">    // 32位整数
]; */
//TODO: add other types

//NV的I3模板不适用于MWV208, 208的格式过于自由
// 内层multiclass：枚举操作数组合（rr/ri/ir）
// 参数：操作码前缀（如"add"）、DAG节点（如add）、是否交换律、类型信息
// multiclass OpVariants<string Opc, SDNode OpNode, bit Commutative, TypeInfo T> {
//   // 1. 寄存器-寄存器（rr）变体：$dst = $a + $b
//   def _rr : MWV208Inst<
//     (outs T.RC:$dst),                   // 输出：当前类型的寄存器
//     (ins T.RC:$a, T.RC:$b),             // 输入：两个同类型寄存器
//     !strconcat(Opc, ".", T.TypeName, " $dst, $a, $b"),  // 汇编格式：add.i32 $d, $a, $b
//     [(set T.Ty:$dst, (OpNode T.Ty:$a, T.Ty:$b))]        // DAG模式：匹配同类型寄存器操作
//   >;

//   // 2. 寄存器-立即数（ri）变体：$dst = $a + imm
//   def _ri : MWV208Inst<
//     (outs T.RC:$dst),
//     (ins T.RC:$a, !cast<ImmLeaf>(T.TypeName # "imm"):$b),  // 立即数类型：如i32imm
//     !strconcat(Opc, ".", T.TypeName, " $dst, $a, $b"),
//     [(set T.Ty:$dst, (OpNode T.Ty:$a, imm:$b))]            // DAG模式：匹配寄存器+立即数
//   >;

//   // 3. 立即数-寄存器（ir）变体：仅非交换操作（如sub）需要
//   if !not(Commutative) then {  // 条件：非交换律操作（Commutative=0）
//     def _ir : MWV208Inst<
//       (outs T.RC:$dst),
//       (ins !cast<ImmLeaf>(T.TypeName # "imm"):$a, T.RC:$b),  // 立即数+寄存器
//       !strconcat(Opc, ".", T.TypeName, " $dst, $a, $b"),
//       [(set T.Ty:$dst, (OpNode imm:$a, T.Ty:$b))]            // DAG模式：匹配立即数+寄存器
//     >;
//   }
// }

// //208的ISA是矩阵, 每条指令都可以接收多种格式的数据...
// // 外层multiclass：枚举数据类型，调用内层生成操作数变体
// //不支持的格式暂时在cpp进行拦截(不做映射)
// multiclass ArithInsts<string Opc, SDNode OpNode, bit Commutative, bit doI32, bit doI16 > {
//   // 遍历所有数据类型
//   foreach T = [
//   TypeInfo<i8,  TempRegClass,  "i8">,   // 8位整数
//   TypeInfo<i16, TempRegClass, "i16">,   // 16位整数
//   TypeInfo<i32, TempRegClass, "i32">    // 32位整数
//    ] in {
//     // 嵌套调用内层multiclass，传递当前类型T及其他参数
//     defm _#T.TypeName : OpVariants<Opc, OpNode, Commutative, T>;
//   }
// }

///////////////////////////////////////////////////////////////////////////////////
// MWV208 Instruction
///////////////////////////////////////////////////////////////////////////////////

//defm ADD : I3<"add", add, 1, 0x1>;

def ADD : MWV208GFInst<
  (outs TempRegClass:$dst),
  (ins TempRegClass:$a, TempRegClass:$b),
  "add.s32 \t$dst, $a, $b",
  [(set i32:$dst, (add i32:$a, i32:$b))],
  0x01> {
    let OP_CODE = 0x01;
}

include "Mwv208InstrAliases.td"
