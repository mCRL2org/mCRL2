set(MCRL2_TOOLS
  confcheck
  lpstransform
  mcrl2rewrite
  mcrl2transform
  mcrl2typecheck
  pbes2bdd
  pbestransform
  reduce_pbes
  reduce_process
)

if(MCRL2_ENABLE_SYLVAN)
  list(APPEND MCRL2_TOOLS ltsconvertsymbolic)
  list(APPEND MCRL2_TOOLS pbesbddsolve)
endif()

# N.B. Some developer tools are needed for the random tests.
if(${MCRL2_ENABLE_DEVELOPER} OR ${MCRL2_ENABLE_TESTS})
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
