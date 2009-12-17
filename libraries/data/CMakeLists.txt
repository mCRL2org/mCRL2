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

# ######### General setup ##########

#jitty.c requires DL_LIBS
if(UNIX)
  LINK_LIBRARIES(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
  set(COMP_REWRITERS 
   source/detail/rewrite/jittyc.cpp
   source/detail/rewrite/innerc.cpp
  )
endif(UNIX)


include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
#The following include directory is required by out-of-source builds for finding setup.h
include_directories(${CMAKE_BINARY_DIR}/libraries/utilities/include)

add_library(${PROJECT_NAME}
# source/data_implementation_concrete.cpp (unused)
# source/data_reconstruct.cpp (unused)
# The following files are included in source/implementations.cpp:
# source/data_specification.cpp
# source/standard.cpp
# source/detail/enumerate/enum_standard.cpp
# source/detail/prover/bdd2dot.cpp
# source/detail/prover/bdd_path_eliminator.cpp
# source/detail/prover/bdd_prover.cpp
# source/detail/prover/bdd_simplifier.cpp
# source/detail/prover/formula_checker.cpp
# source/detail/prover/induction.cpp
# source/detail/prover/info.cpp
# source/detail/prover/manipulator.cpp
# source/detail/prover/prover.cpp
# source/detail/prover/smt_lib_solver.cpp
 source/implementations.cpp
 source/detail/rewrite/with_prover.cpp
 source/detail/rewrite/inner.cpp
 source/detail/rewrite/jitty.cpp
 source/detail/rewrite/rewrite.cpp
 ${COMP_REWRITERS}
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_core
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

