set(MCRL2_TOOLS
  lpssymbolicbisim
  lpstransform
  mcrl2parse
  mcrl2transform
  pbestransform
  reduce_pbes
)

# if(MCRL2_ENABLE_DEVELOPER)  Outcommented to figure out whether this is the cause of massively failing tests. 
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
# endif()
