mcrl2_add_tool(lps2lts
  SOURCES
    lps2lts.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
    mcrl2_pbes
)

target_compile_definitions(lps2lts PRIVATE MCRL2_USE_CONTROL_FLOW)