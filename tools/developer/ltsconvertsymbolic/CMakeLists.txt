if(MCRL2_ENABLE_SYLVAN)

  add_mcrl2_tool(ltsconvertsymbolic
    SOURCES
      ltsconvertsymbolic.cpp
      sigref.cpp
    DEPENDS
      mcrl2_lps
      mcrl2_lts
  )

endif()