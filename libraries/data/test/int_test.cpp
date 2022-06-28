// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file int_test.cpp
/// \brief Basic regression test for data expressions.

#define BOOST_TEST_MODULE int_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/standard_numbers_utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void recogniser_test()
{
  variable i("i", sort_int::int_());
  application minus(sort_int::minus(i, sort_int::int_("5")));
  application negate(sort_int::negate(i));

  BOOST_CHECK(sort_int::is_minus_application(minus));
  BOOST_CHECK(!sort_int::is_negate_application(minus));
  BOOST_CHECK(sort_int::is_negate_application(negate));
  BOOST_CHECK(!sort_int::is_minus_application(negate));
}

BOOST_AUTO_TEST_CASE(test_main)
{
  recogniser_test();
}
