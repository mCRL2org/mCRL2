add_mcrl2_tool(pbes2bool
  SOURCES
    pbes2bool.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_bes       HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_pbes
    mcrl2_utilities HEADER_ONLY
)
