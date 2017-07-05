add_mcrl2_library(smt
  INSTALL_HEADERS TRUE
  SOURCES
    basic_data_specification.cpp
    cvc4.cpp
    recursive_function_definition.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_utilities
)
