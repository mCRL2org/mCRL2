add_mcrl2_library(lts
  INSTALL_HEADERS TRUE
  SOURCES
    check_complexity.cpp
    liblts_bisim_gjkw.cpp
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
