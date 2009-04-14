#include <boost/test/minimal.hpp>
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#include <boost/graph/adjacency_list.hpp>
#include "mcrl2/core/reachable_nodes.h"

using namespace mcrl2::core;

void test_reachable_nodes()
{
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> graph;
  typedef boost::graph_traits<graph>::vertex_descriptor vertex_descriptor;

  graph g(6);
  boost::add_edge(0, 1, g);
  boost::add_edge(0, 2, g);
  boost::add_edge(1, 2, g);
  boost::add_edge(2, 4, g);
  boost::add_edge(3, 2, g);
  boost::add_edge(3, 5, g);

  std::vector<int> v;
  v.push_back(1);
  v.push_back(5);

  std::vector<int> nodes = reachable_nodes(g, v.begin(), v.end());
  BOOST_CHECK(nodes.size() == 4);
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 1) != nodes.end());
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 2) != nodes.end());
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 4) != nodes.end());
  BOOST_CHECK(std::find(nodes.begin(), nodes.end(), 5) != nodes.end());
}

int test_main(int, char*[])
{
  test_reachable_nodes();

  return 0;
}
