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

#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/parse.h"
#include <algorithm>
#include <boost/test/minimal.hpp>
#include <iterator>
#include <set>
#include <vector>

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
data::variable nat(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

inline
data::variable pos(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

inline
data::variable bool_(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

void test_find()
{
  process_expression x = parse_process_expression("a(m).P(0)", DATA_DECL, PROC_DECL);

  //--- find_all_variables ---//
  data::variable m = nat("m");
  std::set<data::variable> v = process::find_all_variables(x);
  BOOST_CHECK(v.find(m) != v.end());

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = process::find_sort_expressions(x);
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_nat::nat()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_pos::pos()) == e.end());

}

void test_free_variables()
{
  data::variable b = bool_("b");
  data::data_expression_list d = { b };
  process_identifier P(core::identifier_string("P"), { b });
  process_instance pi(P, d);

  std::set<data::variable> free_variables = process::find_free_variables(pi);
  std::cout << "free variables: " << core::detail::print_set(free_variables) << std::endl;
  BOOST_CHECK(free_variables.find(b) != free_variables.end());
}

int test_main(int argc, char* argv[])
{
  test_find();
  test_free_variables();

  return EXIT_SUCCESS;
}
