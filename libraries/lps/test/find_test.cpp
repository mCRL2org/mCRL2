// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Test for find functions.

#define BOOST_TEST_MODULE find_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/consistency.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::lps;

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

template <typename T>
std::string print_set(const std::set<T>& v)
{
  std::set<std::string> s;
  for (const T& v_i: v)
  {
    s.insert(data::pp(v_i));
  }
  return core::detail::print_set(s);
}

BOOST_AUTO_TEST_CASE(test_find)
{
  using utilities::detail::contains;

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

  specification lpsspec = parse_linear_process_specification(SPEC);
  action_summand s = lpsspec.process().action_summands().front();
  process::action a = s.multi_action().actions().front();

  BOOST_CHECK_EQUAL(print_set(lps::find_all_variables(s)), "{ m, n }");
  BOOST_CHECK_EQUAL(print_set(lps::find_sort_expressions(a)), "{ Nat }");
}

BOOST_AUTO_TEST_CASE(test_free_variables)
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

// The lineariser determines which global variables occur in the resulting linear process using
// lps::find_free_variables. Global variables that only occur inside a binder (lambda, quantifier, where clause)
// must be found as well.
BOOST_AUTO_TEST_CASE(test_free_variables_in_binders)
{
  const lps::specification specification
    = parse_linear_process_specification("act a: Nat;\n"
                                         "    b;\n"
                                         "glob dc1, dc2, dc3: Nat;\n"
                                         "proc X(n: Nat) = (exists x: Nat. x < dc1) -> b . X(n)\n"
                                         "               + a((lambda y: Nat. y + dc2)(n)) . X(n)\n"
                                         "               + a(y whr y = dc3 end) . X(n);\n"
                                         "init X(0);\n");

  const data::variable dc1("dc1", data::sort_nat::nat());
  const data::variable dc2("dc2", data::sort_nat::nat());
  const data::variable dc3("dc3", data::sort_nat::nat());

  // The bound variables x and y and the process parameter n are not free in the process.
  const std::set<data::variable> free_variables = find_free_variables(specification.process());
  BOOST_CHECK(free_variables == std::set<data::variable>({dc1, dc2, dc3}));

  // Each summand contains one of the global variables, inside a binder.
  const std::vector<data::variable> globals = {dc1, dc2, dc3};
  for (const action_summand& summand: specification.process().action_summands())
  {
    const std::set<data::variable> V = find_free_variables(summand);
    BOOST_CHECK(
      std::count_if(globals.begin(), globals.end(), [&](const data::variable& v) { return V.find(v) != V.end(); })
      == 1);
  }
}

BOOST_AUTO_TEST_CASE(test_search)
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

BOOST_AUTO_TEST_CASE(test_search_sort_expression)
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
