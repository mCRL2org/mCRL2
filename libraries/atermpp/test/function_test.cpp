// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file function_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_appl.h"

using namespace std;
using namespace atermpp;

void test_aterm_function()
{
  // create an unquoted function symbol
  function_symbol sym("f", 1);
  atermpp::aterm x ( read_term_from_string("x"));

  aterm_appl a(sym, x);
  BOOST_CHECK(pp(a) == "f(x)");
  BOOST_CHECK(a.function() == sym);

  string s = pp(a);
  aterm_appl b ( read_term_from_string(s));
  BOOST_CHECK(pp(a) == "f(x)");
  BOOST_CHECK(b.function() == sym); 

  aterm_appl c (read_term_from_string(s));
  BOOST_CHECK(pp(c) == "f(x)");
  BOOST_CHECK(c.function() == sym); 

  aterm_appl f ( read_term_from_string("f(g(a,b),c)"));
  aterm_appl g ( read_term_from_string("g(a,b)"));
  BOOST_CHECK(f[0] == g);
}

int test_main(int argc, char* argv[])
{
  test_aterm_function();
  return 0;
}
