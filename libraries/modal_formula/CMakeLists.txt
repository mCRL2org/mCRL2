mcrl2_add_library(mcrl2_modal_formula
  SOURCES
    source/modal_formula.cpp
    source/regfrmtrans.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_process
)
