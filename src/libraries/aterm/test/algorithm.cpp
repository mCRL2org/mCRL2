// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/algorithm.cpp
// date          : 19/09/06
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>

#include "atermpp/aterm.h"
#include "atermpp/aterm_int.h"
#include "atermpp/aterm_list.h"
#include "atermpp/algorithm.h"
#include "atermpp/vector.h"

using namespace std;
using namespace atermpp;

bool is_appl(aterm t)
{
  return t.type() == AT_APPL;
}

// function object to test if it is an aterm_appl with function symbol "f"
struct is_f
{
  bool operator()(aterm t) const
  {
    return is_appl(t) && aterm_appl(t).function().name() == "f";
  }
};

void test_algorithm()
{
  aterm_appl a = make_term("h(g(x),f(y),p(a(x,y),q(f(z))))");

  aterm t = find_if(a, is_f());
  BOOST_CHECK(t == make_term("f(y)"));
  
  atermpp::vector<aterm> v;
  find_all_if(a, is_f(), back_inserter(v));
  BOOST_CHECK(v.front() == make_term("f(y)"));
  BOOST_CHECK(v.back() == make_term("f(z)"));
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);

  test_algorithm();

  return 0;
}
