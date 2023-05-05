set(MCRL2_TOOLS
  besconvert  
  lps2torx
  lps2pres
  lpscleave
  lpscombine
  lpsrealelm
  lpssymbolicbisim
  pbes2cvc4
  pbes2yices
  pbesabsinthe
  pbespareqelm
  pbespor
  pbessymbolicbisim
  prespp
  presinfo
  symbolic_exploration
  respp
  resinfo
  txt2pres
  txt2res
)

if(MCRL2_ENABLE_EXPERIMENTAL)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
