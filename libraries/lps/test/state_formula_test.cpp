// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rename_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/basic/state_formula_rename.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/rename.h"
#include "mcrl2/pbes/detail/tools.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::detail;

const std::string SPECIFICATION =
"act a:Nat;                              \n"
"                                        \n"
"map smaller: Nat#Nat -> Bool;           \n"
"                                        \n"
"var x,y : Nat;                          \n"
"                                        \n"
"eqn smaller(x,y) = x < y;               \n"
"                                        \n"
"proc P(n:Nat) = sum m: Nat. a(m). P(m); \n"
"                                        \n"
"init P(0);                              \n"
;

void test_rename()
{
  specification spec    = mcrl22lps(SPECIFICATION);

  state_formula formula = mcf2statefrm("(mu X. X) && (mu X. X)", spec);
  set_identifier_generator generator;
  generator.add_identifiers(identifiers(spec));
  formula = rename_predicate_variables(formula, generator);
  BOOST_CHECK(pp(formula) == "(mu X00. X00) && (mu X. X)");

  generator = set_identifier_generator();
  generator.add_identifiers(identifiers(spec));
  formula = mcf2statefrm("mu X. mu X. X", spec);
  std::cout << "formula: " << pp(formula) << std::endl;
  formula = rename_predicate_variables(formula, generator);
  std::cout << "formula: " << pp(formula) << std::endl;
  BOOST_CHECK(pp(formula) == "mu X. mu X00. X00");
}

void test_normalize()
{
  using namespace state_frm;

  state_formula x = var(identifier_string("X"), data_expression_list());
  state_formula y = var(identifier_string("Y"), data_expression_list());
  state_formula z = var(identifier_string("Z"), data_expression_list());
  state_formula f;
  state_formula f1;
  state_formula f2;

  f = imp(x, not_(y));
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << pp(f) << std::endl;
  std::cout << "f1 = " << pp(f1) << std::endl;
  std::cout << "f2 = " << pp(f2) << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = not_(and_(not_(x), not_(y)));
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << pp(f) << std::endl;
  std::cout << "f1 = " << pp(f1) << std::endl;
  std::cout << "f2 = " << pp(f2) << std::endl;
  BOOST_CHECK(f1 == f2);
}

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  test_rename();
  test_normalize();

  return 0;
}
