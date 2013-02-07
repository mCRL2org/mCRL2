// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/utility.h"

using namespace std;
using namespace atermpp;

// function object to test if it is an aterm_appl with function symbol "f"
struct is_f
{
  bool operator()(aterm_appl t) const
  {
    return t.function().name() == "f";
  }
};

// function object to test if it is an aterm_appl with function symbol "g"
struct is_g
{
  bool operator()(aterm_appl t) const
  {
    return t.function().name() == "g";
  }
};

// function object to test if it is an aterm_appl with function symbol "z"
struct is_z
{
  bool operator()(aterm_appl t) const
  {
    return t.function().name() == "z";
  }
};

// function object to test if it is an aterm_appl with function symbol "a" or "b"
struct is_a_or_b
{
  bool operator()(aterm_appl t) const
  {
    return t.function().name() == "a" || t.function().name() == "b";
  }
};

// replaces function names f by g and vice versa
struct fg_replacer
{
  aterm_appl operator()(aterm_appl t) const
  {
    if (t.function().name() == "f")
    {
      return aterm_appl(function_symbol("g", t.function().arity()), t.begin(), t.end());
    }
    else if (t.function().name() == "g")
    {
      return aterm_appl(function_symbol("f", t.function().arity()), t.begin(), t.end());
    }
    else
    {
      return t;
    }
  }
};

// replaces function names f by g and vice versa, but stops the recursion once an f or g term is found
struct fg_partial_replacer
{
  std::pair< aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (t.function().name() == "f")
    {
      return std::make_pair(aterm_appl(function_symbol("g", t.function().arity()), t.begin(), t.end()), false);
    }
    else if (t.function().name() == "g")
    {
      return std::make_pair(aterm_appl(function_symbol("f", t.function().arity()), t.begin(), t.end()), false);
    }
    else
    {
      return std::make_pair(t, true);
    }
  }
};

void test_find()
{
  aterm_appl a(read_term_from_string("h(g(x),f(y),p(a(x,y),q(f(z))))"));

  aterm_appl t = find_if(a, is_f());
  BOOST_CHECK(t == read_term_from_string("f(y)"));

  aterm_appl a1(read_term_from_string("h(g(x),g(f(y)))"));
  t = partial_find_if(a1, is_f(), is_g());
  BOOST_CHECK(t == aterm_appl());
  t = partial_find_if(a1, is_f(), is_z());
  BOOST_CHECK(t == read_term_from_string("f(y)"));

  std::vector< aterm_appl> v;
  find_all_if(a, is_f(), back_inserter(v));
  BOOST_CHECK(v.front() == read_term_from_string("f(y)"));
  BOOST_CHECK(v.back() == read_term_from_string("f(z)"));
}

void test_replace()
{
  BOOST_CHECK(replace(aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("f(a)"))) == read_term_from_string("f(a)"));
  BOOST_CHECK(replace(aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("f(x)"))) == read_term_from_string("f(x)"));
  BOOST_CHECK(replace(atermpp::aterm_list(read_term_from_string("[x]")), atermpp::aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("f(x)"))) == read_term_from_string("[f(x)]"));

  aterm_appl a(read_term_from_string("f(f(x))"));
  aterm_appl b(replace(a, atermpp::aterm_appl(read_term_from_string("f(x)")), atermpp::aterm_appl(read_term_from_string("x"))));
  BOOST_CHECK(b == read_term_from_string("f(x)"));
  b = bottom_up_replace(a, atermpp::aterm_appl(read_term_from_string("f(x)")), atermpp::aterm_appl(read_term_from_string("x")));
  BOOST_CHECK(b == read_term_from_string("x"));

  atermpp::aterm f = read_term_from_string("[]");
  atermpp::aterm g = replace(f, a, b);
  BOOST_CHECK(f == read_term_from_string("[]"));
  BOOST_CHECK(g == read_term_from_string("[]"));

  atermpp::aterm x = read_term_from_string("g(f(x),f(y),h(f(x)))");
  atermpp::aterm y = replace(x, fg_replacer());
  atermpp::aterm z = partial_replace(x, fg_partial_replacer());

  BOOST_CHECK(y == read_term_from_string("f(f(x),f(y),h(f(x)))"));
  BOOST_CHECK(z == read_term_from_string("f(f(x),f(y),h(f(x)))"));
}

int test_main(int argc, char* argv[])
{
  test_find();
  test_replace();

  return 0;
}
