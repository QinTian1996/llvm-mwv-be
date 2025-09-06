add_llvm_component_library(LLVMMwv208Disassembler
  Mwv208Disassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  MC
  Mwv208Info
  Support

  ADD_TO_COMPONENT
  Mwv208
  )
