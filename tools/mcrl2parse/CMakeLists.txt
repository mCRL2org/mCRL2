# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2parse)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

# ########## mcrl2_tools_mcrl2parse executable ##########

# actual target:
add_executable(${PROJECT_NAME}
  mcrl2parse.cpp
)

include_directories(
  ${CMAKE_SOURCE_DIR}/libraries/utilities/include
  ${CMAKE_SOURCE_DIR}/libraries/core/include
  ${CMAKE_SOURCE_DIR}/libraries/data/include
  ${CMAKE_SOURCE_DIR}/libraries/process/include
  ${CMAKE_SOURCE_DIR}/libraries/lps/include
  ${CMAKE_SOURCE_DIR}/libraries/bes/include
  ${CMAKE_SOURCE_DIR}/libraries/pbes/include
  ${CMAKE_SOURCE_DIR}/libraries/atermpp/include
  ${CMAKE_SOURCE_DIR}/libraries/aterm/include
  ${CMAKE_SOURCE_DIR}/3rd-party/dparser
  ${CMAKE_SOURCE_DIR}/build/precompile
  ${CMAKE_SOURCE_DIR}/build/workarounds
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_lps
  mcrl2_pbes
  mcrl2_process
  mcrl2_utilities
  mcrl2_syntax
  dparser
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Runtime)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

