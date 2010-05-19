# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(pbes2bes)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

# ########## mcrl2_tools_pbes2bes executable ##########

add_executable(${PROJECT_NAME}
  pbes2bes.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

if( MCRL2_MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MCRL2_MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  mcrl2_data
  mcrl2_utilities_command_line
)

# ######### required for squadt connectivity ##########
if(MCRL2_SQUADT_CONNECTIVITY)
  add_definitions("-DENABLE_SQUADT_CONNECTIVITY")
  include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
  target_link_libraries(${PROJECT_NAME}
    mcrl2_utilities_squadt
  )
endif(MCRL2_SQUADT_CONNECTIVITY)
# #####################################################
endif( MCRL2_MONO_LIB )

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

