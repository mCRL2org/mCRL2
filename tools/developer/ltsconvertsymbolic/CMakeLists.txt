if(MCRL2_ENABLE_SYLVAN)

  mcrl2_add_tool(ltsconvertsymbolic
    SOURCES
      ltsconvertsymbolic.cpp
    DEPENDS
      mcrl2_lps
      mcrl2_lts
      sylvan
  )

  endif()