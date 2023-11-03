if(MCRL2_ENABLE_SYLVAN)

  mcrl2_add_tool(lpsreach
    SOURCES
      lpsreach.cpp
    DEPENDS
      mcrl2_lps
  )

endif()
