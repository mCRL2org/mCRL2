# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(pbespgsolve)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

add_definitions(-DWITH_MCRL2)

add_library(mcrl2_${PROJECT_NAME}
  compatibility.cpp
  ComponentSolver.cpp
  DecycleSolver.cpp
  DeloopSolver.cpp
  FocusListLiftingStrategy.cpp
  Graph.cpp
  LiftingStrategy.cpp
  LinearLiftingStrategy.cpp
  Logger.cpp
  #main.cpp
  MaxMeasureLiftingStrategy.cpp
  #mcrl2-example.cpp
  OldMaxMeasureLiftingStrategy.cpp
  ParityGame.cpp
  ParityGame_IO.cpp
  ParityGame_verify.cpp
  ParityGameSolver.cpp
  PredecessorLiftingStrategy.cpp
  RecursiveSolver.cpp
  SmallProgressMeasures.cpp

  Abortable.cpp
)

# Sources:
add_executable(${PROJECT_NAME}
  pbespgsolve.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/bes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include/mcrl2/atermpp/)
#include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/tools/pbespgsolve)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)

target_link_libraries(mcrl2_${PROJECT_NAME}
  mcrl2_data
  mcrl2_pbes
  mcrl2_utilities
  mcrl2_syntax
  dparser
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_${PROJECT_NAME}
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})
install(TARGETS mcrl2_${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

