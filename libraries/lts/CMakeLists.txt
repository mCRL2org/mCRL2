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

set(mcrl2_lts_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_scc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_aut.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_det.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotlexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_svc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotlexer.ll
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_bisim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/sim_hashtable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_eq.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.yy
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_bcg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.yy
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_tau_star.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/tree_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmlexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmlexer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotlexer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmlexer.ll
  PARENT_SCOPE
)

if(NOT MONO_LIB)
  # actual target:
  add_library(${PROJECT_NAME}
    ${mcrl2_lts_src}
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
endif( NOT MONO_LIB )
