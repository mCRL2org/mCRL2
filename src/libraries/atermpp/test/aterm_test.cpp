// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/aterm_test.cpp
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
#include "atermpp/aterm_int.h"
#include "atermpp/utility.h"

using namespace std;
using namespace atermpp;

void test_aterm()
{
  aterm a = make_term("f(x)");
  aterm label = make_term("label");
  aterm annotation = make_term("annotation");
  aterm b = set_annotation(a, label, annotation);
  BOOST_CHECK(a != b);
  aterm c = remove_annotation(b, label);
  BOOST_CHECK(a == c);
  
  aterm d = aterm_int(10);
  BOOST_CHECK(aterm_int(d).value() == 10);
  
  aterm e = aterm();
  BOOST_CHECK(!e);
  
  aterm_string s = make_term("\"abc\"");
  std::string s1 = unquote(s);
  BOOST_CHECK(s1 == "abc");
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  test_aterm();
  return 0;
}
