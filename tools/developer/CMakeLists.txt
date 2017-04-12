set(MCRL2_TOOLS
  lpssymbolicbisim
  lpstransform
  mcrl2parse
  mcrl2transform
  pbestransform
  reduce_pbes
)

if(MCRL2_ENABLE_DEVELOPER)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
