# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(svc)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

# actual target:
add_library(${PROJECT_NAME}
  source/huffman.cpp
  source/lz.cpp
  source/compress.cpp
  source/blocklist.cpp
  source/code.cpp
  source/ptable.cpp
  source/svc1.cpp
  source/svc2.cpp
  source/hashtable.cpp
)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/3rd-party/svc/include/svc)

target_link_libraries(${PROJECT_NAME}
  mcrl2_atermpp
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
