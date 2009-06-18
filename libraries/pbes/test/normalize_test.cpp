// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Test for normalization functions.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/modal_formula/detail/algorithms.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/pbes/detail/pfnf_visitor.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;

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
}

void test_normalize2()
{
  // test case from Aad Mathijssen, 2/11/2008
  lps::specification spec       = lps::mcrl22lps("init tau + tau;");
  modal_formula::state_formula formula  = modal_formula::detail::mcf2statefrm("nu X. [true]X", spec);
  bool timed = false;
  pbes_system::pbes<> p = pbes_system::lps2pbes(spec, formula, timed);
  p.normalize();
}

void test_normalize3()
{
  // test case from Aad Mathijssen, 1-4-2008
  lps::specification spec = lps::mcrl22lps(
    "proc P = tau.P;\n"
    "init P;        \n"
  );
  modal_formula::state_formula formula = modal_formula::detail::mcf2statefrm("![true*]<true>true", spec);
  bool timed = false;
  pbes_system::pbes<> p = pbes_system::lps2pbes(spec, formula, timed);
  p.normalize();
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
}

void test_pfnf_visitor()
{
  test_pfnf_expression("forall m:Nat. false");
  test_pfnf_expression("X && Y(3) || X");
  // test_pfnf_expression("forall m:Nat. (Y(m) || exists n:Nat. Y(n))");
  // test_pfnf_expression("forall m:Nat. (Y(m) || exists m:Nat. Y(m))");
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_normalize1();
  core::garbage_collect();
  test_normalize2();
  core::garbage_collect();
  test_normalize3();
  core::garbage_collect();
  test_pfnf_visitor();
  core::garbage_collect();

  return 0;
}
