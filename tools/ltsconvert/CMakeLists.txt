add_mcrl2_tool(ltsconvert
  SOURCES
    ltsconvert.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_lps       HEADER_ONLY
    mcrl2_lts
    mcrl2_utilities HEADER_ONLY
    mcrl2_trace     HEADER_ONLY
)
