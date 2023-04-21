add_mcrl2_library(pres
  INSTALL_HEADERS TRUE
  SOURCES
    pres.cpp
    tools.cpp
    tools2.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
    mcrl2_lts
    mcrl2_symbolic
    mcrl2_pbes
  INCLUDE
)

add_subdirectory(example)
