add_mcrl2_tool(txt2lps
  SOURCES
    txt2lps.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_lps
    mcrl2_utilities HEADER_ONLY
)
