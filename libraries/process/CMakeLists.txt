add_mcrl2_library(process
  INSTALL_HEADERS TRUE
  SOURCES
    process.cpp
    typecheck.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
  INCLUDE
    # TODO: get rid of the LPS header dependency
    ${CMAKE_SOURCE_DIR}/libraries/lps/include
)
