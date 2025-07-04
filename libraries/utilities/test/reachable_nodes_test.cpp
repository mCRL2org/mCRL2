// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/reachable_nodes.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2::utilities;

void test_reachable_nodes()
{
  using graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;

  graph g(6);
  boost::add_edge(0, 1, g);
  boost::add_edge(0, 2, g);
  boost::add_edge(1, 2, g);
  boost::add_edge(2, 4, g);
  boost::add_edge(3, 2, g);
  boost::add_edge(3, 5, g);

  std::vector<std::size_t> v;
  v.push_back(1);
  v.push_back(5);

  std::vector<std::size_t> nodes = reachable_nodes(g, v.begin(), v.end());
  BOOST_CHECK(nodes.size() == 4);
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 1) != nodes.end());
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 2) != nodes.end());
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 4) != nodes.end());
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 5) != nodes.end());
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_reachable_nodes();
}
