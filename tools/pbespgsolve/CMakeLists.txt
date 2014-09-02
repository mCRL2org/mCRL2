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

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "." "*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS})

add_library(mcrl2_${PROJECT_NAME}
  ComponentSolver.cpp
  DecycleSolver.cpp
  DeloopSolver.cpp
  FocusListLiftingStrategy.cpp
  Graph.cpp
  LiftingStrategy.cpp
  LinearLiftingStrategy.cpp
  MaxMeasureLiftingStrategy.cpp
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

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mcrl2_${PROJECT_NAME}
  mcrl2_bes
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_${PROJECT_NAME}
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Applications)
install(TARGETS mcrl2_${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

