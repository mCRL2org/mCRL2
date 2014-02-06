add_mcrl2_library(process
  INSTALL_HEADERS TRUE
  SOURCES
    process.cpp
    typecheck.cpp
  DEPENDS
    mcrl2_atermpp    HEADER_ONLY
    mcrl2_core
    mcrl2_data
    mcrl2_utilities  HEADER_ONLY
  INCLUDE
    # TODO: get rid of the LPS header dependency
    ${CMAKE_SOURCE_DIR}/libraries/lps/include
)
