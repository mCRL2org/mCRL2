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

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/utility.h"

using namespace std;
using namespace atermpp;

void test_aterm_function()
{
  // create an unquoted function symbol
  function_symbol sym("\"f\"", 1, false);
  BOOST_CHECK(!sym.is_quoted());
  atermpp::aterm x ( make_term("x"));

  aterm_appl a(sym, x);
  BOOST_CHECK(a.to_string() == "\"f\"(x)");
  BOOST_CHECK(!a.function().is_quoted());
  BOOST_CHECK(a.function() == sym);

  string s = a.to_string();
  aterm_appl b ( make_term(s));
  BOOST_CHECK(b.to_string() == "\"f\"(x)");
  BOOST_CHECK(b.function().is_quoted());     // this is quite unexpected
  BOOST_CHECK(b.function() != sym); // this is quite unexpected

  aterm_appl c (read_from_string(s));
  BOOST_CHECK(c.to_string() == "\"f\"(x)");
  BOOST_CHECK(c.function().is_quoted());     // this is quite unexpected
  BOOST_CHECK(c.function() != sym); // this is quite unexpected

  aterm_appl f ( make_term("f(g(a,b),c)"));
  aterm_appl g ( make_term("g(a,b)"));
  BOOST_CHECK(f(0) == g);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  test_aterm_function();
  return 0;
}
