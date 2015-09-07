add_mcrl2_library(lts
  INSTALL_HEADERS TRUE
  SOURCES
    liblts_fsm.cpp
    liblts_aut.cpp
    liblts_lts.cpp
    liblts_dot.cpp
    liblts.cpp
    tree_set.cpp
    sim_hashtable.cpp
    exploration.cpp
    fsm_syntax.g
  DEPENDS
    mcrl2_data
    mcrl2_lps
)
