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

// very simple test for atermpp::table

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/dictionary.h"

using namespace std;
using namespace atermpp;

void test_dictionary()
{
  aterm a = make_term("a");
  aterm b = make_term("b");
  aterm c = make_term("c");

  dictionary dict;
  dict.put(a, b);
  BOOST_CHECK(dict.get(a) == b);
  dict.put(b, b);
  BOOST_CHECK(dict.get(a) == b);
  BOOST_CHECK(dict.get(b) == b);
  dict.put(a, c);
  BOOST_CHECK(dict.get(a) == c);
  dict.remove(a);
  BOOST_CHECK(dict.get(a) == aterm());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  test_dictionary();
  return 0;
}
