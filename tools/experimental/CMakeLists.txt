set(MCRL2_TOOLS
  besconvert  
  lps2torx
  lpscleave
  lpscombine
  lpsrealelm
  lpssymbolicbisim
  pbesabsinthe
  pbespareqelm
  pbespor
  pbessymbolicbisim
  symbolic_exploration
)

if(MCRL2_ENABLE_EXPERIMENTAL)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
