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
  source/huffman.c
  source/lz.c
  source/compress.c
  source/blocklist.c
  source/code.c
  source/ptable.c
  source/svc1.c
  source/svc2.c
  source/hashtable.c
)

include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)

target_link_libraries(${PROJECT_NAME}
  aterm 
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
