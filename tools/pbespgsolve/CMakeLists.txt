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
  compatibility.cc
  ComponentSolver.cc
  FocusListLiftingStrategy.cc
  Graph.cc
  LiftingStrategy.cc
  LinearLiftingStrategy.cc
  Logger.cc
  #main.cc
  MaxMeasureLiftingStrategy.cc
  #mcrl2-example.cc
  OldMaxMeasureLiftingStrategy.cc
  ParityGame.cc
  ParityGame_IO.cc
  ParityGame_verify.cc
  ParityGameSolver.cc
  PredecessorLiftingStrategy.cc
  RecursiveSolver.cc
  SmallProgressMeasures.cc

  Abortable.cpp
)

# Sources:
add_executable(${PROJECT_NAME}
  pbespgsolve.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include/mcrl2/atermpp/)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/tools/pbespgsolve)

if( MCRL2_MONO_LIB )
  target_link_libraries(mcrl2_${PROJECT_NAME}
    mcrl2_monolib
  )
else( MCRL2_MONO_LIB )
target_link_libraries(mcrl2_${PROJECT_NAME}
  mcrl2_data
  mcrl2_utilities_command_line
)
endif( MCRL2_MONO_LIB )

target_link_libraries(${PROJECT_NAME}
  mcrl2_${PROJECT_NAME}
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

