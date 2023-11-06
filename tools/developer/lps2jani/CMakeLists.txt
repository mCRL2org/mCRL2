if(MCRL2_ENABLE_BOOST_JSON_SUPPORT)

  mcrl2_add_tool(lps2jani
    SOURCES
      lps2jani.cpp
    DEPENDS
      mcrl2_lps
      mcrl2_lts
  )

endif()
