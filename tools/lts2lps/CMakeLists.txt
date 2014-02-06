add_mcrl2_tool(lts2lps
  SOURCES
    lts2lps.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_lps
    mcrl2_lts
    mcrl2_process   HEADER_ONLY
    mcrl2_utilities HEADER_ONLY
)
