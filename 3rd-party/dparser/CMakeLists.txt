# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(dparser)
cmake_minimum_required(VERSION 2.6)

add_definitions(
  -DD_MAJOR_VERSION=1
  -DD_MINOR_VERSION=26
  -DD_BUILD_VERSION="mCRL2"
)

# ######### General setup ##########
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)

# actual target:
add_library(dparser
  arg.c 
  parse.c 
  scan.c 
  symtab.c 
  util.c 
  read_binary.c 
  dparse_tree.c
)

add_library(mkdparser
  mkdparse.c 
  write_tables.c 
  grammar.g.c 
  gram.c 
  lex.c 
  lr.c
)

add_executable(make_dparser
  make_dparser.c
  version.c
)

target_link_libraries(make_dparser
  dparser
  mkdparser
)

install(TARGETS dparser DESTINATION ${MCRL2_LIB_DIR})
