// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file numeric_string_test.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/utilities/numeric_string.h"
#include "mcrl2/utilities/test_utilities.h"

using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2;

BOOST_AUTO_TEST_CASE(test_zero)
{
  BOOST_CHECK_EQUAL(utilities::NrOfChars(0), 1);
}

BOOST_AUTO_TEST_CASE(test_1)
{
  BOOST_CHECK_EQUAL(utilities::NrOfChars(1), 1);
}

BOOST_AUTO_TEST_CASE(test_minus_1)
{
  BOOST_CHECK_EQUAL(utilities::NrOfChars(-1), 2);
}

BOOST_AUTO_TEST_CASE(test_481)
{
  BOOST_CHECK_EQUAL(utilities::NrOfChars(481), 3);
}

BOOST_AUTO_TEST_CASE(test_minus_481)
{
  BOOST_CHECK_EQUAL(utilities::NrOfChars(-481), 4);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
