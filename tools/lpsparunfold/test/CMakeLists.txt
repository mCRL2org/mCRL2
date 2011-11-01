# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# #########V# Project setup ##########
project(lpsparunfold_test_container)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "lpsparunfold")

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/tools/lpsparunfold/test)

file(GLOB TERM_OBJS "*.cpp")

foreach( OBJ ${TERM_OBJS} )
  get_filename_component(result "${OBJ}" NAME_WE)
  project( "${PREFIX}_${result}" )
  add_executable("${PREFIX}_${result}" "${OBJ}" "${CMAKE_SOURCE_DIR}/tools/lpsparunfold/lpsparunfoldlib.cpp")

  target_link_libraries("${PREFIX}_${result}"
    dparser
    mcrl2_syntax
    mcrl2_lps
    mcrl2_process
    mcrl2_utilities
  )

  add_test("${PREFIX}_${result}" "${lpsparunfold_test_container_BINARY_DIR}/${PREFIX}_${result}" )
  set_tests_properties("${PREFIX}_${result}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")

endforeach( OBJ )
