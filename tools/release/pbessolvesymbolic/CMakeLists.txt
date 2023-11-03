if(MCRL2_ENABLE_SYLVAN)

  mcrl2_add_tool(pbessolvesymbolic
    SOURCES
      pbessolvesymbolic.cpp
    DEPENDS
      mcrl2_pbes
  )

endif()
