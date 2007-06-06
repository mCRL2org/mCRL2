// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/is_quoted.cpp
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

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  // unquoted function symbol s == "f"
  function_symbol s("\"f\"", 1, false);
  aterm_appl f(s, aterm("x"));

  // convert to string and back
  aterm_appl g = make_term(f.to_string());

  BOOST_CHECK(g.is_quoted());
  BOOST_CHECK(g.function() != s);
  function_symbol s1("f", 1, true);
  BOOST_CHECK(g.function() == s1);

  return 0;
}
