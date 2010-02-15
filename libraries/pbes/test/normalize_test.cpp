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
#include "mcrl2/core/detail/test_operation.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/detail/pfnf_visitor.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/detail/normalize_and_or.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_normalize1()
{
  using namespace pbes_system;
  using namespace pbes_system::pbes_expr;

  pbes_expression x = propositional_variable_instantiation("x:X");
  pbes_expression y = propositional_variable_instantiation("y:Y");
  pbes_expression z = propositional_variable_instantiation("z:Z");
  pbes_expression f;
  pbes_expression f1;
  pbes_expression f2;

  f = not_(not_(x));
  f1 = normalize(f);
  f2 = x;
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f = imp(not_(x), y);
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = not_(and_(not_(x), not_(y)));
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(not_(x), not_(y)), z);
  f1 = normalize(f);
  f2 = or_(or_(x, y), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  x = data::variable("x", data::sort_bool::bool_());
  y = data::variable("y", data::sort_bool::bool_());
  z = data::variable("z", data::sort_bool::bool_());

  f  = not_(x);
  f1 = normalize(f);
  f2 = data::sort_bool::not_(x);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(x, y), z);
  f1 = normalize(f);
  f2 = or_(or_(data::sort_bool::not_(x), data::sort_bool::not_(y)), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  pbes_expression T = true_();
  pbes_expression F = false_();
  x = pbes_expression(mcrl2::core::detail::gsMakePBESImp(T, F));
  y = normalize(x);
  std::cout << "x = " << x << std::endl;
  std::cout << "y = " << y << std::endl;

  data::variable_list ab = make_list(data::variable("s", data::basic_sort("S")));
  x = propositional_variable_instantiation("x:X");
  y = and_(x, imp(pbes_expression(mcrl2::core::detail::gsMakePBESAnd(false_(), false_())), false_()));
  z = normalize(y);
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;
  core::garbage_collect();
}

void test_normalize2()
{
  // test case from Aad Mathijssen, 2/11/2008
  lps::specification spec       = lps::linearise("init tau + tau;");
  state_formulas::state_formula formula  = state_formulas::parse_state_formula("nu X. [true]X", spec);
  bool timed = false;
  pbes_system::pbes<> p = pbes_system::lps2pbes(spec, formula, timed);
  p.normalize();
  core::garbage_collect();
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
  pbes_system::pbes<> p = pbes_system::lps2pbes(spec, formula, timed);
  p.normalize();
  core::garbage_collect();
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

void test_pfnf_expression(std::string s)
{
  pbes_system::detail::pfnf_visitor<pbes_system::pbes_expression> visitor;
  pbes_system::pbes_expression t1 = expr(s);
  visitor.visit(t1);
  pbes_system::pbes_expression t2 = visitor.evaluate();
  data::rewriter datar;
  pbes_system::simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> R(datar);
  if (R(t1) != R(t2))
  {
    BOOST_CHECK(R(t1) == R(t2));
    std::cout << "--- failed test --- " << std::endl;
    std::cout << "t1    " << core::pp(t1) << std::endl;
    std::cout << "t2    " << core::pp(t2) << std::endl;
    std::cout << "R(t1) " << core::pp(R(t1)) << std::endl;
    std::cout << "R(t2) " << core::pp(R(t2)) << std::endl;
  }
  core::garbage_collect();
}

void test_pfnf_visitor()
{
  test_pfnf_expression("forall m:Nat. false");
  test_pfnf_expression("X && Y(3) || X");
  // test_pfnf_expression("forall m:Nat. (Y(m) || exists n:Nat. Y(n))");
  // test_pfnf_expression("forall m:Nat. (Y(m) || exists m:Nat. Y(m))");
  core::garbage_collect();
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
std::string print(const pbes_expression& x)
{
	return core::pp(x);
}

void test_normalize_and_or_equality(std::string expr1, std::string expr2)
{
  core::detail::test_operation(
    expr1,
    expr2,
    parse,
    print,
	  std::equal_to<pbes_expression>(),
	  detail::normalize_and_or,
    "normalize_and_or",
	  detail::normalize_and_or,
    "normalize_and_or"
  );
}

void test_normalize_and_or()
{
	test_normalize_and_or_equality("X && Y", "Y && X");
	test_normalize_and_or_equality("X && X && Y", "X && Y && X");
	test_normalize_and_or_equality("X && X && Y", "Y && X && X");
  core::garbage_collect();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_normalize1();
  test_normalize2();
  test_normalize3();
  test_pfnf_visitor();
  test_normalize_and_or();

  return 0;
}
