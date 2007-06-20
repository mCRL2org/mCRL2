// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm_test.cpp
/// \brief Add your file description here.

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
#include <set>
#include <string>
#include <boost/test/minimal.hpp>

#include "atermpp/aterm.h"
#include "atermpp/aterm_int.h"
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_string.h"
#include "atermpp/aterm_int.h"
#include "atermpp/aterm_real.h"
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

  aterm t = find_if(aterm(a), is_f());
  BOOST_CHECK(t == make_term("f(y)"));
  
  atermpp::vector<aterm> v;
  find_all_if(aterm(a), is_f(), back_inserter(v));
  BOOST_CHECK(v.front() == make_term("f(y)"));
  BOOST_CHECK(v.back() == make_term("f(z)"));
}

struct for_each_proc
{
  std::set<std::string>& m_names;
  
  for_each_proc(std::set<std::string>& names)
    : m_names(names)
  {}
  
  bool operator()(aterm_appl t)
  {
    m_names.insert(t.function().name());
    return true;
  }
};

void test_for_each()
{
  aterm_appl t = make_term("h(g(x),f(y))");
  std::set<std::string> names;
  for_each(t, for_each_proc(names));
  for (std::set<std::string>::iterator i = names.begin(); i != names.end(); ++i)
    std::cout << *i << " ";
  BOOST_CHECK(names.find("h") != names.end());
  BOOST_CHECK(names.find("g") != names.end());
  BOOST_CHECK(names.find("x") != names.end());
  BOOST_CHECK(names.find("f") != names.end());
  BOOST_CHECK(names.find("y") != names.end());
}

void test_operators()
{
  {
    aterm_appl a1 = make_term("a1");
    aterm_appl a2 = make_term("a2");
    bool b = (a1 < a2);
  }

  {
    atermpp::aterm_string a1 = make_term("a1");
    atermpp::aterm_string a2 = make_term("a2");
    bool b = (a1 < a2);
  }

  {
    aterm_int a1 = make_term("1");
    aterm_int a2 = make_term("2");
    bool b = (a1 < a2);
  }

  {
    aterm_real a1 = make_term("1.0");
    aterm_real a2 = make_term("2.0");
    bool b = (a1 < a2);
  }

  {
    aterm_list a1 = make_term("[1,2]");
    aterm_list a2 = make_term("[3,2]");
    bool b = (a1 < a2);
  }
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);

  test_algorithm();
  test_operators();
  test_for_each();

  return 0;
}
