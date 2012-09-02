# Authors: Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_process)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "include" "*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS})

# ######### General setup ##########

include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)

set(mcrl2_process_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/alphabet_reduction.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/process.cpp
)

set(mcrl2_process_src ${mcrl2_process_src} PARENT_SCOPE)

# actual target:
add_library(${PROJECT_NAME}
  ${mcrl2_process_src}
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_core
  mcrl2_data
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
