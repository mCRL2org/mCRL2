mcrl2_add_library(mcrl2_bes
  SOURCES
    source/bes.cpp
    source/io.cpp
    source/pgsolver.cpp
  DPARSER_SOURCES
    source/pg_syntax.g
  DEPENDS
    mcrl2_pbes
    dparser
)

add_subdirectory(example)
