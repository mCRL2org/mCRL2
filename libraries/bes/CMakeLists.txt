add_mcrl2_library(bes
  INSTALL_HEADERS TRUE
  SOURCES
    bes.cpp
    io.cpp
    pgsolver.cpp
    pg_syntax.g
  DEPENDS
    mcrl2_pbes
)

add_subdirectory(example)
