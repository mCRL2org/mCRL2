// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_find_loops.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_FIND_LOOPS_H
#define MCRL2_PBES_PBESINST_FIND_LOOPS_H

#include "mcrl2/pbes/simple_structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
bool find_loop(const simple_structure_graph& G,
               structure_graph::index_type v,
               structure_graph::index_type w,
               std::size_t p,
               std::unordered_map<structure_graph::index_type, bool>& visited
)
{
  const auto& w_ = G.find_vertex(w);
  if (w_.decoration == structure_graph::d_true || w_.decoration == structure_graph::d_false)
  {
    return false;
  }
  if (w_.rank != data::undefined_index() && w_.rank != p)
  {
    return false;
  }
  auto i = visited.find(w);
  if (i != visited.end())
  {
    return i->second;
  }

  if (w_.decoration == structure_graph::d_none ||
      ((w_.rank % 2 == 0 && w_.decoration == structure_graph::d_disjunction) ||
       (w_.rank % 2 != 0 && w_.decoration == structure_graph::d_conjunction))
          )
  {
    visited[w] = false;
    for (structure_graph::index_type u: w_.successors)
    {
      if (u == v || find_loop(G, v, u, p, visited))
      {
        visited[w] = true;
        return true;
      }
    }
  }
  else
  {
    visited[w] = true;
    bool has_successors = false;
    for (structure_graph::index_type u: w_.successors)
    {
      has_successors = true;
      if (u != v && !find_loop(G, v, u, p, visited))
      {
        visited[w] = false;
        return false;
      }
    }
    if (!has_successors)
    {
      visited[w] = false;
    }
  }
  return false;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_FIND_LOOPS_H
