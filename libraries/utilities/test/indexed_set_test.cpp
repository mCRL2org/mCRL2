// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/indexed_set.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test_framework.hpp>

#include <map>
#include <iostream>
#include <string>

using namespace std;
using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(basic_test_indexed_set)
{
  indexed_set<std::string> t(100);

  std::pair<std::size_t, bool> p;
  p = t.insert("a");
  BOOST_CHECK(t.size() == 1);
  p = t.insert("b");
  BOOST_CHECK(t.size() == 2);

  {
    const indexed_set<std::string>& t1 = t;
  }
  indexed_set<std::string> t2 = t;

  BOOST_CHECK(t.index("a") == 0);
  BOOST_CHECK(t.index("b") == 1);

  std::string a = t.at(0);
  BOOST_CHECK(a == "a");

  std::string b = t.at(1);
  BOOST_CHECK(b == "b");
             
  p = t.insert("c");
  BOOST_CHECK(t.size() == 3);
  BOOST_CHECK(p.first == 2);

  t.clear();
  BOOST_CHECK(t.size() == 0);

  std::map<std::size_t, indexed_set<std::string> > x;
  x[2] = t;
}

