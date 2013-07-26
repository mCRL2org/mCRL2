# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script tries to compile all header files "*.h" 
# in the libraries directories. The script is only 

# ########## Project setup ##########
project(mcrl2_headertest_targets)
cmake_minimum_required(VERSION 2.6)

set(MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/libraries/lps/include
  ${CMAKE_SOURCE_DIR}/libraries/bes/include
  ${CMAKE_SOURCE_DIR}/libraries/pbes/include
  ${CMAKE_SOURCE_DIR}/libraries/data/include
  ${CMAKE_SOURCE_DIR}/libraries/core/include/ 
  ${CMAKE_SOURCE_DIR}/libraries/utilities/include 
  ${CMAKE_SOURCE_DIR}/libraries/process/include/
  ${CMAKE_SOURCE_DIR}/libraries/atermpp/include
  ${CMAKE_SOURCE_DIR}/build/workarounds/
  ${CMAKE_SOURCE_DIR}/libraries/lts/include
  ${CMAKE_SOURCE_DIR}/libraries/fdr/include
  ${CMAKE_SOURCE_DIR}/libraries/trace/include
  ${CMAKE_SOURCE_DIR}/libraries/utilities/include
  ${CMAKE_SOURCE_DIR}/3rd-party/gl2ps
  ${CMAKE_SOURCE_DIR}/3rd-party/svc
  ${CMAKE_SOURCE_DIR}/3rd-party/tr
)

set(MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES
  ${MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES}
  ${Boost_INCLUDE_DIRS}
)

include_directories( ${MRCL2_HEADERTESTS_INCLUDE_DIRECTORIES} )

file(GLOB_RECURSE TERM_OBJS "${CMAKE_SOURCE_DIR}/libraries/*.h")
foreach( OBJ ${TERM_OBJS} )

  file(RELATIVE_PATH X "${CMAKE_SOURCE_DIR}/libraries/" "${OBJ}")
  file(TO_CMAKE_PATH "${X}" Y )
  string(REPLACE "/" "_" Z "${Y}")
  string(REPLACE "." "_" result "${Z}")

  project( "header_${result}" )
  add_executable("header_${result}" EXCLUDE_FROM_ALL "header_test.cpp" )

  set_target_properties( "header_${result}"
  PROPERTIES COMPILE_DEFINITIONS "BOOST_UNITS_HEADER_NAME=${OBJ}" )

  add_test(NAME "header_${result}" 
    COMMAND ${CMAKE_MAKE_PROGRAM} "header_${result}" 
  )


endforeach( OBJ )    
