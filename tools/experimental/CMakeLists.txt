set(MCRL2_TOOLS
  besconvert
  bessolve
  complps2pbes
  lpsbisim2pbes
  lpsrealelm
  lpsrealzone
  lpssymbolicbisim
  pbesabsinthe
  pbesinst
  pbespareqelm
  txt2bes
  symbolic_exploration
)

if(MCRL2_ENABLE_EXPERIMENTAL)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
