mcrl2_add_library(mcrl2_lps
  SOURCES
    source/lps.cpp
    source/lps_io.cpp
    source/linearise.cpp
    source/lpsparunfoldlib.cpp
    source/symbolic_lts_io.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_process
    mcrl2_smt
    mcrl2_symbolic
    Boost::boost
)

add_subdirectory(example)
