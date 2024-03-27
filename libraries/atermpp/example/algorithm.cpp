// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm.cpp
/// \brief Add your file description here.

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_io.h"

using namespace atermpp;

// function object to test if it is an aterm with function symbol "f"
struct is_f
{
  bool operator()(const atermpp::aterm& t) const
  {
    return (t.type_is_appl()) && t.function().name() == "f";
  }
};

// function object to test if it is an aterm with function symbol "a" or "b"
struct is_a_or_b
{
  bool operator()(const atermpp::aterm& t) const
  {
    return (t.type_is_appl()) &&
           (t.function().name() == "a" || t.function().name() == "b");
  }
};

void test_find()
{
  aterm a(read_appl_from_string("h(g(x),f(y),p(a(x,y),q(f(z))))"));

  aterm t = find_if(a, is_f());
  assert(t == read_term_from_string("f(y)"));

  std::vector<atermpp::aterm> v;
  find_all_if(a, is_f(), back_inserter(v));
  assert(v.front() == read_term_from_string("f(y)"));
  assert(v.back() == read_term_from_string("f(z)"));
}

void test_replace()
{
  aterm a (read_appl_from_string("f(f(x))"));
  aterm b = replace(a, read_appl_from_string("f(x)"), read_appl_from_string("x"));
  assert(b == read_term_from_string("f(x)"));
}

int main(int /*argc*/, char** /*argv*/)
{
  test_find();
  test_replace();

  return 0;
}
