// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_equation_test.cpp
/// \brief Basic regression test for data equations.

#define BOOST_TEST_MODULE data_equation_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/data_equation.h"

using namespace mcrl2;
using namespace mcrl2::data;

void data_equation_test()
{
  basic_sort s("S");
  data::function_symbol c("c", s);
  data::function_symbol f("f", s);
  variable x("x", s);
  variable_list xl = { x };

  data_equation e(xl, c, x, f);
  BOOST_CHECK(e.variables() == xl);
  BOOST_CHECK(e.condition() == c);
  BOOST_CHECK(e.lhs() == x);
  BOOST_CHECK(e.rhs() == f);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  data_equation_test();
}


