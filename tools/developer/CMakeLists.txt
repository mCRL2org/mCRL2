set(MCRL2_TOOLS
  lpstransform
  mcrl2transform
  mcrl2typecheck
  pbes2bdd
  pbessolve
  pbestransform
  reduce_pbes
  reduce_process
)

# N.B. Some developer tools are needed for the random tests.
if(${MCRL2_ENABLE_DEVELOPER} OR ${MCRL2_ENABLE_TESTS})
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
