# Author: Sjoerd Cranen
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(dparser)
cmake_minimum_required(VERSION 3.8)

add_definitions(
  -DD_MAJOR_VERSION=1
  -DD_MINOR_VERSION=26
  -DD_BUILD_VERSION="mCRL2"
  -DD_USE_FREELISTS=1
)

if(MSVC)
add_definitions(
  -Dstrtoll=_strtoi64
  -Dstrncasecmp=_strnicmp
)
endif(MSVC)

if(MSVC AND MSVC_VERSION LESS 1900)
add_definitions(
  -Dsnprintf=_snprintf
)
endif(MSVC AND MSVC_VERSION LESS 1900)

add_executable(make_dparser
  make_dparser.c
  version.c
)

add_library(mkdparse STATIC
  mkdparse.c
  write_tables.c
  grammar.g.c
  gram.c
  lex.c
  lr.c
  version.c
)

add_library(dparser STATIC
  arg.c
  parse.c
  scan.c
  dsymtab.c
  util.c
  read_binary.c
  dparse_tree.c
)

target_link_libraries(make_dparser PRIVATE mkdparse dparser)

set(dparser_include ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
