// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <cassert>
#include <vector>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"

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

  aterm_appl t = find_if(a, is_f());
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
  aterm_appl c = replace(a, make_term("f(x)"), make_term("x"));
  assert(c == make_term("x"));
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_find();
  test_replace();

  return 0;
}
