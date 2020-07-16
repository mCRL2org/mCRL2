add_mcrl2_tool(pbesbddsolve
  SOURCES
    pbesbddsolve.cpp
  DEPENDS
    mcrl2_pbes
    mcrl2_bes
    sylvan
)

include_directories(sylvan_include)
