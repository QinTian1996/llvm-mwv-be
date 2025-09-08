//===-- Mwv208InstrInfo.cpp - Mwv208 Instruction Information
//----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Mwv208 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Mwv208InstrInfo.h"
#include "Mwv208.h"
#include "Mwv208MachineFunctionInfo.h"
#include "Mwv208Subtarget.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#include <iostream>

#define GET_INSTRINFO_CTOR_DTOR
#include "Mwv208GenInstrInfo.inc"

static cl::opt<unsigned> BPccDisplacementBits(
    "mwv208-bpcc-offset-bits", cl::Hidden, cl::init(19),
    cl::desc("Restrict range of BPcc/FBPfcc instructions (DEBUG)"));

static cl::opt<unsigned>
    BPrDisplacementBits("mwv208-bpr-offset-bits", cl::Hidden, cl::init(16),
                        cl::desc("Restrict range of BPr instructions (DEBUG)"));

// Pin the vtable to this file.
void Mwv208InstrInfo::anchor() {}

Mwv208InstrInfo::Mwv208InstrInfo(Mwv208Subtarget &ST)
    : Mwv208GenInstrInfo(JJ::ADJCALLSTACKDOWN, JJ::ADJCALLSTACKUP), RI(),
      Subtarget(ST) {}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
