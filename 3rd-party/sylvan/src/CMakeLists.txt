# Author: Wieger Wesselink
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_library(sylvan
  STATIC
  lace.c
  sha2.c
  sylvan_bdd.c
  sylvan_cache.c
  sylvan_common.c
  sylvan_hash.c
  sylvan_ldd.c
  sylvan_mt.c
  sylvan_mtbdd.c
  # Sylvan is actually a C library, and this should not compile in C++17.
  #sylvan_obj.cpp
  sylvan_refs.c
  sylvan_sl.c
  sylvan_stats.c
  sylvan_table.c
)

target_include_directories(sylvan PUBLIC .)
target_link_libraries(sylvan pthread)
set_target_properties(sylvan 
  PROPERTIES 
  POSITION_INDEPENDENT_CODE TRUE)

set(sylvan_include ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
