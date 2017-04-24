set(MCRL2_TOOLS
  lpstransform
  mcrl2parse
  mcrl2transform
  pbestransform
  reduce_pbes
)

# if(MCRL2_ENABLE_DEVELOPER)  Outcommented as lpstransform is needed 
# for tests, while this flag is not set in TeamCity when automatic tests 
# are performed. This should be adapted elsewhere, but as it stands I do
# not know where. 
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
# endif()