Register Mwv208InstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                              int &FrameIndex) const {
  if (MI.getOpcode() == JJ::LDri || MI.getOpcode() == JJ::LDXri ||
      MI.getOpcode() == JJ::LDFri || MI.getOpcode() == JJ::LDDFri ||
      MI.getOpcode() == JJ::LDQFri) {
    if (MI.getOperand(1).isFI() && MI.getOperand(2).isImm() &&
        MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
  }
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
Register Mwv208InstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  if (MI.getOpcode() == JJ::STri || MI.getOpcode() == JJ::STXri ||
      MI.getOpcode() == JJ::STFri || MI.getOpcode() == JJ::STDFri ||
      MI.getOpcode() == JJ::STQFri) {
    if (MI.getOperand(0).isFI() && MI.getOperand(1).isImm() &&
        MI.getOperand(1).getImm() == 0) {
      FrameIndex = MI.getOperand(0).getIndex();
      return MI.getOperand(2).getReg();
    }
  }
  return 0;
}

static JJCC::CondCodes GetOppositeBranchCondition(JJCC::CondCodes CC) {
  switch (CC) {
  case JJCC::ICC_A:
    return JJCC::ICC_N;
  case JJCC::ICC_N:
    return JJCC::ICC_A;
  case JJCC::ICC_NE:
    return JJCC::ICC_E;
  case JJCC::ICC_E:
    return JJCC::ICC_NE;
  case JJCC::ICC_G:
    return JJCC::ICC_LE;
  case JJCC::ICC_LE:
    return JJCC::ICC_G;
  case JJCC::ICC_GE:
    return JJCC::ICC_L;
  case JJCC::ICC_L:
    return JJCC::ICC_GE;
  case JJCC::ICC_GU:
    return JJCC::ICC_LEU;
  case JJCC::ICC_LEU:
    return JJCC::ICC_GU;
  case JJCC::ICC_CC:
    return JJCC::ICC_CS;
  case JJCC::ICC_CS:
    return JJCC::ICC_CC;
  case JJCC::ICC_POS:
    return JJCC::ICC_NEG;
  case JJCC::ICC_NEG:
    return JJCC::ICC_POS;
  case JJCC::ICC_VC:
    return JJCC::ICC_VS;
  case JJCC::ICC_VS:
    return JJCC::ICC_VC;

  case JJCC::FCC_A:
    return JJCC::FCC_N;
  case JJCC::FCC_N:
    return JJCC::FCC_A;
  case JJCC::FCC_U:
    return JJCC::FCC_O;
  case JJCC::FCC_O:
    return JJCC::FCC_U;
  case JJCC::FCC_G:
    return JJCC::FCC_ULE;
  case JJCC::FCC_LE:
    return JJCC::FCC_UG;
  case JJCC::FCC_UG:
    return JJCC::FCC_LE;
  case JJCC::FCC_ULE:
    return JJCC::FCC_G;
  case JJCC::FCC_L:
    return JJCC::FCC_UGE;
  case JJCC::FCC_GE:
    return JJCC::FCC_UL;
  case JJCC::FCC_UL:
    return JJCC::FCC_GE;
  case JJCC::FCC_UGE:
    return JJCC::FCC_L;
  case JJCC::FCC_LG:
    return JJCC::FCC_UE;
  case JJCC::FCC_UE:
    return JJCC::FCC_LG;
  case JJCC::FCC_NE:
    return JJCC::FCC_E;
  case JJCC::FCC_E:
    return JJCC::FCC_NE;

  case JJCC::CPCC_A:
    return JJCC::CPCC_N;
  case JJCC::CPCC_N:
    return JJCC::CPCC_A;
  case JJCC::CPCC_3:
    [[fallthrough]];
  case JJCC::CPCC_2:
    [[fallthrough]];
  case JJCC::CPCC_23:
    [[fallthrough]];
  case JJCC::CPCC_1:
    [[fallthrough]];
  case JJCC::CPCC_13:
    [[fallthrough]];
  case JJCC::CPCC_12:
    [[fallthrough]];
  case JJCC::CPCC_123:
    [[fallthrough]];
  case JJCC::CPCC_0:
    [[fallthrough]];
  case JJCC::CPCC_03:
    [[fallthrough]];
  case JJCC::CPCC_02:
    [[fallthrough]];
  case JJCC::CPCC_023:
    [[fallthrough]];
  case JJCC::CPCC_01:
    [[fallthrough]];
  case JJCC::CPCC_013:
    [[fallthrough]];
  case JJCC::CPCC_012:
    // "Opposite" code is not meaningful, as we don't know
    // what the CoProc condition means here. The cond-code will
    // only be used in inline assembler, so this code should
    // not be reached in a normal compilation pass.
    llvm_unreachable("Meaningless inversion of co-processor cond code");

  case JJCC::REG_BEGIN:
    llvm_unreachable("Use of reserved cond code");
  case JJCC::REG_Z:
    return JJCC::REG_NZ;
  case JJCC::REG_LEZ:
    return JJCC::REG_GZ;
  case JJCC::REG_LZ:
    return JJCC::REG_GEZ;
  case JJCC::REG_NZ:
    return JJCC::REG_Z;
  case JJCC::REG_GZ:
    return JJCC::REG_LEZ;
  case JJCC::REG_GEZ:
    return JJCC::REG_LZ;
  }
  llvm_unreachable("Invalid cond code");
}

static bool isUncondBranchOpcode(int Opc) { return Opc == JJ::BA; }

static bool isI32CondBranchOpcode(int Opc) {
  return Opc == JJ::BCOND || Opc == JJ::BPICC || Opc == JJ::BPICCA ||
         Opc == JJ::BPICCNT || Opc == JJ::BPICCANT;
}

static bool isI64CondBranchOpcode(int Opc) {
  return Opc == JJ::BPXCC || Opc == JJ::BPXCCA || Opc == JJ::BPXCCNT ||
         Opc == JJ::BPXCCANT;
}

static bool isRegCondBranchOpcode(int Opc) {
  return Opc == JJ::BPR || Opc == JJ::BPRA || Opc == JJ::BPRNT ||
         Opc == JJ::BPRANT;
}

static bool isFCondBranchOpcode(int Opc) {
  return Opc == JJ::FBCOND || Opc == JJ::FBCONDA || Opc == JJ::FBCOND_V9 ||
         Opc == JJ::FBCONDA_V9;
}

static bool isCondBranchOpcode(int Opc) {
  return isI32CondBranchOpcode(Opc) || isI64CondBranchOpcode(Opc) ||
         isRegCondBranchOpcode(Opc) || isFCondBranchOpcode(Opc);
}

static bool isIndirectBranchOpcode(int Opc) {
  return Opc == JJ::BINDrr || Opc == JJ::BINDri;
}

static void parseCondBranch(MachineInstr *LastInst, MachineBasicBlock *&Target,
                            SmallVectorImpl<MachineOperand> &Cond) {
  unsigned Opc = LastInst->getOpcode();
  int64_t CC = LastInst->getOperand(1).getImm();

  // Push the branch opcode into Cond too so later in insertBranch
  // it can use the information to emit the correct MWV208 branch opcode.
  Cond.push_back(MachineOperand::CreateImm(Opc));
  Cond.push_back(MachineOperand::CreateImm(CC));

  // Branch on register contents need another argument to indicate
  // the register it branches on.
  if (isRegCondBranchOpcode(Opc)) {
    Register Reg = LastInst->getOperand(2).getReg();
    Cond.push_back(MachineOperand::CreateReg(Reg, false));
  }

  Target = LastInst->getOperand(0).getMBB();
}

MachineBasicBlock *
Mwv208InstrInfo::getBranchDestBlock(const MachineInstr &MI) const {
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("unexpected opcode!");
  case JJ::BA:
  case JJ::BCOND:
  case JJ::BCONDA:
  case JJ::FBCOND:
  case JJ::FBCONDA:
  case JJ::BPICC:
  case JJ::BPICCA:
  case JJ::BPICCNT:
  case JJ::BPICCANT:
  case JJ::BPXCC:
  case JJ::BPXCCA:
  case JJ::BPXCCNT:
  case JJ::BPXCCANT:
  case JJ::BPFCC:
  case JJ::BPFCCA:
  case JJ::BPFCCNT:
  case JJ::BPFCCANT:
  case JJ::FBCOND_V9:
  case JJ::FBCONDA_V9:
  case JJ::BPR:
  case JJ::BPRA:
  case JJ::BPRNT:
  case JJ::BPRANT:
    return MI.getOperand(0).getMBB();
  }
}

