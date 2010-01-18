# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(pbes_tests)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "pbes")

include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
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
  project( "${PREFIX}_${result}" )

  if( NOT ENABLE_EXPERIMENTAL AND ${result} STREQUAL  "pbes_solve_test")
    message(STATUS "skipped pbes_solve_test")
  else(NOT ENABLE_EXPERIMENTAL AND ${result} STREQUAL  "pbes_solve_test" )

    add_executable("${PREFIX}_${result}" "${OBJ}"  )
    if( MONO_LIB )
      target_link_libraries(${PREFIX}_${result}
        mcrl2_monolib
      )
    else( MONO_LIB )  
      target_link_libraries("${PREFIX}_${result}"
       mcrl2_lps
      )
      if(USE_BOOST_INTERNAL)
        target_link_libraries(${PROJECT_NAME}
          boost
        ) 
      else(USE_BOOST_INTERNAL)
        target_link_libraries(${PROJECT_NAME}
          ${Boost_LIBRARIES}     
        ) 
      endif(USE_BOOST_INTERNAL)
    endif( MONO_LIB )
  
    if(ENABLE_EXPERIMENTAL )
      target_link_libraries(${PROJECT_NAME}
        mcrl2_pbespgsolve
      )
    endif(ENABLE_EXPERIMENTAL)
    add_test("${PREFIX}_${result}" "${pbes_tests_BINARY_DIR}/${PREFIX}_${result}" )
    set_tests_properties("${PREFIX}_${result}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")

  endif( NOT ENABLE_EXPERIMENTAL AND ${result} STREQUAL  "pbes_solve_test")

endforeach( OBJ )  
