add_mcrl2_library(lps
  INSTALL_HEADERS TRUE
  SOURCES
    lps.cpp
    tools.cpp
    linearise.cpp
    lpsparunfoldlib.cpp
    next_state_generator.cpp
    disjointness_checker.cpp
    simulation.cpp
    typecheck.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_process
    mcrl2_trace
)