bool Mwv208InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end())
    return false;

  if (!isUnpredicatedTerminator(*I))
    return false;

  // Get the last instruction in the block.
  MachineInstr *LastInst = &*I;
  unsigned LastOpc = LastInst->getOpcode();

  // If there is only one terminator instruction, process it.
  if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
    if (isUncondBranchOpcode(LastOpc)) {
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    }
    if (isCondBranchOpcode(LastOpc)) {
      // Block ends with fall-through condbranch.
      parseCondBranch(LastInst, TBB, Cond);
      return false;
    }
    return true; // Can't handle indirect branch.
  }

  // Get the instruction before it if it is a terminator.
  MachineInstr *SecondLastInst = &*I;
  unsigned SecondLastOpc = SecondLastInst->getOpcode();

  // If AllowModify is true and the block ends with two or more unconditional
  // branches, delete all but the first unconditional branch.
  if (AllowModify && isUncondBranchOpcode(LastOpc)) {
    while (isUncondBranchOpcode(SecondLastOpc)) {
      LastInst->eraseFromParent();
      LastInst = SecondLastInst;
      LastOpc = LastInst->getOpcode();
      if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
        // Return now the only terminator is an unconditional branch.
        TBB = LastInst->getOperand(0).getMBB();
        return false;
      } else {
        SecondLastInst = &*I;
        SecondLastOpc = SecondLastInst->getOpcode();
      }
    }
  }

  // If there are three terminators, we don't know what sort of block this is.
  if (SecondLastInst && I != MBB.begin() && isUnpredicatedTerminator(*--I))
    return true;

  // If the block ends with a B and a Bcc, handle it.
  if (isCondBranchOpcode(SecondLastOpc) && isUncondBranchOpcode(LastOpc)) {
    parseCondBranch(SecondLastInst, TBB, Cond);
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  }

  // If the block ends with two unconditional branches, handle it.  The second
  // one is not executed.
  if (isUncondBranchOpcode(SecondLastOpc) && isUncondBranchOpcode(LastOpc)) {
    TBB = SecondLastInst->getOperand(0).getMBB();
    return false;
  }

  // ...likewise if it ends with an indirect branch followed by an unconditional
  // branch.
  if (isIndirectBranchOpcode(SecondLastOpc) && isUncondBranchOpcode(LastOpc)) {
    I = LastInst;
    if (AllowModify)
      I->eraseFromParent();
    return true;
  }

  // Otherwise, can't handle this.
  return true;
}

