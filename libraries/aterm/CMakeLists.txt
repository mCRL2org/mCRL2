# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(mcrl2_aterm)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB HEADERS "include/mcrl2/aterm/*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS}
)

# main target
add_library(${PROJECT_NAME}
  source/idmappings.cpp
  source/bafio.cpp
  source/memmgnt.cpp
  source/memory.cpp
  source/aterm.cpp
  source/hash.cpp
  source/list.cpp
  source/afun.cpp
  source/safio.cpp
  source/byteencoding.cpp
  source/byteio.cpp
  source/gc.cpp
  source/aterm_ext.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

target_link_libraries(mcrl2_aterm
  mcrl2_utilities
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
