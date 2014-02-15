# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(data_tests)
cmake_minimum_required(VERSION 2.6)
# ######### General setup ##########

set(PREFIX "data")

file(GLOB TESTS "*.cpp")

foreach(test ${TESTS})
  get_filename_component(result "${test}" NAME_WE)
  add_executable("${PREFIX}_${result}" EXCLUDE_FROM_ALL "${test}")
  target_link_libraries("${PREFIX}_${result}"
     mcrl2_data
     mcrl2_utilities
     mcrl2_syntax
  )
  # MACRO is defined in CMakeLists.txt in root of source tree
  build_and_run_test_target( ${PREFIX}_${result} )
  set_tests_properties("${PREFIX}_${result}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}" )
endforeach()
