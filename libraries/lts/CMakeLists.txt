# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_lts)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

# actual target:
add_library(${PROJECT_NAME}
  source/liblts_scc.cpp
  source/liblts_sim.cpp
  source/liblts_aut.cpp
  source/liblts_det.cpp
  source/liblts_dotlexer.cpp
  source/liblts_svc.cpp
  source/liblts_fsmparser.h
  source/liblts_dotparser.h
  source/liblts_fsmparser.cpp
  source/liblts_dotlexer.ll
  source/liblts_bisim.cpp
  source/sim_hashtable.cpp
  source/liblts_dotparser.cpp
  source/liblts_eq.cpp
  source/liblts_fsmparser.yy
  source/liblts_dot.cpp
  source/liblts.cpp
  source/liblts_bcg.cpp
  source/liblts_dotparser.yy
  source/liblts_tau_star.cpp
  source/tree_set.cpp
  source/liblts_fsm.cpp
  source/liblts_fsmlexer.cpp
  source/liblts_fsmlexer.h
  source/liblts_dotlexer.h
  source/liblts_fsmlexer.ll
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

target_link_libraries(${PROJECT_NAME}
  mcrl2_data
  svc
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

