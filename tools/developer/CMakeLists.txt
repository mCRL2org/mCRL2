set(MCRL2_TOOLS
  lpstransform
  mcrl2parse
  mcrl2transform
  pbestransform
  reduce_pbes
  reduce_process
)

# N.B. The developer tool lpstransform is needed for the random tests.
if(${MCRL2_ENABLE_DEVELOPER} OR ${MCRL2_ENABLE_TESTS})
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
