// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file substitute_test.cpp
/// \brief Test for the pbes rewriters.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/parse.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/substitutions/variable_assignment.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/substitutions.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_substitution()
{
  std::string text1 =
    "pbes nu X(n: Nat) = X(n + 1);\n"
    "init X(0);                   \n"
    ;
  pbes p1 = txt2pbes(text1);

  std::string text2 =
    "pbes nu X(n: Nat) = X(4 + 1);\n"
    "init X(0);                   \n"
    ;
  pbes p2 = txt2pbes(text2);

  data::mutable_map_substitution<> sigma;
  data::variable n("n", data::sort_nat::nat());
  sigma[n] = data::parse_data_expression("4");

  pbes p = p1;
  pbes_system::replace_free_variables(p.equations(), sigma);
  BOOST_CHECK(p == p1);

  std::cout << "--- p =\n" << pbes_system::pp(p) << std::endl;
  std::cout << "--- p1 =\n" << pbes_system::pp(p1) << std::endl;

  pbes_system::replace_variables(p, sigma);

  std::cout << "--- p =\n" << pbes_system::pp(p) << std::endl;
  std::cout << "--- p2 =\n" << pbes_system::pp(p2) << std::endl;

  // compare textual representations, to avoid conflicts between types
  BOOST_CHECK(pbes_system::pp(p) == pbes_system::pp(p2));
}

void test_propositional_variable_substitution()
{
  std::string text1 =
    "pbes                     \n"
    "nu X(m: Nat) = Y(m + 1); \n"
    "nu Y(n: Nat) = X(n + 2); \n"
    "init X(0);               \n"
    ;
  pbes p1 = txt2pbes(text1);

  std::string text2 =
    "pbes                                     \n"
    "nu X(m: Nat) = Y(m + 1);                 \n"
    "nu Y(n: Nat) = X(n + 2 + 1) && Y(n + 2); \n"
    "init X(0);                               \n"
    ;
  pbes p2 = txt2pbes(text2);

  pbes p = p1;
  propositional_variable X = p.equations().front().variable();
  pbes_expression phi = parse_pbes_expression("X(m + 1) && Y(m)", "datavar m: Nat; \npredvar X: Nat; Y: Nat");
  propositional_variable_substitution sigma(X, phi);
  pbes_system::replace_propositional_variables(p, sigma);
  std::cout << pbes_system::pp(p) << std::endl;
  std::cout << pbes_system::pp(p2) << std::endl;

  // compare textual representations, to avoid conflicts between types
  BOOST_CHECK(pbes_system::pp(p) == pbes_system::pp(p2));

}

inline
pbes_expression sigma(const pbes_expression& x)
{
  std::string var_decl = "datavar b: Bool; \npredvar X: Bool;";
  pbes_expression x1 = parse_pbes_expression("X(false)", var_decl);
  pbes_expression x2 = parse_pbes_expression("X(true) || true", var_decl);
  return x == x1 ? x2 : x;
}

void test_replace_pbes_expressions()
{
  std::string var_decl = "datavar b: Bool; \npredvar X: Bool;";
  pbes_expression x = parse_pbes_expression("X(b) || X(false)", var_decl);
  pbes_expression result = replace_pbes_expressions(x, sigma);
  pbes_expression expected_result = parse_pbes_expression("X(b) || X(true) || true", var_decl);
  if (!(result == expected_result))
  {
    std::cout << "error: " << pbes_system::pp(result) << " != " << pbes_system::pp(expected_result) << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

void test_replace_variables()
{
  pbes_expression x = parse_pbes_expression("forall n: Nat. exists m: Nat. val(m > n)");
  pbes_expression expected_result = parse_pbes_expression("forall n: Nat. exists m: Nat. val(n > n)");
  data::mutable_map_substitution<> sigma;
  data::variable m("m", data::sort_nat::nat());
  data::variable n("n", data::sort_nat::nat());
  sigma[m] = n;
  pbes_expression result = pbes_system::replace_variables(x, sigma);
  if (!(result == expected_result))
  {
    std::cout << "error: " << pbes_system::pp(result) << " != " << pbes_system::pp(expected_result) << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

void test_variable_assignment()
{
  pbes_expression x = parse_pbes_expression("forall n: Nat. exists m: Nat. val(m > n)");
  pbes_expression expected_result = parse_pbes_expression("forall n: Nat. exists m: Nat. val(n > n)");
  data::variable m("m", data::sort_nat::nat());
  data::variable n("n", data::sort_nat::nat());
  data::variable_assignment sigma(m, n);
  pbes_expression result = pbes_system::replace_variables(x, sigma);
  if (!(result == expected_result))
  {
    std::cout << "error: " << pbes_system::pp(result) << " != " << pbes_system::pp(expected_result) << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

int test_main(int argc, char* argv[])
{
  test_substitution();
  test_propositional_variable_substitution();
  test_replace_pbes_expressions();
  test_replace_variables();
  test_variable_assignment();

  return 0;
}
