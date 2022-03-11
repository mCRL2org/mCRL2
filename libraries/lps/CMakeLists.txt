add_mcrl2_library(lps
  INSTALL_HEADERS TRUE
  SOURCES
    lps.cpp
    lps_io.cpp
    tools.cpp
    linearise.cpp
    lpsparunfoldlib.cpp
    next_state_generator.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_process
    mcrl2_smt
    mcrl2_symbolic
  INCLUDE
    ${Boost_INCLUDE_DIRS}
)

add_subdirectory(example)
