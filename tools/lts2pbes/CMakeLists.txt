add_mcrl2_tool(lts2pbes
  SOURCES
    lts2pbes.cpp
  DEPENDS
    mcrl2_atermpp       HEADER_ONLY
    mcrl2_bes           HEADER_ONLY
    mcrl2_core          HEADER_ONLY
    mcrl2_data          HEADER_ONLY
    mcrl2_lps           HEADER_ONLY
    mcrl2_lts
    mcrl2_modal_formula HEADER_ONLY
    mcrl2_pbes
    mcrl2_process       HEADER_ONLY
    mcrl2_utilities     HEADER_ONLY
)