unsigned Mwv208InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  assert(TBB && "insertBranch must not be told to insert a fallthrough");
  assert((Cond.size() <= 3) &&
         "Mwv208 branch conditions should have at most three components!");

  if (Cond.empty()) {
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(JJ::BA)).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded = 8;
    return 1;
  }

  // Conditional branch
  unsigned Opc = Cond[0].getImm();
  unsigned CC = Cond[1].getImm();
  if (isRegCondBranchOpcode(Opc)) {
    Register Reg = Cond[2].getReg();
    BuildMI(&MBB, DL, get(Opc)).addMBB(TBB).addImm(CC).addReg(Reg);
  } else {
    BuildMI(&MBB, DL, get(Opc)).addMBB(TBB).addImm(CC);
  }

  if (!FBB) {
    if (BytesAdded)
      *BytesAdded = 8;
    return 1;
  }

  BuildMI(&MBB, DL, get(JJ::BA)).addMBB(FBB);
  if (BytesAdded)
    *BytesAdded = 16;
  return 2;
}

unsigned Mwv208InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                       int *BytesRemoved) const {
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;
  int Removed = 0;
  while (I != MBB.begin()) {
    --I;

    if (I->isDebugInstr())
      continue;

    if (!isCondBranchOpcode(I->getOpcode()) &&
        !isUncondBranchOpcode(I->getOpcode()))
      break; // Not a branch

    Removed += getInstSizeInBytes(*I);
    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }

  if (BytesRemoved)
    *BytesRemoved = Removed;
  return Count;
}

bool Mwv208InstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  assert(Cond.size() <= 3);
  JJCC::CondCodes CC = static_cast<JJCC::CondCodes>(Cond[1].getImm());
  Cond[1].setImm(GetOppositeBranchCondition(CC));
  return false;
}

bool Mwv208InstrInfo::isBranchOffsetInRange(unsigned BranchOpc,
                                            int64_t Offset) const {
  assert((Offset & 0b11) == 0 && "Malformed branch offset");
  switch (BranchOpc) {
  case JJ::BA:
  case JJ::BCOND:
  case JJ::BCONDA:
  case JJ::FBCOND:
  case JJ::FBCONDA:
    return isIntN(22, Offset >> 2);

  case JJ::BPICC:
  case JJ::BPICCA:
  case JJ::BPICCNT:
  case JJ::BPICCANT:
  case JJ::BPXCC:
  case JJ::BPXCCA:
  case JJ::BPXCCNT:
  case JJ::BPXCCANT:
  case JJ::BPFCC:
  case JJ::BPFCCA:
  case JJ::BPFCCNT:
  case JJ::BPFCCANT:
  case JJ::FBCOND_V9:
  case JJ::FBCONDA_V9:
    return isIntN(BPccDisplacementBits, Offset >> 2);

  case JJ::BPR:
  case JJ::BPRA:
  case JJ::BPRNT:
  case JJ::BPRANT:
    return isIntN(BPrDisplacementBits, Offset >> 2);
  }

  llvm_unreachable("Unknown branch instruction!");
}

void Mwv208InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, MCRegister DestReg,
                                  MCRegister SrcReg, bool KillSrc,
                                  bool RenamableDest, bool RenamableSrc) const {
  if (JJ::IntRegsRegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, I, DL, get(JJ::ORrr), DestReg)
        .addReg(JJ::G0)
        .addReg(SrcReg, getKillRegState(KillSrc));
    std::cout << __LINE__ << std::endl;
  } else
    llvm_unreachable("Impossible reg-to-reg copy");
}

