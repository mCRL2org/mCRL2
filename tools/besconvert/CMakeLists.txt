add_mcrl2_tool(besconvert
  SOURCES
    besconvert.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_bes       HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_lps       HEADER_ONLY
    mcrl2_lts
    mcrl2_pbes
    mcrl2_process   HEADER_ONLY
    mcrl2_utilities HEADER_ONLY
    mcrl2_trace     HEADER_ONLY
)
