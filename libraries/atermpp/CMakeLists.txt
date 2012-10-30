# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(mcrl2_atermpp)
cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB HEADERS "include/mcrl2/atermpp/*.h")
FILE(GLOB DETAIL_HEADERS "include/mcrl2/atermpp/detail/*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS} ${DETAIL_HEADERS}
)

# main target
add_library(${PROJECT_NAME}
  source/aterm_io_binary.cpp
  source/memory.cpp
  source/aterm_io_text.cpp
  source/table.cpp
  source/indexed_set.cpp
#  source/byteio.cpp
#  source/byteencoding.cpp
#  source/safio.cpp
#  source/idmappings.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

target_link_libraries( mcrl2_atermpp
  mcrl2_utilities
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

