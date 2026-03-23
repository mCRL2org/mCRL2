mcrl2_add_tool(lps2lts
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)

mcrl2_add_tool(lps2ltsproject
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)
target_compile_definitions(lps2ltsproject PRIVATE MCRL2_USE_PROJECTIONS)
