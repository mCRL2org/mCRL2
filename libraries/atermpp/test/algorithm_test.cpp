// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_string.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/vector.h"

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

void test_algorithm()
{
  aterm_appl a = make_term("h(g(x),f(y),p(a(x,y),q(f(z))))");
  aterm_appl b = make_term("h(g(x),p(a(x,y),q(g(z))))");

  aterm_appl t = find_if(a, is_f());
  BOOST_CHECK(t == make_term("f(y)"));
  aterm_appl t1 = find_if(b, is_f());
  BOOST_CHECK(t1 == aterm_appl());

  atermpp::vector<aterm_appl> v;
  find_all_if(a, is_f(), back_inserter(v));
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
  {
    std::cout << *i << " ";
  }
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
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }

  {
    atermpp::aterm_string a1("a1");
    atermpp::aterm_string a2("a2");
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }

  {
    aterm_int a1 = make_term("1");
    aterm_int a2 = make_term("2");
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }

  {
    aterm_list a1 = make_term("[1,2]");
    aterm_list a2 = make_term("[3,2]");
    bool b = (a1 < a2);
    std::clog << "b = " << (b?"true":"false") << std::endl;
  }
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_algorithm();
  test_operators();
  test_for_each();

  return 0;
}
