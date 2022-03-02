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
  pbessolve_parallel
  pbessymbolicbisim
  symbolic_exploration
  lps2lts_parallel
)

if(MCRL2_ENABLE_EXPERIMENTAL)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
