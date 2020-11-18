add_mcrl2_tool(pbesbddsolve1
  SOURCES
    pbesbddsolve1.cpp
  DEPENDS
    mcrl2_pbes
    mcrl2_bes
    sylvan
)

include_directories(sylvan_include)
