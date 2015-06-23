// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_test.cpp
/// \brief Some tests for BES.

#include <boost/test/minimal.hpp>
#include "mcrl2/bes/bdd_expression.h"
#include "mcrl2/bes/print.h"

using namespace mcrl2;
using namespace mcrl2::bdd;

void test_bdd()
{
  if_ b1("b1", true_(), false_());
  if_ b2("b2", b1, b1);
  if_ b3("b3", b2, b1);
  std::cout << "b1 = " << b1 << std::endl;
  std::cout << "b2 = " << b2 << std::endl;
  std::cout << "b3 = " << b3 << std::endl;
  BOOST_CHECK(bdd::pp(true_()) == "true");
  BOOST_CHECK(bdd::pp(b1) == "if(true, false)");
  BOOST_CHECK(bdd::pp(b2) == "if(if(true, false), if(true, false))");
  BOOST_CHECK(bdd::pp(b3) == "if(if(if(true, false), if(true, false)), if(true, false))");
  BOOST_CHECK(is_if(b1));
  BOOST_CHECK(b2.left() == b1);
  BOOST_CHECK(b2.right() == b1);
  BOOST_CHECK(b3.left() == b2);
  BOOST_CHECK(b3.right() == b1);
  BOOST_CHECK(b1.name() == core::identifier_string("b1"));
  BOOST_CHECK(b2.name() == core::identifier_string("b2"));
  BOOST_CHECK(b3.name() == core::identifier_string("b3"));
}

int test_main(int argc, char* argv[])
{
  test_bdd();

  return 0;
}
