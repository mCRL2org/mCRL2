// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_test.cpp
/// \brief Some tests for BES.

#include <boost/test/minimal.hpp>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/find.h"
#include "mcrl2/bes/print.h"

using namespace mcrl2;
using namespace mcrl2::bes;

typedef core::term_traits<boolean_expression> tr;

void test_join()
{
  boolean_variable X("X");
  boolean_expression Z1 = X;
  boolean_expression Z2(X);
  boolean_expression Z3;
  Z3 = X;

  std::set<boolean_expression> s;
  s.insert(boolean_variable("X1"));
  s.insert(boolean_variable("X2"));
  boolean_expression x = join_or(s.begin(), s.end());
  std::cout << "x = " << bes::pp(x) << std::endl;

#ifdef MCRL2_JOIN_TEST
// The gcc compiler gives the following error:
//
// D:\mcrl2\libraries\core\include/mcrl2/utilities/detail/join.h:54:22: error: call
// of overloaded 'boolean_expression(const mcrl2::bes::boolean_variable&)' is
// ambiguous
//
// This seems to be triggered by an incorrect optimization, in which the return
// type of the function false_() is discarded.
//
  std::set<boolean_variable> sv;
  sv.insert(boolean_variable("X1"));
  sv.insert(boolean_variable("X2"));
  x = join_or(sv.begin(), sv.end());
  std::cout << "x = " << bes::pp(x) << std::endl;
#endif

}

void test_expressions()
{
  boolean_variable X("X");
  boolean_variable Y("Y");

  BOOST_CHECK(tr::is_variable(X));
  BOOST_CHECK(tr::is_variable(Y));

  boolean_expression true_(tr::true_());
  boolean_expression false_(tr::false_());
  boolean_expression not_(tr::not_(X));
  boolean_expression and_(tr::and_(X,Y));
  boolean_expression or_(tr::or_(X,Y));
  boolean_expression imp(tr::imp(X,Y));

  BOOST_CHECK(tr::is_true(true_));
  BOOST_CHECK(!tr::is_true(false_));
  BOOST_CHECK(!tr::is_true(not_));
  BOOST_CHECK(!tr::is_true(and_));
  BOOST_CHECK(!tr::is_true(or_));
  BOOST_CHECK(!tr::is_true(imp));

  BOOST_CHECK(!tr::is_false(true_));
  BOOST_CHECK(tr::is_false(false_));
  BOOST_CHECK(!tr::is_false(not_));
  BOOST_CHECK(!tr::is_false(and_));
  BOOST_CHECK(!tr::is_false(or_));
  BOOST_CHECK(!tr::is_false(imp));

  BOOST_CHECK(!tr::is_not(true_));
  BOOST_CHECK(!tr::is_not(false_));
  BOOST_CHECK(tr::is_not(not_));
  BOOST_CHECK(!tr::is_not(and_));
  BOOST_CHECK(!tr::is_not(or_));
  BOOST_CHECK(!tr::is_not(imp));

  BOOST_CHECK(!tr::is_and(true_));
  BOOST_CHECK(!tr::is_and(false_));
  BOOST_CHECK(!tr::is_and(not_));
  BOOST_CHECK(tr::is_and(and_));
  BOOST_CHECK(!tr::is_and(or_));
  BOOST_CHECK(!tr::is_and(imp));

  BOOST_CHECK(!tr::is_or(true_));
  BOOST_CHECK(!tr::is_or(false_));
  BOOST_CHECK(!tr::is_or(not_));
  BOOST_CHECK(!tr::is_or(and_));
  BOOST_CHECK(tr::is_or(or_));
  BOOST_CHECK(!tr::is_or(imp));

  BOOST_CHECK(!tr::is_imp(true_));
  BOOST_CHECK(!tr::is_imp(false_));
  BOOST_CHECK(!tr::is_imp(not_));
  BOOST_CHECK(!tr::is_imp(and_));
  BOOST_CHECK(!tr::is_imp(or_));
  BOOST_CHECK(tr::is_imp(imp));
}

void test_boolean_equation()
{
  boolean_variable X("X");
  boolean_variable Y("Y");
  boolean_variable Z("Z");

  boolean_equation e(fixpoint_symbol::nu(), X, tr::and_(Y,Z));
  BOOST_CHECK(e.symbol() == fixpoint_symbol::nu());
  BOOST_CHECK(e.variable() == X);
  BOOST_CHECK(e.formula() == tr::and_(Y,Z));

  // Check for finding variables in the right hand side
  std::set<boolean_variable> expected;
  expected.insert(Y);
  expected.insert(Z);

  std::set<boolean_variable> found;

  find_boolean_variables(Y, std::inserter(found, found.end()));
  find_boolean_variables(Z, std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);

  find_boolean_variables(tr::and_(Y,Z), std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);

  found.clear();
  find_boolean_variables(e.formula(), std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);
}

void test_bes()
{
  boolean_variable X("X");
  boolean_variable Y("Y");
  boolean_variable Z("Z");

  boolean_equation eqX(fixpoint_symbol::nu(), X, tr::and_(X, tr::and_(Y,Z)));
  boolean_equation eqY(fixpoint_symbol::nu(), Y, tr::and_(X, Y));
  boolean_equation eqZ(fixpoint_symbol::mu(), Z, tr::or_(Z, X));
  std::vector<boolean_equation> eqns;
  eqns.push_back(eqX);
  eqns.push_back(eqY);
  eqns.push_back(eqZ);

  boolean_equation_system<> bes(eqns, X);
  BOOST_CHECK(bes.is_closed());

  bes::pp(bes);

  std::set<boolean_variable> occurring_variables = bes.occurring_variables();
  BOOST_CHECK(occurring_variables.size() == 3);
}

int test_main(int argc, char* argv[])
{
  test_expressions();
  test_boolean_equation();
  test_join();

  return 0;
}
