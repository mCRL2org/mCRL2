if(MCRL2_ENABLE_SYLVAN)

  add_mcrl2_tool(ltsconvertsymbolic
    SOURCES
      bdd_util.cpp
      ltsconvertsymbolic.cpp
      sigref.cpp
      symbolic_lts_bdd.cpp
    DEPENDS
      mcrl2_lps
      mcrl2_lts
  )

endif()
