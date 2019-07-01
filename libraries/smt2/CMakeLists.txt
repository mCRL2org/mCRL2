add_mcrl2_library(smt2
  INSTALL_HEADERS TRUE
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_utilities
)

add_subdirectory(example)