void Mwv208InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          Register SrcReg, bool isKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI,
                                          Register VReg,
                                          MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  MachineFunction *MF = MBB.getParent();
  const MachineFrameInfo &MFI = MF->getFrameInfo();
  MachineMemOperand *MMO = MF->getMachineMemOperand(
      MachinePointerInfo::getFixedStack(*MF, FI), MachineMemOperand::MOStore,
      MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  // On the order of operands here: think "[FrameIdx + 0] = SrcReg".
  if (RC == &JJ::I64RegsRegClass)
    BuildMI(MBB, I, DL, get(JJ::STXri))
        .addFrameIndex(FI)
        .addImm(0)
        .addReg(SrcReg, getKillRegState(isKill))
        .addMemOperand(MMO);
  else if (RC == &JJ::IntRegsRegClass)
    BuildMI(MBB, I, DL, get(JJ::STri))
        .addFrameIndex(FI)
        .addImm(0)
        .addReg(SrcReg, getKillRegState(isKill))
        .addMemOperand(MMO);
  else if (RC == &JJ::IntPairRegClass)
    BuildMI(MBB, I, DL, get(JJ::STDri))
        .addFrameIndex(FI)
        .addImm(0)
        .addReg(SrcReg, getKillRegState(isKill))
        .addMemOperand(MMO);
  else if (RC == &JJ::FPRegsRegClass)
    BuildMI(MBB, I, DL, get(JJ::STFri))
        .addFrameIndex(FI)
        .addImm(0)
        .addReg(SrcReg, getKillRegState(isKill))
        .addMemOperand(MMO);
  else if (JJ::DFPRegsRegClass.hasSubClassEq(RC))
    BuildMI(MBB, I, DL, get(JJ::STDFri))
        .addFrameIndex(FI)
        .addImm(0)
        .addReg(SrcReg, getKillRegState(isKill))
        .addMemOperand(MMO);
  else if (JJ::QFPRegsRegClass.hasSubClassEq(RC))
    // Use STQFri irrespective of its legality. If STQ is not legal, it will be
    // lowered into two STDs in eliminateFrameIndex.
    BuildMI(MBB, I, DL, get(JJ::STQFri))
        .addFrameIndex(FI)
        .addImm(0)
        .addReg(SrcReg, getKillRegState(isKill))
        .addMemOperand(MMO);
  else
    llvm_unreachable("Can't store this register to stack slot");
}

void Mwv208InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register DestReg,
    int FI, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
    Register VReg, MachineInstr::MIFlag Flags) const {
  return;
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  MachineFunction *MF = MBB.getParent();
  const MachineFrameInfo &MFI = MF->getFrameInfo();
  MachineMemOperand *MMO = MF->getMachineMemOperand(
      MachinePointerInfo::getFixedStack(*MF, FI), MachineMemOperand::MOLoad,
      MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  if (RC == &JJ::I64RegsRegClass)
    BuildMI(MBB, I, DL, get(JJ::LDXri), DestReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  else if (RC == &JJ::IntRegsRegClass)
    BuildMI(MBB, I, DL, get(JJ::LDri), DestReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  else if (RC == &JJ::IntPairRegClass)
    BuildMI(MBB, I, DL, get(JJ::LDDri), DestReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  else if (RC == &JJ::FPRegsRegClass)
    BuildMI(MBB, I, DL, get(JJ::LDFri), DestReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  else if (JJ::DFPRegsRegClass.hasSubClassEq(RC))
    BuildMI(MBB, I, DL, get(JJ::LDDFri), DestReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  else if (JJ::QFPRegsRegClass.hasSubClassEq(RC))
    // Use LDQFri irrespective of its legality. If LDQ is not legal, it will be
    // lowered into two LDDs in eliminateFrameIndex.
    BuildMI(MBB, I, DL, get(JJ::LDQFri), DestReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  else
    llvm_unreachable("Can't load this register from stack slot");
}

Register Mwv208InstrInfo::getGlobalBaseReg(MachineFunction *MF) const {
  Mwv208MachineFunctionInfo *Mwv208FI =
      MF->getInfo<Mwv208MachineFunctionInfo>();
  Register GlobalBaseReg = Mwv208FI->getGlobalBaseReg();
  if (GlobalBaseReg)
    return GlobalBaseReg;

  // Insert the set of GlobalBaseReg into the first MBB of the function
  MachineBasicBlock &FirstMBB = MF->front();
  MachineBasicBlock::iterator MBBI = FirstMBB.begin();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();

  const TargetRegisterClass *PtrRC =
      Subtarget.is64Bit() ? &JJ::I64RegsRegClass : &JJ::IntRegsRegClass;
  GlobalBaseReg = RegInfo.createVirtualRegister(PtrRC);

  DebugLoc dl;

  BuildMI(FirstMBB, MBBI, dl, get(JJ::GETPCX), GlobalBaseReg);
  Mwv208FI->setGlobalBaseReg(GlobalBaseReg);
  return GlobalBaseReg;
}