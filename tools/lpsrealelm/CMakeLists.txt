# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(lpsrealelm)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "." "*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS})

add_executable(${PROJECT_NAME}
  lpsrealelm.cpp
  realelm.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/bes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)


target_link_libraries(${PROJECT_NAME}
  mcrl2_lps
  mcrl2_utilities
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Applications)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

