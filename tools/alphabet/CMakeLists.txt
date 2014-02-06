add_mcrl2_tool(alphabet
  SOURCES
    alphabet.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_lps       HEADER_ONLY
    mcrl2_process
    mcrl2_utilities HEADER_ONLY
)
