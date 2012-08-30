# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_data)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB HEADERS "include/mcrl2/data/*.h")
FILE(GLOB DETAIL_HEADERS "include/mcrl2/data/detail/*.h")
FILE(GLOB DETAIL_SUB_HEADERS "include/mcrl2/data/detail/*/*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS} ${DETAIL_HEADERS} ${DETAIL_SUB_HEADERS}
)

# ######### General setup ##########

#jitty.c requires DL_LIBS
if(UNIX)
  LINK_LIBRARIES(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
  set(COMP_REWRITERS
   ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/rewrite/jittyc.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/rewrite/innerc.cpp
  )
endif(UNIX)

set(mcrl2_data_src
 ${CMAKE_CURRENT_SOURCE_DIR}/source/data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/source/data_specification.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/source/typecheck.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/enumerate/enum_standard.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/prover/smt_lib_solver.cpp

 ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/rewrite/with_prover.cpp
 # ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/rewrite/inner.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/rewrite/jitty.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/source/detail/rewrite/rewrite.cpp
 ${COMP_REWRITERS}
)

set(mcrl2_data_src ${mcrl2_data_src} PARENT_SCOPE)

include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

add_library(${PROJECT_NAME}
  ${mcrl2_data_src}
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_core
  mcrl2_utilities
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
