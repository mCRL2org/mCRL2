add_mcrl2_library(lts
  INSTALL_HEADERS TRUE
  NOHEADERTEST
    # These headers require C++17. Reenable when compiling with C++17.
    mcrl2/lts/state_space_generator-cpp17.h

  SOURCES
    check_complexity.cpp
    liblts_bisim_gjkw.cpp
    liblts_bisim_dnj.cpp
    liblts_fsm.cpp
    liblts_aut.cpp
    liblts_lts.cpp
    liblts_dot.cpp
    liblts.cpp
    tree_set.cpp
    sim_hashtable.cpp
    exploration.cpp
  DEPENDS
    mcrl2_data
    mcrl2_lps
)
