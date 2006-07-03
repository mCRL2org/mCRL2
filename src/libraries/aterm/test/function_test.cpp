// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/function_test.cpp
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"

using namespace std;
using namespace atermpp;

void test_aterm_function()
{
  // create an unquoted function symbol
  function_symbol sym("\"f\"", 1, false);
  BOOST_CHECK(!sym.is_quoted()); 
  aterm x = make_term("x");

  aterm_appl a(sym, x);
  BOOST_CHECK(a.to_string() == "\"f\"(x)");
  BOOST_CHECK(!a.is_quoted());
  BOOST_CHECK(a.function() == sym);

  string s = a.to_string();
  aterm_appl b = make_term(s);
  BOOST_CHECK(b.to_string() == "\"f\"(x)");
  BOOST_CHECK(b.is_quoted());     // this is quite unexpected
  BOOST_CHECK(b.function() != sym); // this is quite unexpected

  aterm_appl c = read_from_string(s);
  BOOST_CHECK(c.to_string() == "\"f\"(x)");
  BOOST_CHECK(c.is_quoted());     // this is quite unexpected
  BOOST_CHECK(c.function() != sym); // this is quite unexpected
  
  aterm_appl f = make_term("f(g(a,b),c)");
  aterm_appl g = make_term("g(a,b)");
  BOOST_CHECK(f.argument(0) == g);
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  test_aterm_function();
  return 0;
}
