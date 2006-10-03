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
#include <cassert>
#include <vector>

#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/algorithm.h"

using namespace std;
using namespace atermpp;

// function object to test if it is an aterm_appl with function symbol "f"
struct is_f
{
  bool operator()(aterm t) const
  {
    return (t.type() == AT_APPL) && aterm_appl(t).function().name() == "f";
  }
};

// function object to test if it is an aterm_appl with function symbol "a" or "b"
struct is_a_or_b
{
  bool operator()(aterm t) const
  {
    return (t.type() == AT_APPL) &&
    (aterm_appl(t).function().name() == "a" || aterm_appl(t).function().name() == "b");
  }
};

void test_find()
{
  aterm_appl a = make_term("h(g(x),f(y),p(a(x,y),q(f(z))))");

  aterm t = find_if(a, is_f());
  assert(t == make_term("f(y)"));
  
  vector<aterm> v;
  find_all_if(a, is_f(), back_inserter(v));
  assert(v.front() == make_term("f(y)"));
  assert(v.back() == make_term("f(z)"));
}

void test_replace()
{
  aterm_appl a = make_term("f(f(x))");
  aterm_appl b = replace(a, make_term("f(x)"), make_term("x"));
  assert(b == make_term("f(x)"));
  aterm_appl c = replace(a, make_term("f(x)"), make_term("x"), true);
  assert(c == make_term("x"));

  aterm_appl d = make_term("h(g(b),f(a),p(a(x,y),q(a(a))))");
  aterm_appl e = replace_if(d, is_a_or_b(), make_term("u"));
  assert(e == make_term("h(g(u),f(u),p(u,q(u)))"));
}

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);

  test_find();
  test_replace();

  return 0;
}
