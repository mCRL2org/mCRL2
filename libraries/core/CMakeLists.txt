add_mcrl2_library(core
  INSTALL_HEADERS TRUE
  SOURCES
    dparser.cpp
    core.cpp
    mcrl2_syntax.g
  DEPENDS
    mcrl2_atermpp
    mcrl2_utilities
    dparser
)
