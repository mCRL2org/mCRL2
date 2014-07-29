# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(mcrl2_atermpp)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "include" "*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS} ${DETAIL_HEADERS}
)

# main target
add_library(${PROJECT_NAME}
  source/function_symbol.cpp
  source/aterm_implementation.cpp
  source/aterm_io_text.cpp
  source/aterm_io_binary.cpp
)

target_link_libraries( ${PROJECT_NAME}
  mcrl2_utilities
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
