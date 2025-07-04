// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_test.cpp
/// \brief Some tests for BES.

#define BOOST_TEST_MODULE bes_test
#include <boost/test/included/unit_test.hpp>
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/print.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

using tr = core::term_traits<pbes_expression>;

void test_join()
{
  propositional_variable X("X");
  const pbes_expression& Z2(propositional_variable_instantiation(X.name()));
  pbes_expression Z3;
  Z3 = propositional_variable_instantiation(X.name());
  Z3=Z2;

  std::set<pbes_expression> s;
  s.insert(propositional_variable_instantiation("X1"));
  s.insert(propositional_variable_instantiation("X2"));
  pbes_expression x = join_or(s.begin(), s.end());
  std::cout << "x = " << pbes_system::pp(x) << std::endl;
}

void test_expressions()
{
  propositional_variable_instantiation X("X");
  propositional_variable_instantiation Y("Y");

  BOOST_CHECK(tr::is_prop_var(X));
  BOOST_CHECK(tr::is_prop_var(Y));

  pbes_expression true_(tr::true_());
  pbes_expression false_(tr::false_());
  pbes_expression not_(tr::not_(X));
  pbes_expression and_(tr::and_(X,Y));
  pbes_expression or_(tr::or_(X,Y));
  pbes_expression imp(tr::imp(X,Y));

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
  propositional_variable X("X");
  propositional_variable_instantiation Y("Y");
  propositional_variable_instantiation Z("Z");

  pbes_equation e(fixpoint_symbol::nu(), X, tr::and_(Y,Z));
  BOOST_CHECK(e.symbol() == fixpoint_symbol::nu());
  BOOST_CHECK(e.variable() == X);
  BOOST_CHECK(e.formula() == tr::and_(Y,Z));

  // Check for finding variables in the right hand side
  std::set<propositional_variable_instantiation> expected;
  expected.insert(Y);
  expected.insert(Z);

  std::set<propositional_variable_instantiation> found;

  find_propositional_variable_instantiations(Y, std::inserter(found, found.end()));
  find_propositional_variable_instantiations(Z, std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);

  find_propositional_variable_instantiations(tr::and_(Y,Z), std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);

  found.clear();
  find_propositional_variable_instantiations(e.formula(), std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);
}

void test_bes()
{
  propositional_variable X("X");
  propositional_variable Y("Y");
  propositional_variable Z("Z");

  pbes_equation eqX(fixpoint_symbol::nu(), X, tr::and_(propositional_variable_instantiation(X.name()), tr::and_(propositional_variable_instantiation(Y.name()),propositional_variable_instantiation(Z.name()))));
  pbes_equation eqY(fixpoint_symbol::nu(), Y, tr::and_(propositional_variable_instantiation(X.name()), propositional_variable_instantiation(Y.name())));
  pbes_equation eqZ(fixpoint_symbol::mu(), Z, tr::or_(propositional_variable_instantiation(Z.name()), propositional_variable_instantiation(X.name())));
  std::vector<pbes_equation> eqns;
  eqns.push_back(eqX);
  eqns.push_back(eqY);
  eqns.push_back(eqZ);

  pbes_system::pbes bes(data::data_specification(), eqns, propositional_variable_instantiation(X.name()));
  BOOST_CHECK(bes.is_closed());

  pbes_system::pp(bes);

  std::set<propositional_variable> occurring_variables = bes.occurring_variables();
  BOOST_CHECK(occurring_variables.size() == 3);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_expressions();
  test_boolean_equation();
  test_join();
  test_bes();
}
