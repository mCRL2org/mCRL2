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

#include <map>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/indexed_set.h"

using namespace std;
using namespace atermpp;

void test_table()
{
  table t(100, 75);
  t.put(make_term("a"), make_term("f(a)"));
  BOOST_CHECK(t.table_keys().size() == 1);

  {
    table t1 = t;
  }
  table t2 = t;

  atermpp::aterm a = t.get(make_term("a"));
  BOOST_CHECK(a == make_term("f(a)"));

  std::map<int, table> x;
  x[2] = t;
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  test_table();
  return 0;
}
