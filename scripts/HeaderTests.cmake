# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script tries to compile all header files "*.h" 
# in the libraries directories. The script is only 
# executed if the MCRL2_TRYCOMPILE_HEADER_TESTS variable
# is set. The script is executed at configure time.
# Executing the script prolongs the configure time
# extensively. If the script is executed, the verbose
# output is written to the file that corresponds to the
# value of variable MRCL2_HEADERTESTS_LOG.

if(MCRL2_TRYCOMPILE_HEADER_TESTS)

if( NOT MCRL2_HEADER_TESTS_OUTPUT STREQUAL "" )
  SET(MRCL2_HEADERTESTS_LOG ${MCRL2_HEADER_TESTS_OUTPUT} )
  file(REMOVE ${MRCL2_HEADERTESTS_LOG} )
endif( NOT MCRL2_HEADER_TESTS_OUTPUT STREQUAL "" )

SET(MCRL2_HEADERTEST_DIR "${CMAKE_BINARY_DIR}/Testing" )
file(GLOB_RECURSE TERM_OBJS "libraries/*.h")

set(MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/
  ${CMAKE_SOURCE_DIR}/libraries/lps/include
  ${CMAKE_SOURCE_DIR}/libraries/pbes/include
  ${CMAKE_SOURCE_DIR}/libraries/data/include
  ${CMAKE_SOURCE_DIR}/libraries/core/include/ 
  ${CMAKE_SOURCE_DIR}/libraries/utilities/include 
  ${CMAKE_SOURCE_DIR}/libraries/process/include/
  ${CMAKE_SOURCE_DIR}/libraries/atermpp/include
  ${CMAKE_SOURCE_DIR}/build/workarounds/
  ${CMAKE_SOURCE_DIR}/build/precompile
  ${CMAKE_SOURCE_DIR}/libraries/lts/include
  ${CMAKE_SOURCE_DIR}/libraries/fdr/include
  ${CMAKE_SOURCE_DIR}/libraries/trace/include
  ${CMAKE_SOURCE_DIR}/libraries/utilities/include
  ${CMAKE_SOURCE_DIR}/3rd-party/gl2ps
  ${CMAKE_SOURCE_DIR}/3rd-party/svc
  ${CMAKE_SOURCE_DIR}/3rd-party/ticpp
  ${CMAKE_SOURCE_DIR}/3rd-party/tr
)

if(USE_BOOST_INTERNAL)
  set(MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES
      ${MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES}
      ${CMAKE_SOURCE_DIR}/3rd-party/boost
      )
else(USE_BOOST_INTERNAL)
  set(MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES
      ${MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES}
      ${Boost_INCLUDE_DIRS}
      )
endif(USE_BOOST_INTERNAL)

foreach( OBJ ${TERM_OBJS} )
  try_compile(RESULT_VAR ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/scripts/test.cpp
	      CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES}"
              COMPILE_DEFINITIONS "-DBOOST_UNITS_HEADER_NAME=${OBJ}"
	      OUTPUT_VARIABLE var
              )
  MESSAGE( STATUS "TryCompile: ${OBJ} - ${RESULT_VAR} ")

  if( NOT RESULT_VAR )
    if( NOT MCRL2_HEADER_TESTS_OUTPUT STREQUAL "" )
      file(APPEND ${MRCL2_HEADERTESTS_LOG} "=============================================\n")
      file(APPEND ${MRCL2_HEADERTESTS_LOG}  "TryCompile: ${OBJ} - ${RESULT_VAR} \n")
      file(APPEND ${MRCL2_HEADERTESTS_LOG} "---------------------------------------------\n")
      file(APPEND ${MRCL2_HEADERTESTS_LOG} "${OBJ}\n${var}\n" )
      file(APPEND ${MRCL2_HEADERTESTS_LOG} "=============================================\n\n")
    else( NOT MCRL2_HEADER_TESTS_OUTPUT STREQUAL "" )
      message(${MRCL2_HEADERTESTS_LOG} "=============================================")
      message(${MRCL2_HEADERTESTS_LOG}  "TryCompile: ${OBJ} - ${RESULT_VAR} ")
      message(${MRCL2_HEADERTESTS_LOG} "---------------------------------------------")
      message(${MRCL2_HEADERTESTS_LOG} "${OBJ}\n${var}\n" )
      message(${MRCL2_HEADERTESTS_LOG} "=============================================\n")
    endif( NOT MCRL2_HEADER_TESTS_OUTPUT STREQUAL "" )
  endif( NOT RESULT_VAR )

endforeach( OBJ )    

endif(MCRL2_TRYCOMPILE_HEADER_TESTS)
