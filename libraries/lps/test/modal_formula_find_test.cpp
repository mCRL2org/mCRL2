// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file modal_formula_find_test.cpp
/// \brief Test for find functions.

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/parse.h"

using namespace mcrl2;
using namespace mcrl2::state_formulas;

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
  lps::specification spec = lps::parse_linear_process_specification(SPEC);
  state_formula f = parse_state_formula("(mu X. X) && (forall b:Bool. true)", spec);

  //--- find_variables ---//
  data::variable b = bool_("b");
  std::set<data::variable> v = state_formulas::find_variables(f);
  BOOST_CHECK(v.find(b) != v.end());

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = state_formulas::find_sort_expressions(f);
  std::cout << "e.size() = " << e.size() << std::endl;
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_bool::bool_()) != e.end());
}

void test_free_variables()
{
  variable X("X", data::data_expression_list());
  data::variable b = bool_("b");
  data::variable c = bool_("c");
  data::data_expression phi = data::equal_to(b, c);
  data::variable_list v = atermpp::make_list(b);
  state_formula f = forall(v, phi);
  std::set<data::variable> free_variables = state_formulas::find_free_variables(f);
  std::cout << "free variables: " << data::detail::print_set(free_variables) << std::endl;
  BOOST_CHECK(free_variables.find(b) == free_variables.end());
  BOOST_CHECK(free_variables.find(c) != free_variables.end());
}

int test_main(int argc, char* argv[])
{
  test_find();
  test_free_variables();

  return EXIT_SUCCESS;
}
