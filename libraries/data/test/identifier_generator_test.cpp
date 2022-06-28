// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file identifier_generator_test.cpp
/// \brief Test for identifier generators.

#define BOOST_TEST_MODULE identifier_generator_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/xyz_identifier_generator.h"

using namespace mcrl2;
using namespace mcrl2::data;

inline
core::identifier_string str(const std::string& s)
{
  return core::identifier_string(s);
}

void set_identifier_generator_test()
{
  set_identifier_generator generator;
  generator.add_identifier(str("c0"));
  core::identifier_string s;

  s = generator("c");
  BOOST_CHECK(s == str("c"));

  s = generator("c");
  BOOST_CHECK(s == str("c1"));
  BOOST_CHECK(generator.has_identifier(str("c1")));

  generator.remove_identifier(str("c1"));
  BOOST_CHECK(!generator.has_identifier(str("c1")));

  s = generator("c", false);
  BOOST_CHECK(s == str("c2"));
  BOOST_CHECK(!generator.has_identifier(str("c2")));
}

void multiset_identifier_generator_test()
{
  multiset_identifier_generator generator;
  generator.add_identifier(str("c0"));
  core::identifier_string s;

  s = generator("c");
  BOOST_CHECK(s == str("c"));

  s = generator("c");
  BOOST_CHECK(s == str("c1"));
  BOOST_CHECK(generator.has_identifier(str("c1")));

  generator.add_identifier(str("c1"));
  generator.remove_identifier(str("c1"));
  BOOST_CHECK(generator.has_identifier(str("c1")));
  generator.remove_identifier(str("c1"));
  BOOST_CHECK(!generator.has_identifier(str("c1")));

  s = generator("c", false);
  BOOST_CHECK(s == str("c2"));
  BOOST_CHECK(!generator.has_identifier(str("c2")));
}

void xyz_identifier_generator_test()
{
  xyz_identifier_generator generator;
  generator.add_identifier(str("X"));
  generator.add_identifier(str("Y0"));
  core::identifier_string s;

  s = generator("X");
  BOOST_CHECK(s == str("Y"));

  s = generator("X");
  BOOST_CHECK(s == str("Z"));

  s = generator("X");
  BOOST_CHECK(s == str("X0"));

  s = generator("X");
  BOOST_CHECK(s == str("Z0"));
}

BOOST_AUTO_TEST_CASE(test_main)
{
  set_identifier_generator_test();
  multiset_identifier_generator_test();
  xyz_identifier_generator_test();
}
