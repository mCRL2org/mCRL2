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
  source/version.c
  source/idmappings.c
  source/make.c
  source/bafio.c
  source/memmgnt.c
  source/memory.c
  source/aterm.c
  source/hash.c
  source/list.c
  source/afun.c
  source/safio.c
  source/byteencoding.c
  source/byteio.c
  source/gc.c
  source/md5c.c
  source/tafio.c
)

include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
