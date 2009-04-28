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
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/bes.h"

using namespace mcrl2;
using namespace mcrl2::bes;

typedef core::term_traits<boolean_expression> tr;

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
  atermpp::set<boolean_variable> expected;
  expected.insert(Y);
  expected.insert(Z);

  atermpp::set<boolean_variable> found;
  atermpp::find_all_if(Y, &tr::is_variable, std::inserter(found, found.end()));
  atermpp::find_all_if(Z, &tr::is_variable, std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);

  found.clear();
  atermpp::find_all_if(tr::and_(Y,Z), &tr::is_variable, std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);

  found.clear();
  atermpp::find_all_if(e.formula(), &tr::is_variable, std::inserter(found, found.end()));
  BOOST_CHECK(found == expected);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_expressions();
  core::garbage_collect();
  test_boolean_equation();
  core::garbage_collect();

  return 0;
}
