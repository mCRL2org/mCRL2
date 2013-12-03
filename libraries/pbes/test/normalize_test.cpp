// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normalize_test.cpp
/// \brief Test for normalization functions.

#include <functional>
#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/utilities/detail/test_operation.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/detail/normalize_and_or.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_normalize1()
{
  using namespace pbes_system;
  namespace p = pbes_system::pbes_expr;

  pbes_expression x = propositional_variable_instantiation("x:X");
  pbes_expression y = propositional_variable_instantiation("y:Y");
  pbes_expression z = propositional_variable_instantiation("z:Z");
  pbes_expression f;
  pbes_expression f1;
  pbes_expression f2;

  f = p::not_(p::not_(x));
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

  f  = p::not_(p::and_(p::not_(x), p::not_(y)));
  f1 = pbes_system::normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = p::imp(p::and_(p::not_(x), p::not_(y)), z);
  f1 = pbes_system::normalize(f);
  f2 = p::or_(p::or_(x, y), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  x = data::variable("x", data::sort_bool::bool_());
  y = data::variable("y", data::sort_bool::bool_());
  z = data::variable("z", data::sort_bool::bool_());
  const data::data_expression& x1 = atermpp::aterm_cast<data::data_expression>(x);
  const data::data_expression& y1 = atermpp::aterm_cast<data::data_expression>(y);
  const data::data_expression& z1 = atermpp::aterm_cast<data::data_expression>(z);

  f  = not_(x);
  f1 = pbes_system::normalize(f);
  f2 = data::sort_bool::not_(x1);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(x, y), z);
  f1 = pbes_system::normalize(f);
  f2 = p::or_(p::or_(data::sort_bool::not_(x1), data::sort_bool::not_(y1)), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  pbes_expression T = p::true_();
  pbes_expression F = p::false_();
  x = pbes_expression(mcrl2::core::detail::gsMakePBESImp(T, F));
  y = pbes_system::normalize(x);
  std::cout << "x = " << x << std::endl;
  std::cout << "y = " << y << std::endl;

  data::variable_list ab = make_list(data::variable("s", data::basic_sort("S")));
  x = propositional_variable_instantiation("x:X");
  y = and_(x, imp(pbes_expression(mcrl2::core::detail::gsMakePBESAnd(p::false_(), p::false_())), p::false_()));
  z = pbes_system::normalize(y);
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;
}

void test_normalize2()
{
  // test case from Aad Mathijssen, 2/11/2008
  lps::specification spec       = lps::linearise("init tau + tau;");
  state_formulas::state_formula formula  = state_formulas::parse_state_formula("nu X. [true]X", spec);
  bool timed = false;
  pbes_system::pbes p = pbes_system::lps2pbes(spec, formula, timed);
  pbes_system::normalize(p);
}

void test_normalize3()
{
  // test case from Aad Mathijssen, 1-4-2008
  lps::specification spec = lps::linearise(
                              "proc P = tau.P;\n"
                              "init P;        \n"
                            );
  state_formulas::state_formula formula = state_formulas::parse_state_formula("![true*]<true>true", spec);
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

void test_normalize_and_or_equality(std::string expr1, std::string expr2)
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

void test_normalize_and_or()
{
  test_normalize_and_or_equality("X && Y", "Y && X");
  test_normalize_and_or_equality("X && X && Y", "X && Y && X");
  test_normalize_and_or_equality("X && X && Y", "Y && X && X");
}

int test_main(int argc, char** argv)
{
  test_normalize1();
  test_normalize2();
  test_normalize3();
  test_normalize_and_or();

  return 0;
}
