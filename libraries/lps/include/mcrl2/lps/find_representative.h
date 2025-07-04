// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/find_representative.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_FIND_REPRESENTATIVE_H
#define MCRL2_LPS_FIND_REPRESENTATIVE_H

#include <cstddef>
#include <vector>
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2::lps
{

/// \brief Search for a unique representative in a graph.
/// \param u0 The root of the graph.
/// \param generate_successors A function that generates successors of a node.
/// \details This function is based on an iterative version of Tarjan's strongly connected components algorithm.
/// It returns the smallest node of the first SCC that is detected. The first SCC is a TSCC, meaning
/// that it has no outgoing edges. In a confluent tau graph there is only one TSCC, so this should
/// guarantee a unique representative.
/// N.B. The implementation is based on https://llbit.se/?p=3379
template <typename Node, typename GenerateSuccessors>
Node find_representative(Node& u0, GenerateSuccessors generate_successors)
{
  using utilities::detail::contains;

  std::vector<Node> stack;
  std::map<Node, std::size_t> low;
  std::map<Node, std::size_t> disc;

  std::map<Node, std::vector<Node>> successors;
  std::vector<std::pair<Node, std::size_t>> work;

  successors[u0] = generate_successors(u0);
  work.emplace_back(std::make_pair(u0, 0));

  while (!work.empty())
  {
    Node u = work.back().first;
    std::size_t i = work.back().second;
    work.pop_back();

    if (i == 0)
    {
      std::size_t k = disc.size();
      disc[u] = k;
      low[u] = k;
      stack.push_back(u);
    }

    bool recurse = false;
    const std::vector<Node>& succ = successors[u];
    for (std::size_t j = i; j < succ.size(); j++)
    {
      const Node& v = succ[j];
      if (disc.find(v) == disc.end())
      {
        successors[v] = generate_successors(v);
        work.emplace_back(std::make_pair(u, j + 1));
        work.emplace_back(std::make_pair(v, 0));
        recurse = true;
        break;
      }
      else if (contains(stack, v))
      {
        low[u] = std::min(low[u], disc[v]);
      }
    }
    if (recurse)
    {
      continue;
    }
    if (disc[u] == low[u])
    {
      // an SCC has been found; return the node with the minimum value in this SCC
      Node result = u;
      while (true)
      {
        const auto& v = stack.back();
        if (v == u)
        {
          break;
        }
        if (v < result)
        {
          result = v;
        }
        stack.pop_back();
      }
      return result;
    }
    if (!work.empty())
    {
      Node v = u;
      u = work.back().first;
      low[u] = std::min(low[u], low[v]);
    }
  }
  throw mcrl2::runtime_error("find_representative did not find a solution");
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_FIND_REPRESENTATIVE_H
