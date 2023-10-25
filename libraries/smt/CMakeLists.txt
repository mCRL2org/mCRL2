mcrl2_add_library(mcrl2_smt
  SOURCES
    source/child_process.cpp
    source/solver.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_utilities
)

add_subdirectory(example)
