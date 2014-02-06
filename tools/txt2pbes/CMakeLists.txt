add_mcrl2_tool(txt2pbes
  SOURCES
    txt2pbes.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_pbes
    mcrl2_utilities HEADER_ONLY
)
