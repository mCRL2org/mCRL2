// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/configuration.h"
#include "mcrl2/utilities/indexed_set.h"

#include <thread>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(basic_test_indexed_set)
{
  indexed_set<std::string> t(1,100);

  std::pair<std::size_t, bool> p;
  p = t.insert("a");
  BOOST_CHECK(t.size() == 1);
  p = t.insert("b");
  BOOST_CHECK(t.size() == 2);

  {
    const indexed_set<std::string>& t1 = t;
    BOOST_CHECK(t1.size() == t.size());
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

BOOST_AUTO_TEST_CASE(test_indexed_set_parallel)
{
  if (detail::GlobalThreadSafe)
  {
    // One thread continuously modifies a local atermpp::vector of aterms while the main thread performs garbage collection extensively.
    std::vector<std::thread> threads;

    indexed_set<std::size_t, true> set(20);

    for (int i = 0; i < 20; ++i)
    {
      threads.emplace_back([&set](int index) 
      {
        // Insert every elements into the set.
        set.insert(5, index);
      }, i);
    }

    for (auto& thread : threads)
    {
      thread.join();
    }
  }
}