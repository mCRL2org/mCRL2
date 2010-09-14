# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(lps_tests)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "lps")

include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)

file(GLOB TERM_OBJS "*.cpp")

foreach( OBJ ${TERM_OBJS} )
  get_filename_component(result "${OBJ}" NAME_WE)
  add_executable("${PREFIX}_${result}" EXCLUDE_FROM_ALL "${OBJ}"  )
  if( MCRL2_MONO_LIB )
    target_link_libraries("${PREFIX}_${result}"
      mcrl2_monolib
    )
  else( MCRL2_MONO_LIB )  
  target_link_libraries("${PREFIX}_${result}"
     mcrl2_lps
  )
  endif( MCRL2_MONO_LIB )

  # Add tests 
  # This method compiles tests when invoked
  # using the current configuration 
  ADD_TEST("${PREFIX}_${result}" ${CMAKE_CTEST_COMMAND}
   --build-and-test
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${CMAKE_CURRENT_BINARY_DIR}"
   --build-noclean
   --build-nocmake
   --build-generator "${CMAKE_GENERATOR}"
   --build-target "${PREFIX}_${result}"
   --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
   --test-command "${CMAKE_CURRENT_BINARY_DIR}/${PREFIX}_${result}"
  )
  set_tests_properties("${PREFIX}_${result}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
endforeach( OBJ ) 
