add_llvm_component_library(LLVMMwv208Info
  Mwv208TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  Mwv208
  )
