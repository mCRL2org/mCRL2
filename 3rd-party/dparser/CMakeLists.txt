# Author: Sjoerd Cranen
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(dparser)

add_definitions(
  -DD_MAJOR_VERSION=1
  -DD_MINOR_VERSION=26
  -DD_BUILD_VERSION="mCRL2"
  -DD_USE_FREELISTS=1
)

if(MSVC)
  # MSVC complains about deprecated POSIX names, so this replaces those by other deprecated functions.
  add_compile_definitions(
    strtoll=_strtoi64
    strnicmp=_strnicmp
    strncasecmp=_strnicmp
  )
endif(MSVC)

if(MSVC AND MSVC_VERSION LESS 1900)
  add_compile_definitions(
    snprintf=_snprintf
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

add_library(dparser
  arg.c
  parse.c
  scan.c
  dsymtab.c
  util.c
  read_binary.c
  dparse_tree.c
)

target_include_directories(dparser PUBLIC ".")
target_link_libraries(make_dparser PRIVATE mkdparse dparser)
