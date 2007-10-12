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
"% Test Case 3                                                     \n"
"%                                                                 \n"
"% rename:                                                         \n"
"% var                                                             \n"
"%   x:Bool;                                                       \n"
"%   y:Nat;                                                        \n"
"%   z:Nat;                                                        \n"
"% rename                                                          \n"
"%   a(x,y) => a(x,y);                                             \n"
"                                                                  \n"
"act                                                               \n"
"  a: Bool#Nat;                                                    \n"
"                                                                  \n"
"proc                                                              \n"
"  X(x:Bool, y:Nat)= sum z:Nat. (y<=z && z<3) -> a(x,y).X(!x,y+1); \n"
"                                                                  \n"
"init                                                              \n"
"  X(true,0);                                                      \n"
;

const std::string SPECIFICATION2 =
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

void test_lps_rename()
{
  specification spec = mcrl22lps(SPECIFICATION);
  linear_process p = spec.process();
  std::set<identifier_string> forbidden_names;
  forbidden_names.insert(identifier_string("x"));
  forbidden_names.insert(identifier_string("y"));
  forbidden_names.insert(identifier_string("z"));
  linear_process q = rename_summation_variables(p, forbidden_names);

  for (summand_list::iterator i = q.summands().begin(); i != q.summands().end(); ++i)
  {
    for (data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); ++j)
    {
      BOOST_CHECK(std::find(forbidden_names.begin(), forbidden_names.end(), j->name()) == forbidden_names.end());
    }
  }
}

void test_state_formula_rename()
{
  specification spec    = mcrl22lps(SPECIFICATION2);

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

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  test_lps_rename();
  test_state_formula_rename();

  return 0;
}
