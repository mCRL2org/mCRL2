// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file function_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE function_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_io.h"

using namespace atermpp;

BOOST_AUTO_TEST_CASE(test_aterm_function)
{
  // create an unquoted function symbol
  function_symbol sym("f", 1);
  atermpp::aterm x ( read_term_from_string("x"));

  aterm a(sym, x);
  BOOST_CHECK(pp(a) == "f(x)");
  BOOST_CHECK(a.function() == sym);

  std::string s = pp(a);
  aterm b ( read_appl_from_string(s));
  BOOST_CHECK(pp(a) == "f(x)");
  BOOST_CHECK(b.function() == sym); 

  aterm c (read_appl_from_string(s));
  BOOST_CHECK(pp(c) == "f(x)");
  BOOST_CHECK(c.function() == sym); 

  aterm f ( read_appl_from_string("f(g(a,b),c)"));
  aterm g ( read_appl_from_string("g(a,b)"));
  BOOST_CHECK(f[0] == g);
}
