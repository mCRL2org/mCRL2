add_mcrl2_library(modal_formula
  INSTALL_HEADERS TRUE
  SOURCES
    modal_formula.cpp
    regfrmtrans.cpp
    typecheck.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_process
)
