set(MCRL2_TOOLS
  confcheck
  lps2ltsdeprecated
  lpstransform
  mcrl2rewrite
  mcrl2transform
  mcrl2typecheck
  pbes2bdd
  pbestransform
  reduce_pbes
  reduce_process
)

if (UNIX)
  list(APPEND MCRL2_TOOLS pbesbddsolve)
  list(APPEND MCRL2_TOOLS lpsreach)
  list(APPEND MCRL2_TOOLS pbesreach)
  list(APPEND MCRL2_TOOLS pbessolvesymbolic)
endif (UNIX)

# N.B. Some developer tools are needed for the random tests.
if(${MCRL2_ENABLE_DEVELOPER} OR ${MCRL2_ENABLE_TESTS})
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
