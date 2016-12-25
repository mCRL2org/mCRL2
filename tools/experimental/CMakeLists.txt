set(MCRL2_TOOLS
  besconvert
  bessolve
  complps2pbes
  lpsbisim2pbes
  lpsrealelm
  lpsrealzone
  lpstransform
  lps2torx
  mcrl2transform
  pbesabsinthe
  pbesabstract
  pbesinst
  pbespareqelm
  pbestransform
  txt2bes
  symbolic_exploration
)


foreach(TOOL ${MCRL2_TOOLS})
  add_subdirectory(${TOOL})
endforeach()
