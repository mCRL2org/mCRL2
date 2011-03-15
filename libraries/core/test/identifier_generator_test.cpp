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
#include <boost/test/minimal.hpp>

#include "mcrl2/core/identifier_generator.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::core;

void identifier_generator_test()
{
  number_postfix_generator generator;
  generator.add_identifier("c6");
  std::string s;

  s = generator("c");
  BOOST_CHECK(s == "c7");

  std::vector<std::string> v;
  v.push_back("a1");
  v.push_back("c012");
  generator.add_identifiers(v.begin(), v.end());

  s = generator("c");
  BOOST_CHECK(s == "c13");

  s = generator("a");
  BOOST_CHECK(s == "a2");

  s = generator("a");
  BOOST_CHECK(s == "a3");

  s = generator("a2");
  BOOST_CHECK(s == "a4");

  s = generator();
  BOOST_CHECK(s == "FRESH_VAR1");

  generator.add_identifier("a0");
  s = generator("a2");
  BOOST_CHECK(s == "a5");

  core::garbage_collect();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  identifier_generator_test();

  return EXIT_SUCCESS;
}
