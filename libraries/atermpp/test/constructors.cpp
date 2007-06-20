// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constructors.cpp
/// \brief Add your file description here.

// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/constructors.cpp
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <iostream>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "atermpp/aterm_int.h"
#include "atermpp/aterm_real.h"
#include "atermpp/aterm_string.h"

using namespace std;
using namespace atermpp;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  aterm_int i(10);
  aterm_real r(2.5);
  aterm_appl f(function_symbol("f", 2), aterm("x"), aterm("y"));
  BOOST_CHECK(f.to_string() == "f(x,y)");

  atermpp::aterm_string s("s"); // g++ 3.4.4 complains if atermpp:: is removed :-(
  // aterm_string s1("s1(x)"); will generate an error!

  return 0;
}
