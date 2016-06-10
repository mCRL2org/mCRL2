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

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_io.h"

using namespace std;
using namespace atermpp;

// function object to test if it is an aterm_appl with function symbol "f"
struct is_f
{
  bool operator()(const atermpp::aterm& t) const
  {
    return (t.type_is_appl()) && aterm_appl(t).function().name() == "f";
  }
};

// function object to test if it is an aterm_appl with function symbol "a" or "b"
struct is_a_or_b
{
  bool operator()(const atermpp::aterm& t) const
  {
    return (t.type_is_appl()) &&
           (aterm_appl(t).function().name() == "a" || aterm_appl(t).function().name() == "b");
  }
};

void test_find()
{
  aterm_appl a(read_term_from_string("h(g(x),f(y),p(a(x,y),q(f(z))))"));

  aterm_appl t = find_if(a, is_f());
  assert(t == read_term_from_string("f(y)"));

  vector<atermpp::aterm> v;
  find_all_if(a, is_f(), back_inserter(v));
  assert(v.front() == read_term_from_string("f(y)"));
  assert(v.back() == read_term_from_string("f(z)"));
}

void test_replace()
{
  aterm_appl a (read_term_from_string("f(f(x))"));
  aterm_appl b = replace(a, aterm_appl(read_term_from_string("f(x)")), aterm_appl(read_term_from_string("x")));
  assert(b == read_term_from_string("f(x)"));
}

int main(int argc, char* argv[])
{
  test_find();
  test_replace();

  return 0;
}
