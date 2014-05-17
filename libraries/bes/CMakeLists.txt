# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(mcrl2_bes)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "include" "*.h")
add_custom_target(${PROJECT_NAME}_headers SOURCES ${HEADERS}
)

# ##### Start generate parser tables ######

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libraries/bes/source)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/libraries/bes/source/pg_syntax.c
  COMMAND make_dparser -A -H0 -ipg -o${CMAKE_BINARY_DIR}/libraries/bes/source/pg_syntax.c ${CMAKE_SOURCE_DIR}/doc/specs/pg-syntax.g
  DEPENDS make_dparser ${CMAKE_SOURCE_DIR}/doc/specs/pg-syntax.g
)

add_library(${PROJECT_NAME}
  ${CMAKE_BINARY_DIR}/libraries/bes/source/pg_syntax.c
  source/bes.cpp
  source/pgsolver.cpp
  source/cwi.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
  mcrl2_pbes
  dparser
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)

# ##### End generate parser tables ######

#target_link_libraries(${PROJECT_NAME}
#  pg_syntax
#  dparser
#)
