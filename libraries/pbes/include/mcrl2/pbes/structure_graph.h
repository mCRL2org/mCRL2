// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/structure_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_STRUCTURE_GRAPH_H
#define MCRL2_PBES_STRUCTURE_GRAPH_H

#include <cassert>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/data/undefined.h"

namespace mcrl2 {

namespace pbes_system {

class structure_graph
{
  public:
    enum decoration_type
    {
      d_conjunction, d_disjunction, d_true, d_false, d_none
    };

    struct vertex
    {
      pbes_expression formula;
      decoration_type decoration;
      mutable std::vector<const vertex*> predecessors;
      mutable std::vector<const vertex*> successors;
      std::size_t rank;
      bool enabled;

      vertex(const pbes_expression& formula_,
             decoration_type decoration_ = d_none,
             std::vector<const vertex*> pred_ = std::vector<const vertex*>(),
             std::vector<const vertex*> succ_ = std::vector<const vertex*>(),
             std::size_t rank_ = data::undefined_index(),
             bool enabled_ = false
            )
        : formula(formula_),
          decoration(decoration_),
          predecessors(pred_),
          successors(succ_),
          rank(rank_),
          enabled(enabled_)
      {}
    };

  protected:
    std::unordered_map<pbes_expression, vertex> m_vertices;

    decoration_type get_decoration(const pbes_expression& x) const
    {
      if (is_true(x))
      {
        return d_true;
      }
      else if (is_false(x))
      {
        return d_false;
      }
      else if (is_propositional_variable_instantiation(x))
      {
        return d_none;
      }
      else if (is_and(x))
      {
        return d_conjunction;
      }
      else if (is_or(x))
      {
        return d_disjunction;
      }
    }

  public:
    const vertex& insert_variable(const pbes_expression& x, std::size_t rank = data::undefined_index())
    {
      auto q = m_vertices.insert({ x, vertex(x) });
      assert(q.second);
      return q.first->second;
    }

    const vertex& insert_vertex(const pbes_expression& x)
    {
      // if the vertex already exists, return it
      auto i = m_vertices.find(x);
      if (i != m_vertices.end())
      {
        return i->second;
      }

      // create a new vertex, and insert it
      auto q = m_vertices.insert({ x, vertex(x, get_decoration(x)) });
      assert(q.second);
      return q.first->second;
    }

    void insert_edge(const vertex& u, const vertex& v)
    {
      u.successors.push_back(&v);
      v.predecessors.push_back(&u);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_STRUCTURE_GRAPH_H
