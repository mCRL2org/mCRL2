set(MCRL2_TOOLS
  besconvert  
  lps2torx
  lpscleave
  lpscombine
  lpsrealelm
  lpsstategraph
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
