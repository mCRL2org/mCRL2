// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normalize_test.cpp
/// \brief Test for normalization functions.

#define BOOST_TEST_MODULE normalize_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/detail/normalize_and_or.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/detail/parse.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/utilities/detail/test_operation.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(test_normalize1)
{
  pbes_expression x = propositional_variable_instantiation{core::identifier_string("X"), data::data_expression_list()};
  pbes_expression y = propositional_variable_instantiation{core::identifier_string("Y"), data::data_expression_list()};
  pbes_expression z = propositional_variable_instantiation{core::identifier_string("Z"), data::data_expression_list()};
  pbes_expression f;
  pbes_expression f1;
  pbes_expression f2;

  f = not_(x);
  f = not_(f); // N.B. not_(not_(x)) does not work!
  f1 = pbes_system::normalize(f);
  f2 = x;
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f = imp(not_(x), y);
  f1 = pbes_system::normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = not_(and_(not_(x), not_(y)));
  f1 = pbes_system::normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(not_(x), not_(y)), z);
  f1 = pbes_system::normalize(f);
  f2 = or_(or_(x, y), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  x = data::variable("x", data::bool_());
  y = data::variable("y", data::bool_());
  z = data::variable("z", data::bool_());
  const data::data_expression& x1 = atermpp::down_cast<data::data_expression>(x);
  const data::data_expression& y1 = atermpp::down_cast<data::data_expression>(y);

  f  = not_(x);
  f1 = pbes_system::normalize(f);
  f2 = data::not_(x1);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(x, y), z);
  f1 = pbes_system::normalize(f);
  f2 = or_(or_(data::not_(x1), data::not_(y1)), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  pbes_expression T = true_();
  pbes_expression F = false_();
  x = pbes_expression(atermpp::aterm(core::detail::function_symbol_PBESImp(), T, F));
  y = pbes_system::normalize(x);
  std::cout << "x = " << x << std::endl;
  std::cout << "y = " << y << std::endl;

  data::variable_list ab = { data::variable("s", data::basic_sort("S")) };
  x = propositional_variable_instantiation{core::identifier_string("X"), data::data_expression_list()};
  y = and_(x, imp(pbes_expression(atermpp::aterm(core::detail::function_symbol_PBESAnd(), false_(), false_())), false_()));
  z = pbes_system::normalize(y);
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;
}

BOOST_AUTO_TEST_CASE(test_normalize2)
{
  // test case from Aad Mathijssen, 2/11/2008
  lps::specification spec=remove_stochastic_operators(lps::linearise("init tau + tau;"));
  state_formulas::state_formula formula  = state_formulas::parse_state_formula("nu X. [true]X", spec, false);
  bool timed = false;
  pbes_system::pbes p = pbes_system::lps2pbes(spec, formula, timed);
  pbes_system::normalize(p);
}

BOOST_AUTO_TEST_CASE(test_normalize3)
{
  // test case from Aad Mathijssen, 1-4-2008
  lps::specification spec=remove_stochastic_operators(lps::linearise(
                              "proc P = tau.P;\n"
                              "init P;        \n"));
  state_formulas::state_formula formula = state_formulas::parse_state_formula("![true*]<true>true", spec, false);
  bool timed = false;
  pbes_system::pbes p = pbes_system::lps2pbes(spec, formula, timed);
  pbes_system::normalize(p);
}

const std::string VARIABLE_SPECIFICATION =
  "datavar         \n"
  "  b:  Bool;     \n"
  "  b1: Bool;     \n"
  "  b2: Bool;     \n"
  "  b3: Bool;     \n"
  "                \n"
  "  n:  Nat;      \n"
  "  n1: Nat;      \n"
  "  n2: Nat;      \n"
  "  n3: Nat;      \n"
  "                \n"
  "  p:  Pos;      \n"
  "  p1: Pos;      \n"
  "  p2: Pos;      \n"
  "  p3: Pos;      \n"
  "                \n"
  "predvar         \n"
  "  X;            \n"
  "  Y: Nat;       \n"
  "  Z: Bool, Pos; \n"
  ;

inline
pbes_system::pbes_expression expr(const std::string& text)
{
  return pbes_system::parse_pbes_expression(text, VARIABLE_SPECIFICATION);
}

inline
pbes_expression parse(const std::string& expr)
{
  std::string var_decl =
    "datavar    \n"
    "  m: Nat;  \n"
    "  n: Nat;  \n"
    "           \n"
    "predvar    \n"
    "  X;       \n"
    "  Y;       \n"
    "  Z;       \n"
    ;

  std::string data_spec = "";
  return pbes_system::parse_pbes_expression(expr, var_decl, data_spec);
}

inline
pbes_expression norm(const pbes_expression& x)
{
  return pbes_system::detail::normalize_and_or(x);
}

inline
void test_normalize_and_or_equality(const std::string& expr1, const std::string& expr2)
{
  BOOST_CHECK(utilities::detail::test_operation(
    expr1,
    expr2,
    parse,
    std::equal_to<pbes_expression>(),
    norm,
    "normalize_and_or",
    norm,
    "normalize_and_or"
  ));
}

BOOST_AUTO_TEST_CASE(test_normalize_and_or)
{
  test_normalize_and_or_equality("X && Y", "Y && X");
  test_normalize_and_or_equality("X && X && Y", "X && Y && X");
  test_normalize_and_or_equality("X && X && Y", "Y && X && X");
}
