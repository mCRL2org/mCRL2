add_mcrl2_library(smt
  INSTALL_HEADERS TRUE
  SOURCES
    solver.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_utilities
)

add_subdirectory(example)
