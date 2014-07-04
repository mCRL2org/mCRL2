// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file identifier_generator_test.cpp
/// \brief Test for identifier generators.

#include <iostream>
#include <string>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/set_identifier_generator.h"
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

// test if std::to_string is supported on all compilers
void to_string_test()
{
  int i = 42;
  std::cout << std::to_string(i) << std::endl;
}

int test_main(int argc, char** argv)
{
  set_identifier_generator_test();
  multiset_identifier_generator_test();
  xyz_identifier_generator_test();
  to_string_test();

  return EXIT_SUCCESS;
}
