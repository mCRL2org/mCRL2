set(MCRL2_TOOLS
  lpstransform
  mcrl2parse
  mcrl2transform
  pbestransform
  reduce_pbes
)

foreach(TOOL ${MCRL2_TOOLS})
  add_subdirectory(${TOOL})
endforeach()
