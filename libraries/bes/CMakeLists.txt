add_mcrl2_library(bes
  INSTALL_HEADERS TRUE
  SOURCES
    bes.cpp
    pgsolver.cpp
    pg_syntax.g
  DEPENDS
    mcrl2_pbes
)
