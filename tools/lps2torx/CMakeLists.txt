add_mcrl2_tool(lps2torx
  SOURCES
    lps2torx.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_lps
    mcrl2_utilities HEADER_ONLY
)
