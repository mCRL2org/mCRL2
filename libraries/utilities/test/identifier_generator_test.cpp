// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/number_postfix_generator.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;
using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(identifier_generator_test)
{
  number_postfix_generator generator;
  generator.add_identifier("c6");
  std::string s;

  s = generator("c");
  BOOST_CHECK_EQUAL(s, "c7");

  std::vector<std::string> v;
  v.emplace_back("a1");
  v.emplace_back("c012");
  generator.add_identifiers(v.begin(), v.end());

  s = generator("c");
  BOOST_CHECK_EQUAL(s, "c13");

  s = generator("a");
  BOOST_CHECK_EQUAL(s, "a2");

  s = generator("a");
  BOOST_CHECK_EQUAL(s, "a3");

  s = generator("a2");
  BOOST_CHECK_EQUAL(s, "a4");

  s = generator();
  BOOST_CHECK_EQUAL(s, "FRESH_VAR");

  s = generator();
  BOOST_CHECK_EQUAL(s, "FRESH_VAR1");

  s = generator("b", false);
  BOOST_CHECK_EQUAL(s, "b");

  s = generator("b", false);
  BOOST_CHECK_EQUAL(s, "b");

  s = generator("b");
  BOOST_CHECK_EQUAL(s, "b");

  s = generator("b", false);
  BOOST_CHECK_EQUAL(s, "b1");

  generator.add_identifier("a0");
  s = generator("a2");
  BOOST_CHECK_EQUAL(s, "a5");
}
