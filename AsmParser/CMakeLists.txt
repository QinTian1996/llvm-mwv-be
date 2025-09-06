add_llvm_component_library(LLVMMwv208AsmParser
  Mwv208AsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  Mwv208Desc
  Mwv208Info
  Support
  TargetParser

  ADD_TO_COMPONENT
  Mwv208
  )
