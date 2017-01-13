set(MCRL2_TOOLS
  besconvert
  bessolve
  complps2pbes
  lpsbisim2pbes
  lpsrealelm
  lpsrealzone
  lps2torx
  pbesabsinthe
  pbesabstract
  pbesinst
  pbespareqelm
  txt2bes
  symbolic_exploration
)


foreach(TOOL ${MCRL2_TOOLS})
  add_subdirectory(${TOOL})
endforeach()
