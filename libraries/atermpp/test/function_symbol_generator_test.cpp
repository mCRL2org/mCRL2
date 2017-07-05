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
  const function_symbol f = generator();
  BOOST_CHECK(f.name().find(prefix) == 0);
  const function_symbol f1 = generator();
  BOOST_CHECK(f1.name().find(prefix) == 0);
  BOOST_CHECK(f!=f1);
  const function_symbol f2 = generator();
  BOOST_CHECK(f2.name().find(prefix) == 0);
  BOOST_CHECK(f!=f2);
  BOOST_CHECK(f1!=f2);


  prefix = "a";
  function_symbol a10("a10", 0);
  function_symbol_generator agenerator(prefix);
  function_symbol g = agenerator();
  BOOST_CHECK(g.name() == "a11");
  BOOST_CHECK(g.name() != a10.name());

  function_symbol a100("a100", 0);
  g = agenerator();
  BOOST_CHECK(g.name() == "a101");

  function_symbol_generator zgen("z");
  function_symbol q1 = zgen();
  zgen.clear();
  function_symbol q2 = zgen();
  BOOST_CHECK(q1 == q2);
  std::cout << "q1 == " << q1 << " name = " << q1.name() << " arity = " << q1.arity() << std::endl;
  std::cout << "q2 == " << q2 << " name = " << q2.name() << " arity = " << q2.arity() << std::endl;
}

int test_main(int argc, char* argv[])
{
  test_generator();

  return 0;
}
