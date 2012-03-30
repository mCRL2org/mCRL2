## Authors: Frank Stappers and Aad Mathijssen
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

include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)

set(mcrl2_lts_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmlexer.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmlexer.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmlexer.ll
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotlexer.ll
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.yy
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.yy
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotlexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_aut.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_lts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_svc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/sim_hashtable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_bcg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/tree_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/exploration_old.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/exploration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/lps2lts_lts.cpp
)
set(mcrl2_lts_src ${mcrl2_lts_src} PARENT_SCOPE)

# actual target:
add_library(${PROJECT_NAME}
  ${mcrl2_lts_src}
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_aterm
  svc
  mcrl2_data
  mcrl2_lps
  mcrl2_syntax
  dparser
)

if( USE_BCG STREQUAL "" )
  include_directories( ${MCRL2_CADP_INCLUDE_DIR} )
  target_link_libraries(${PROJECT_NAME}
		imp_bcg_io
    imp_bcg
  )
endif( USE_BCG STREQUAL "" )

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
