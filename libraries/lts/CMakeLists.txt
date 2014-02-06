if(BCG_FOUND)
  set(EXTERNAL_LIBS ${BCG_LIBRARIES})
  set(EXTERNAL_INCL ${BCG_INCLUDE_DIRS})
endif()

add_mcrl2_library(lts
  INSTALL_HEADERS TRUE
  SOURCES
    liblts_fsmlexer.cpp
    liblts_fsmparser.cpp
    liblts_fsm.cpp
    liblts_aut.cpp
    liblts_lts.cpp
    liblts_dot.cpp
    liblts_bcg.cpp
    liblts.cpp
    tree_set.cpp
    sim_hashtable.cpp
    exploration.cpp
    fsm_syntax.g
  DEPENDS
    mcrl2_atermpp    HEADER_ONLY
    mcrl2_core       HEADER_ONLY
    mcrl2_data
    mcrl2_lps
    mcrl2_svc
    mcrl2_trace      HEADER_ONLY
    mcrl2_utilities  HEADER_ONLY
    ${EXTERNAL_LIBS}
  INCLUDE
    ${EXTERNAL_INCL}
)
