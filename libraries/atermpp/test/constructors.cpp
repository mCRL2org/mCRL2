// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE constructors
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/atermpp/aterm_string.h"
#include "mcrl2/atermpp/aterm_io.h"

using namespace std;
using namespace atermpp;

BOOST_AUTO_TEST_CASE(test_main)
{
  aterm_int i(10);
  atermpp::aterm x=aterm_appl(function_symbol("x",0));
  atermpp::aterm y=aterm_appl(function_symbol("y",0));
  aterm_appl f(function_symbol("f", 2), x, y);
  BOOST_CHECK(pp(f) == "f(x,y)");
  aterm_appl f1(function_symbol("f", 0));
  BOOST_CHECK(pp(f1) == "f");

  atermpp::aterm_string s("s"); // g++ 3.4.4 complains if atermpp:: is removed :-(
  // aterm_string s1("s1(x)"); will generate an error!
}
