// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Test for find functions.

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/find.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::process;

std::string DATA_DECL = 
  "glob                   \n"
  "  m: Nat;              \n"
  "                       \n" 
  "act                    \n"
  "  a: Nat;              \n"
  ;

std::string PROC_DECL = 
  "proc P(n:Nat);         \n"
  ;

inline
data::variable nat(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

inline
data::variable pos(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

inline
data::variable bool_(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

void test_find()
{
  process_expression x = parse_process_expression("a(m).P(0)", DATA_DECL, PROC_DECL);

  //--- find_variables ---//
  data::variable m = nat("m"); 
  std::set<data::variable> v = data::find_variables(x);
  BOOST_CHECK(v.find(m) != v.end());   

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = data::find_sort_expressions(x);
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_nat::nat()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_pos::pos()) == e.end());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_find();
  core::garbage_collect();

  return EXIT_SUCCESS;
}
