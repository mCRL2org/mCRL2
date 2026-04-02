mcrl2_add_tool(lps2lts
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)

# This adds projections, and changes the structure of the existing code
mcrl2_add_tool(lps2ltsproject1
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)
target_compile_definitions(lps2ltsproject1 PRIVATE MCRL2_USE_PROJECTIONS MCRL2_LPS2LTS_NEW_STRUCTURE)

# This adds projections, but leaves the existing code as is
mcrl2_add_tool(lps2ltsproject2
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)
target_compile_definitions(lps2ltsproject2 PRIVATE MCRL2_USE_PROJECTIONS)