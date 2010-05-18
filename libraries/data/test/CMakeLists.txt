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

include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)

file(GLOB TERM_OBJS "*.cpp")

set(FAILING_TESTS
 data_typecheck_test      
)

foreach( OBJ ${TERM_OBJS} )
  get_filename_component(result "${OBJ}" NAME_WE)
  project( "${PREFIX}_${result}" )
  add_executable("${PREFIX}_${result}" "${OBJ}" )
  if( MCRL2_MONO_LIB )
    target_link_libraries(${PREFIX}_${result}
      mcrl2_monolib
    )
  else( MCRL2_MONO_LIB )  
  target_link_libraries("${PREFIX}_${result}"
     mcrl2_data
  )
  endif( MCRL2_MONO_LIB )
  add_test("${PREFIX}_${result}" "${data_tests_BINARY_DIR}/${PREFIX}_${result}" )

  list(FIND FAILING_TESTS "${PREFIX}_${result}" FIND_ELM ) 
  if( -1 EQUAL FIND_ELM )
    set_tests_properties("${PREFIX}_${result}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}" )
  else( -1 EQUAL FIND_ELM )
    set_tests_properties("${PREFIX}_${result}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}" WILL_FAIL true )
  endif( -1 EQUAL FIND_ELM )

endforeach( OBJ )
