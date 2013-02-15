// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rename_test.cpp
/// \brief Add your file description here.

#define MCRL2_STATE_FORMULA_BUILDER_DEBUG

#define BOOST_TEST_MODULE state_formula
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <iterator>
#include <set>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/utilities/test_utilities.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/state_formula_normalize.h"
#include "mcrl2/modal_formula/detail/state_formula_accessors.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/count_fixpoints.h"
#include "mcrl2/modal_formula/maximal_closed_subformula.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/linearise.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::state_formulas;

BOOST_AUTO_TEST_CASE(test_rename)
{
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

  using mcrl2::state_formulas::pp;
  specification spec    = linearise(SPECIFICATION);

  state_formula formula = parse_state_formula("(mu X. X) && (mu X. X)", spec);

  data::set_identifier_generator generator;
  generator.add_identifiers(lps::find_identifiers(spec));
  formula = rename_predicate_variables(formula, generator);

  BOOST_CHECK(pp(formula) == "(mu X1. X1) && (mu X. X)" || pp(formula) == "(mu X. X) && (mu X1. X1)");

  std::cerr << "formula: " << pp(formula) << std::endl;

  generator = data::set_identifier_generator();
  generator.add_identifiers(lps::find_identifiers(spec));
  formula = parse_state_formula("mu X. mu X. X", spec, false);
  std::cout << "formula: " << pp(formula) << std::endl;
  formula = rename_predicate_variables(formula, generator);
  std::cout << "formula: " << pp(formula) << std::endl;
  BOOST_CHECK_EQUAL(pp(formula), "mu X. mu X1. X1");
}

BOOST_AUTO_TEST_CASE(test_normalize)
{
  using namespace state_formulas::detail::accessors;

  state_formula x = state_formulas::variable(identifier_string("X"), data::data_expression_list());
  state_formula y = state_formulas::variable(identifier_string("Y"), data::data_expression_list());
  state_formula f;
  state_formula f1;
  state_formula f2;

  f = imp(not_(x), y);
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << state_formulas::pp(f) << std::endl;
  std::cout << "f1 = " << state_formulas::pp(f1) << std::endl;
  std::cout << "f2 = " << state_formulas::pp(f2) << std::endl;
  BOOST_CHECK_EQUAL(f1, f2);

  f  = not_(and_(not_(x), not_(y)));
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << state_formulas::pp(f) << std::endl;
  std::cout << "f1 = " << state_formulas::pp(f1) << std::endl;
  std::cout << "f2 = " << state_formulas::pp(f2) << std::endl;
  BOOST_CHECK_EQUAL(f1, f2);
}

BOOST_AUTO_TEST_CASE(test_type_checking)
{
  using namespace state_formulas::detail::accessors;

  specification context = linearise(
                            "sort B = struct d;"
                            "act a: List(B);"
                            "init a([d]);"
                          );

  state_formula formula = parse_state_formula("<a([d])>true", context);

  BOOST_CHECK(is_may(formula));
  BOOST_CHECK(is_regular_formula(act(formula)));
}

static inline
state_formula negate_variable(const variable& x)
{
  return state_formulas::not_(x);
}

BOOST_AUTO_TEST_CASE(test_not)
{
  data::data_expression_list args;
  variable v(core::identifier_string("v"), args);
  state_formula s = not_(v);
  BOOST_CHECK(is_not(s));

  state_formula t = negate_variable(v);
  BOOST_CHECK_EQUAL(s, t);

  // The following is expected to trigger an assertion failure
  // aterm_appl a = v;
  // state_formula t = not_(a);
}

// test case supplied by Jan Friso, 4-1-2011
BOOST_AUTO_TEST_CASE(test_parse)
{
  std::string spec_text =
    "act a:Nat; \n"
    "init a(1); \n"
    ;

  std::string formula_text = "<a(1)>true";

  lps::specification spec = lps::linearise(spec_text);
  state_formulas::state_formula f = state_formulas::parse_state_formula(formula_text, spec);

  std::cerr << "--- f ---\n" << state_formulas::pp(f) << "\n\n" << f << std::endl;
  std::set<core::identifier_string> ids = state_formulas::find_identifiers(f);
  BOOST_CHECK(ids.find(core::identifier_string("1")) == ids.end());
  BOOST_CHECK(ids.find(core::identifier_string("@c1")) != ids.end());
}

/* Not supported
BOOST_AUTO_TEST_CASE(test_find_nil)
{
  state_formula formula;
  specification spec;

  formula = parse_state_formula("(mu X. X) && (mu X. X)", spec);
  BOOST_CHECK(find_nil(formula) == false);

  formula = parse_state_formula("[nil]true", spec);
  BOOST_CHECK(find_nil(formula) == true);
}
*/

BOOST_AUTO_TEST_CASE(test_count_fixpoints)
{
  state_formula formula;
  specification spec;

  formula = parse_state_formula("(mu X. X) && (mu X. X)", spec);
  BOOST_CHECK_EQUAL(count_fixpoints(formula), 2);

  formula = parse_state_formula("exists b:Bool. (mu X. X) || forall b:Bool. (nu X. mu Y. (X || Y))", spec);
  BOOST_CHECK_EQUAL(count_fixpoints(formula), 3);
}

