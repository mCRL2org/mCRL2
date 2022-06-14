set(MCRL2_TOOLS
  besconvert  
  lps2torx
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
  symbolic_exploration
)

if(MCRL2_ENABLE_SYLVAN)
  list(APPEND MCRL2_TOOLS lpsreach)
  list(APPEND MCRL2_TOOLS pbessolvesymbolic)
endif()

if(MCRL2_ENABLE_EXPERIMENTAL)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
