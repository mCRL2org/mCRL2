add_mcrl2_library(bes
  INSTALL_HEADERS TRUE
  SOURCES
    bes.cpp
    pg_syntax.g
  DEPENDS
    mcrl2_atermpp    HEADER_ONLY
    mcrl2_core 
    mcrl2_data       HEADER_ONLY
    mcrl2_utilities  HEADER_ONLY
  INCLUDE
    # TODO: get rid of the dependency on PBES library
    ${CMAKE_SOURCE_DIR}/libraries/pbes/include
)
