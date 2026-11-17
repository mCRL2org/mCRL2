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

#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/detail/container_utility.h"

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

BOOST_AUTO_TEST_CASE(test_find)
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

BOOST_AUTO_TEST_CASE(test_free_variables)
{
  data::variable b = bool_("b");
  data::data_expression_list d = { b };
  process_identifier P(core::identifier_string("P"), { b });
  process_instance pi(P, d);

  std::set<data::variable> free_variables = process::find_free_variables(pi);
  std::cout << "free variables: " << core::detail::print_set(free_variables) << std::endl;
  BOOST_CHECK(free_variables.find(b) != free_variables.end());
}

// The lineariser relies on the following behaviour of process::find_free_variables when determining whether a
// variable occurs in a pCRL process.
BOOST_AUTO_TEST_CASE(test_free_variables_pcrl)
{
  const data::variable b = bool_("b");
  const data::variable c = bool_("c");
  const data::variable x = bool_("x");
  const data::variable y = nat("y");
  const data::variable z = nat("z");
  const data::variable t = data::variable("t", data::sort_real::real_());
  const process_identifier P(core::identifier_string("P"), {b, c});
  const action_label a(core::identifier_string("a"), {data::sort_nat::nat()});
  const action a_y(a, {y});

  // P() keeps the values of all parameters, so they occur in it.
  const std::set<data::variable> V1
    = process::find_free_variables(process_instance_assignment(P, data::assignment_list()));
  BOOST_CHECK(V1 == std::set<data::variable>({b, c}));

  // In P(b = x) the parameter b is assigned, so it does not occur unless it occurs in the right hand side; the
  // unassigned parameter c occurs.
  const std::set<data::variable> V2
    = process::find_free_variables(process_instance_assignment(P, {data::assignment(b, x)}));
  BOOST_CHECK(V2 == std::set<data::variable>({c, x}));

  // In P(b = c) the variable c occurs both via the right hand side and as an unassigned parameter.
  const std::set<data::variable> V3
    = process::find_free_variables(process_instance_assignment(P, {data::assignment(b, c)}));
  BOOST_CHECK(V3 == std::set<data::variable>({c}));

  // A sum binds its variables, also when they are used to assign a parameter.
  const std::set<data::variable> V4 = process::find_free_variables(sum({y}, a_y));
  BOOST_CHECK(V4.empty());
  const std::set<data::variable> V5
    = process::find_free_variables(sum({x}, process_instance_assignment(P, {data::assignment(b, x)})));
  BOOST_CHECK(V5 == std::set<data::variable>({c}));

  // A stochastic operator binds its variables in the distribution and in the body.
  const data::data_expression distribution = data::equal_to(y, z);
  const std::set<data::variable> V6 = process::find_free_variables(stochastic_operator({y}, distribution, a_y));
  BOOST_CHECK(V6 == std::set<data::variable>({z}));

  // Variables in conditions and time stamps occur.
  const std::set<data::variable> V7 = process::find_free_variables(if_then(data::equal_to(z, y), at(a_y, t)));
  BOOST_CHECK(V7 == std::set<data::variable>({y, z, t}));

  // A variable that is bound by a sum does not occur outside its scope.
  const std::set<data::variable> V8 = process::find_free_variables(seq(sum({y}, a_y), a_y));
  BOOST_CHECK(mcrl2::utilities::detail::contains(V8, y));
  BOOST_CHECK(!mcrl2::utilities::detail::contains(process::find_free_variables(sum({y}, a_y)), y));
}

// process::find_all_variables also returns the formal parameters of called processes, including those that
// are assigned. It is therefore an over-approximation of the variables that occur in a process, and does not
// coincide with the 'strict' occurrence check of the lineariser (free variables and bound variables).
BOOST_AUTO_TEST_CASE(test_all_variables_includes_formal_parameters)
{
  const data::variable b = bool_("b");
  const data::variable c = bool_("c");
  const data::variable x = bool_("x");
  const process_identifier P(core::identifier_string("P"), {b, c});

  const std::set<data::variable> V
    = process::find_all_variables(sum({x}, process_instance_assignment(P, {data::assignment(b, x)})));
  BOOST_CHECK(V == std::set<data::variable>({b, c, x}));
}
