// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file function_symbol_generator_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/function_symbol_generator.h"

using namespace atermpp;

void test_generator()
{
  std::string prefix("@");
  function_symbol_generator generator(prefix);
  const function_symbol& f = generator();

  BOOST_CHECK(f.name().find(prefix) == 0);

/*
  prefix = "a";
  function_symbol a10("a10", 0);
  const function_symbol& g = generator();
  BOOST_CHECK(g.name() == "a11");
*/
}

int test_main(int argc, char* argv[])
{
  test_generator();

  return 0;
}
