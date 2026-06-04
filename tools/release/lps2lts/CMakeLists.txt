mcrl2_add_tool(lps2lts
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)

# This adds projections
mcrl2_add_tool(lps2ltspr
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
)
target_compile_definitions(lps2ltspr PRIVATE MCRL2_USE_PROJECTIONS)

# This adds control flow summand pruning
mcrl2_add_tool(lps2ltscf
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
    mcrl2_pbes
)
target_compile_definitions(lps2ltscf PRIVATE MCRL2_USE_CONTROL_FLOW)
