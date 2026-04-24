mcrl2_add_tool(ltscombine
  SOURCES
    ltscombine.cpp
    lts_combine.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)

if(MCRL2_ENABLE_TESTS)
  mcrl2_add_tool_tests(ltscombine "test/" "tooltest"
    TEST_SOURCES
      lts_combine.cpp
    DEPENDS
      mcrl2_lps
      mcrl2_lts
    INCLUDE_DIRS
      "."
  )
endif()