// Test case for bug #1094.
// This is expected to fail, due to the occurrence of an unbound variable Y
// in the formula.
BOOST_AUTO_TEST_CASE(test_1094)
{
  const std::string SPEC =
      "act  a,b,c,d;\n"
      "\n"
      "proc P(s3_X: Pos) =\n"
      "       (s3_X == 1) ->\n"
      "         a .\n"
      "         P(s3_X = 2)\n"
      "     + (s3_X == 2) ->\n"
      "         b .\n"
      "         P(s3_X = 3)\n"
      "     + (s3_X == 3) ->\n"
      "         a .\n"
      "         P(s3_X = 4)\n"
      "     + (s3_X == 3) ->\n"
      "         a .\n"
      "         P(s3_X = 5)\n"
      "     + (s3_X == 4) ->\n"
      "         c .\n"
      "         P(s3_X = 1)\n"
      "     + (s3_X == 5) ->\n"
      "         d .\n"
      "         P(s3_X = 1)\n"
      "     + delta;\n"
      "\n"
      "init P(1);\n"
    ;
  specification s(parse_linear_process_specification(SPEC));

  const std::string FORMULA = "[true*]([b] nu X. mu X.( [!c]X && [!c]Y))";

  BOOST_CHECK_THROW(parse_state_formula(FORMULA, s), mcrl2::runtime_error);
}

inline
state_formula sigma(const state_formula& x)
{
  variable X("X", data::data_expression_list());
  return x == X ? false_() : x;
}

BOOST_AUTO_TEST_CASE(test_replace_state_formulas)
{
  specification spec;
  state_formula f = parse_state_formula("(mu X. X) && (mu X. X)", spec);
  state_formula result = replace_state_formulas(f, sigma);
  state_formula expected_result = parse_state_formula("(mu X. false) && (mu X. false)", spec);
  if (!(result == expected_result))
  {
    std::cout << "error: " << state_formulas::pp(result) << " != " << state_formulas::pp(expected_result) << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

BOOST_AUTO_TEST_CASE(test_find_state_variables)
{
  specification spec;

  state_formula f = parse_state_formula("(mu X. nu Y. true && mu Z. X && Z)", spec);
  std::set<state_formulas::variable> v = state_formulas::find_state_variables(f);
  BOOST_CHECK(v.size() == 2);

  f = parse_state_formula("mu X. nu Y. (true && mu Z. (X && Y || Z))", spec);
  v = find_state_variables(f);
  BOOST_CHECK(v.size() == 3);

  state_formulas::variable X("X", data::data_expression_list());
  state_formulas::variable Y("Y", data::data_expression_list());
  state_formulas::variable Z("Z", data::data_expression_list());
  state_formula phi = state_formulas::and_(X, Y);
  v = find_state_variables(phi);
  BOOST_CHECK(v.size() == 2);
  v = find_free_state_variables(phi);
  BOOST_CHECK(v.size() == 2);
  state_formula psi = state_formulas::mu("X", data::assignment_list(), phi);
  v = find_state_variables(psi);
  BOOST_CHECK(v.size() == 2);
  v = find_free_state_variables(psi);
  BOOST_CHECK(v.size() == 1);
}

inline
bool contains(const std::set<state_formulas::state_formula>& v, const std::string& s)
{
  for (std::set<state_formulas::state_formula>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    if (state_formulas::pp(*i) == s)
    {
      return true;
    }
  }
  return false;
}

BOOST_AUTO_TEST_CASE(test_maximal_closed_subformulas)
{
  specification spec;
  state_formula f = parse_state_formula("(mu X. nu Y. true && mu Z. X && Z)", spec);
  std::set<state_formulas::state_formula> v = maximal_closed_subformulas(f);
  BOOST_CHECK(v.size() == 1);

  f = parse_state_formula("exists b: Bool. forall c: Bool. val(b) && (val(c) || true) && false", spec);
  v = maximal_closed_subformulas(f);
  BOOST_CHECK(v.size() == 1);

  state_formula g = exists(f).body();
  std::cout << "g = " << state_formulas::pp(g) << std::endl;
  v = maximal_closed_subformulas(g);
  for (std::set<state_formulas::state_formula>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    std::cout << "element " << state_formulas::pp(*i) << std::endl;
  }
  BOOST_CHECK(v.size() == 2);
  BOOST_CHECK(contains(v, "true"));
  BOOST_CHECK(contains(v, "false"));

  state_formula h = forall(g).body();
  v = maximal_closed_subformulas(h);
  std::cout << "h = " << state_formulas::pp(h) << std::endl;
  for (std::set<state_formulas::state_formula>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    std::cout << "element " << state_formulas::pp(*i) << std::endl;
  }
  BOOST_CHECK(v.size() == 2);
  BOOST_CHECK(contains(v, "true"));
  BOOST_CHECK(contains(v, "false"));
}
