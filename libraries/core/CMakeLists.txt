# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_core)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "include" "*.h")
add_custom_target(${PROJECT_NAME}_headers SOURCES ${HEADERS})

# ##### Generate parser tables ######

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libraries/core/source)
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/libraries/core/source/mcrl2_syntax.c
  COMMAND make_dparser -A -H0 -imcrl2 -o${CMAKE_BINARY_DIR}/libraries/core/source/mcrl2_syntax.c ${CMAKE_SOURCE_DIR}/doc/specs/mcrl2-syntax.g
  DEPENDS make_dparser ${CMAKE_SOURCE_DIR}/doc/specs/mcrl2-syntax.g
)

add_library(mcrl2_syntax
  ${CMAKE_BINARY_DIR}/libraries/core/source/mcrl2_syntax.c
)

target_link_libraries(mcrl2_syntax
  dparser
)

# add install target:
install(TARGETS mcrl2_syntax DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)


# ######### General setup ##########

set(mcrl2_core_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/dparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/core.cpp
)

set(mcrl2_core_src ${mcrl2_core_src} PARENT_SCOPE)

# actual target:
add_library(${PROJECT_NAME}
  ${mcrl2_core_src}
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_atermpp
  mcrl2_utilities
  mcrl2_syntax
)


# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
