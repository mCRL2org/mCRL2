// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/power_of_two.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(test_is_power_of_two)
{
  BOOST_CHECK(!is_power_of_two(0u));
  BOOST_CHECK(is_power_of_two(1u));
  BOOST_CHECK(is_power_of_two(2u));
  BOOST_CHECK(!is_power_of_two(3u));
  BOOST_CHECK(is_power_of_two(4u));
  BOOST_CHECK(!is_power_of_two(1000u));
  BOOST_CHECK(is_power_of_two(1024u));
  BOOST_CHECK(is_power_of_two(std::size_t(1) << 40));
}

BOOST_AUTO_TEST_CASE(test_round_up_to_power_of_two)
{
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(0u), 1u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(1u), 1u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(2u), 2u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(3u), 4u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(5u), 8u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(7u), 8u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(1024u), 1024u);
  BOOST_CHECK_EQUAL(round_up_to_power_of_two(1025u), 2048u);
}
