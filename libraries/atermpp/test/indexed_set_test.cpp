// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file table_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include <map>
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/indexed_set.h"

using namespace std;
using namespace atermpp;

void test_indexed_set()
{
  indexed_set<atermpp::aterm> t(100, 75);

  std::pair<long, bool> p;
  p = t.put(read_term_from_string("a"));
  BOOST_CHECK(t.size() == 1);
  p = t.put(read_term_from_string("b"));
  BOOST_CHECK(t.size() == 2);

  {
    const indexed_set<atermpp::aterm>& t1 = t;
  }
  indexed_set<atermpp::aterm> t2 = t;

  BOOST_CHECK(t.index(read_term_from_string("a")) == 0);
  BOOST_CHECK(t.index(read_term_from_string("b")) == 1);

  atermpp::aterm a = t.get(0);
  BOOST_CHECK(a == read_term_from_string("a"));

  atermpp::aterm b = t.get(1);
  BOOST_CHECK(b == read_term_from_string("b"));
  t.erase(a); 
  BOOST_CHECK(t.size() == 1); 
             
  t.erase(read_term_from_string("x")); 
  BOOST_CHECK(t.size() == 1); 


  p = t.put(read_term_from_string("c"));
  BOOST_CHECK(t.size() == 2);
  BOOST_CHECK(p.first == 0);

  t.clear();
  BOOST_CHECK(t.size() == 0);

  std::map<int, indexed_set<atermpp::aterm> > x;
  x[2] = t;
}

int test_main(int argc, char* argv[])
{
  test_indexed_set();
  return 0;
}
