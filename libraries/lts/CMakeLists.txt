mcrl2_add_library(mcrl2_lts
  SOURCES
    source/check_complexity.cpp
    source/liblts_bisim_gjkw.cpp
    source/liblts_fsm.cpp
    source/liblts_aut.cpp
    source/liblts_lts.cpp
    source/liblts_dot.cpp
    source/liblts.cpp
    source/tree_set.cpp
    source/sim_hashtable.cpp
    source/simulation.cpp
  DEPENDS
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
)
