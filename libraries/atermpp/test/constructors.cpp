// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE constructors
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_string.h"
#include "mcrl2/atermpp/aterm_io.h"

using namespace atermpp;

BOOST_AUTO_TEST_CASE(test_main)
{
  aterm_int i(10);
  atermpp::aterm x=aterm(function_symbol("x",0));
  atermpp::aterm y=aterm(function_symbol("y",0));
  aterm f(function_symbol("f", 2), x, y);
  BOOST_CHECK(pp(f) == "f(x,y)");
  aterm f1(function_symbol("f", 0));
  BOOST_CHECK(pp(f1) == "f");

  function_symbol f2("f2", 8);
  aterm_int d(1);
  aterm long_term(f2,d,d,d,d,d,d,d,d);
  BOOST_CHECK(pp(long_term) == "f2(1,1,1,1,1,1,1,1)");

  function_symbol f3("f2", 16);
  aterm very_long_term(f3,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d);
  BOOST_CHECK(pp(very_long_term) == "f2(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)");

  atermpp::aterm_string s("s"); // g++ 3.4.4 complains if atermpp:: is removed :-(
  // aterm_string s1("s1(x)"); will generate an error!
}
