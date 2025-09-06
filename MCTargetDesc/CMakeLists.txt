add_llvm_component_library(LLVMMwv208Desc
  Mwv208AsmBackend.cpp
  Mwv208ELFObjectWriter.cpp
  Mwv208InstPrinter.cpp
  Mwv208MCAsmInfo.cpp
  Mwv208MCCodeEmitter.cpp
  Mwv208MCTargetDesc.cpp
  Mwv208MCExpr.cpp
  Mwv208TargetStreamer.cpp

  LINK_COMPONENTS
  MC
  Mwv208Info
  Support
  TargetParser

  ADD_TO_COMPONENT
  Mwv208
  )
