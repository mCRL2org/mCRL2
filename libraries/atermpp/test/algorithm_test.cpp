// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE algorithm_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_string.h"
#include "mcrl2/atermpp/algorithm.h"

using namespace atermpp;

// function object to test if it is an aterm with function symbol "f"
struct is_f
{
  bool operator()(const aterm& t) const
  {
    return t.function().name() == "f";
  }
};

BOOST_AUTO_TEST_CASE(test_algorithm)
{
  aterm a (read_appl_from_string("h(g(x),f(y),p(a(x,y),q(f(z))))"));
  aterm b (read_appl_from_string("h(g(x),p(a(x,y),q(g(z))))"));

  aterm t = find_if(a, is_f());
  BOOST_CHECK(t == read_term_from_string("f(y)"));
  aterm t1 = find_if(b, is_f());
  BOOST_CHECK(t1 == aterm());

  std::vector<aterm> v;
  find_all_if(a, is_f(), back_inserter(v));
  BOOST_CHECK(v.front() == read_term_from_string("f(y)"));
  BOOST_CHECK(v.back() == read_term_from_string("f(z)"));
}

struct for_each_proc
{
  std::set<std::string>& m_names;

  for_each_proc(std::set<std::string>& names)
    : m_names(names)
  {}

  bool operator()(const aterm& t)
  {
    if(t.type_is_appl())
    {
      m_names.insert(t.function().name());
      return true;
    }
    else
    {
      return false;
    }
  }

};

BOOST_AUTO_TEST_CASE(test_for_each)
{
  aterm t (read_appl_from_string("h(g(x),f(y))"));
  std::set<std::string> names;
  atermpp::for_each(t, for_each_proc(names));
  for (const std::string& name : names)
  {
    std::cout << name << " ";
  }
  BOOST_CHECK(names.find("h") != names.end());
  BOOST_CHECK(names.find("g") != names.end());
  BOOST_CHECK(names.find("x") != names.end());
  BOOST_CHECK(names.find("f") != names.end());
  BOOST_CHECK(names.find("y") != names.end());
}

BOOST_AUTO_TEST_CASE(test_operators)
{
  {
    aterm a1 (read_appl_from_string("a1"));
    aterm a2 (read_appl_from_string("a2"));
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }

  {
    atermpp::aterm_string a1("a1");
    atermpp::aterm_string a2("a2");
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }

  {
    aterm_int a1 = read_int_from_string("1");
    aterm_int a2 = read_int_from_string("2");
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }

  {
    aterm_list a1 = read_list_from_string("[1,2]");
    aterm_list a2 = read_list_from_string("[3,2]");
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }
}
