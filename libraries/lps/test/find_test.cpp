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
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::lps;

std::string SPEC = 
  "glob                      \n"
  "  m: Nat;                 \n"
  "                          \n"
  "act                       \n"
  "  a: Nat;                 \n"
  "                          \n"
  "proc                      \n"
  "  P(n:Nat) = a(m).P(n+1); \n"
  "                          \n"
  "init P(0);                \n"
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
  specification spec = parse_linear_process_specification(SPEC);
  std::cout << spec.process().action_summands().size() << std::endl;
  action_summand s = spec.process().action_summands().front();
  action a = s.multi_action().actions().front();

  //--- find_variables ---//
  data::variable m = nat("m"); 
  std::set<data::variable> v = lps::find_variables(a);
  //v = data::find_variables(s); // TODO: this doesn't compile!
  BOOST_CHECK(v.find(m) != v.end());   

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = lps::find_sort_expressions(a);
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
