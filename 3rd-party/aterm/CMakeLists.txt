# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(aterm)
cmake_minimum_required(VERSION 2.6)

# main target
add_library(${PROJECT_NAME}
  source/version.cpp
  source/idmappings.cpp
#  source/make.c
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
#  source/md5c.c
#  source/tafio.c
)

include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
