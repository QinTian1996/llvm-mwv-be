add_llvm_component_group(Mwv208)

set(LLVM_TARGET_DEFINITIONS Mwv208.td)

tablegen(LLVM Mwv208GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM Mwv208GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM Mwv208GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM Mwv208GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM Mwv208GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Mwv208GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM Mwv208GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Mwv208GenSearchableTables.inc -gen-searchable-tables)
tablegen(LLVM Mwv208GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(Mwv208CommonTableGen)

add_llvm_target(Mwv208CodeGen
  LeonPasses.cpp
  Mwv208AsmPrinter.cpp
  Mwv208InstrInfo.cpp
  Mwv208ISelDAGToDAG.cpp
  Mwv208ISelLowering.cpp
  Mwv208FrameLowering.cpp
  Mwv208MachineFunctionInfo.cpp
  Mwv208RegisterInfo.cpp
  Mwv208Subtarget.cpp
  Mwv208TargetMachine.cpp
  Mwv208MCInstLower.cpp
  Mwv208TargetObjectFile.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  Mwv208Desc
  Mwv208Info
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  Mwv208
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
