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

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "include" "*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS})

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
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsmparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dotlexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_aut.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_lts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/sim_hashtable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_dot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_bcg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/tree_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/liblts_fsm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/exploration.cpp
)
set(mcrl2_lts_src ${mcrl2_lts_src} PARENT_SCOPE)

# actual target:
add_library(${PROJECT_NAME}
  ${mcrl2_lts_src}
)

# ##### Start generate parser tables ######

# Silence warnings while compiling generated C code (fsm_syntax.c is the only
# C code in this library)
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w") # Disable all warnings
else(MSVC)
  if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w") # Disable all warnings
  else (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Assuming GCC: disable only the missing-field-initializer warning.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers")
  endif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
endif(MSVC)

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/libraries/lts/source/fsm_syntax.c
  COMMAND make_dparser -A -H1 -ifsm -o${CMAKE_CURRENT_SOURCE_DIR}/source/fsm_syntax.c ${CMAKE_SOURCE_DIR}/doc/specs/fsm-syntax.g
  DEPENDS make_dparser ${CMAKE_SOURCE_DIR}/doc/specs/fsm-syntax.g
)

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/libraries/lts/source/dot_syntax.c
  COMMAND make_dparser -A -H1 -idot -o${CMAKE_CURRENT_SOURCE_DIR}/source/dot_syntax.c ${CMAKE_SOURCE_DIR}/doc/specs/dot-syntax.g
  DEPENDS make_dparser ${CMAKE_SOURCE_DIR}/doc/specs/dot-syntax.g
)

include_directories(
  ${CMAKE_SOURCE_DIR}/3rd-party/dparser
)

add_library(dot_syntax
  source/dot_syntax.c
)

add_library(fsm_syntax
  source/fsm_syntax.c
)

target_link_libraries(dot_syntax
  fsm_syntax
  dparser
)

# add install target:
install(TARGETS dot_syntax fsm_syntax DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)

# ##### End generate parser tables ######

target_link_libraries(${PROJECT_NAME}
  mcrl2_aterm
  svc
  mcrl2_data
  mcrl2_lps
  mcrl2_syntax
  dot_syntax
  fsm_syntax
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
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
