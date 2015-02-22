add_mcrl2_library(lps
  INSTALL_HEADERS TRUE
  SOURCES
    lps.cpp
    tools.cpp
    linearise.cpp
    lpsparunfoldlib.cpp
    next_state_generator.cpp
    simulation.cpp
    typecheck.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_process
  INCLUDE
    # TODO: get rid of the trace header dependency
    ${CMAKE_SOURCE_DIR}/libraries/trace/include
)
