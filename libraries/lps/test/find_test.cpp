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

#include "mcrl2/data/consistency.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include <algorithm>
#include <boost/test/minimal.hpp>
#include <iterator>
#include <set>
#include <vector>

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
  return data::variable(core::identifier_string(name), data::bool_());
}

void test_find()
{
  specification spec = parse_linear_process_specification(SPEC);
  std::cout << spec.process().action_summands().size() << std::endl;
  action_summand s = spec.process().action_summands().front();
  process::action a = s.multi_action().actions().front();

  //--- find_all_variables ---//
  data::variable m = nat("m");
  std::set<data::variable> v = lps::find_all_variables(a);
  v = lps::find_all_variables(s);
  BOOST_CHECK(v.find(m) != v.end());

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = lps::find_sort_expressions(a);
  std::cout << "e.size() = " << e.size() << std::endl;
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_nat::nat()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_pos::pos()) == e.end());
}

void test_free_variables()
{
  std::set<data::variable> free_variables;

  lps::specification specification(parse_linear_process_specification(
                                     "act a : Bool;\n"
                                     "proc X = a((forall x : Nat. exists y : Nat. x < y)).X;\n"
                                     "init X;\n"
                                   ));

  free_variables = find_free_variables(specification.process());
  BOOST_CHECK(free_variables.find(data::variable("x", data::sort_nat::nat())) == free_variables.end());
  BOOST_CHECK(free_variables.find(data::variable("y", data::sort_nat::nat())) == free_variables.end());

  specification = parse_linear_process_specification(
                    "act a;\n"
                    "proc X(z : Bool) = (z && (forall x : Nat. exists y : Nat. x < y)) -> a.X(!z);\n"
                    "init X(true);\n"
                  );
  free_variables = find_free_variables(specification);
  BOOST_CHECK(free_variables.empty());

  free_variables = find_free_variables(specification.process());
  BOOST_CHECK(free_variables.empty());

  free_variables = find_free_variables(specification.process().action_summands().front());
  BOOST_CHECK(free_variables.size() == 1);
  BOOST_CHECK(free_variables.find(data::variable("z", data::bool_())) != free_variables.end());

  BOOST_CHECK(check_well_typedness(specification));
}

void test_search()
{
  lps::specification spec(parse_linear_process_specification(
                            "glob dc: Nat;\n"
                            "act a : Bool;\n"
                            "proc X = a((forall x : Nat. exists y : Nat. (x < y) && (y < dc))).X;\n"
                            "init X;\n"
                          ));
  data::variable x("x", data::sort_nat::nat());
  BOOST_CHECK(!lps::search_free_variable(spec.process().action_summands(), x));

  data::variable y("y", data::sort_nat::nat());
  BOOST_CHECK(!lps::search_free_variable(spec.process().action_summands(), y));

  data::variable dc("dc", data::sort_nat::nat());
  BOOST_CHECK(lps::search_free_variable(spec.process().action_summands(), dc));
}

void test_search_sort_expression()
{
  std::string text =
    "act a: List(Bool);                   \n"
    "proc X(x: List(Bool)) = a(x) . X(x); \n"
    "init X([true]);                      \n"
    ;
  lps::specification spec = parse_linear_process_specification(text);
  data::sort_expression s = data::parse_sort_expression("List(Bool)");
  BOOST_CHECK(data::search_sort_expression(spec.data().sorts(), s));
}

int test_main(int argc, char* argv[])
{
  test_find();
  test_free_variables();
  test_search();
  test_search_sort_expression();

  return EXIT_SUCCESS;
}
