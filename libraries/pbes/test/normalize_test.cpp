// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/modal_formula/detail/algorithms.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/normalize.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::modal;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;

void test_normalize1()
{
  using namespace pbes_expr;

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

  x = data_variable("x:X");
  y = data_variable("y:Y");
  z = data_variable("z:Z");

  f  = not_(x);
  f1 = normalize(f);
  f2 = data_expr::not_(x);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(x, y), z);
  f1 = normalize(f);
  f2 = or_(or_(data_expr::not_(x), data_expr::not_(y)), z);
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

  data_variable_list ab;
  ab = push_front(ab, data_variable("s:S"));
  x = propositional_variable_instantiation("x:X");
  y = and_(x, imp(pbes_expression(mcrl2::core::detail::gsMakePBESAnd(false_(), false_())), false_()));
  z = normalize(y);
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;
}

void test_normalize2()
{
  // test case from Aad Mathijssen, 2/11/2008
  specification spec     = mcrl22lps("init tau + tau;");
  state_formula formula  = mcrl2::modal::detail::mcf2statefrm("nu X. [true]X", spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  p.normalize();
}

void test_normalize3()
{
  // test case from Aad Mathijssen, 1-4-2008
  specification spec = mcrl22lps(
    "proc P = tau.P;\n"
    "init P;        \n"
  );
  state_formula formula  = mcrl2::modal::detail::mcf2statefrm("![true*]<true>true", spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  p.normalize();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_normalize1(); 
  test_normalize2();
  test_normalize3();

  return 0;
}
