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
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

// Implementation of a structure_graph that supports removal of vertices, by setting
// the enabled attribute to false.
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
      std::size_t rank;
      mutable std::vector<const vertex*> predecessors;
      mutable std::vector<const vertex*> successors;
      mutable bool enabled;

      vertex(const pbes_expression& formula_ = pbes_expression(),
             decoration_type decoration_ = d_none,
             std::size_t rank_ = data::undefined_index(),
             std::vector<const vertex*> pred_ = std::vector<const vertex*>(),
             std::vector<const vertex*> succ_ = std::vector<const vertex*>(),
             bool enabled_ = true
            )
        : formula(formula_),
          decoration(decoration_),
          rank(rank_),
          predecessors(pred_),
          successors(succ_),
          enabled(enabled_)
      {}

      std::vector<pbes_expression> predecessor_formulas() const
      {
        std::vector<pbes_expression> result;
        for (const vertex* u: predecessors)
        {
          if (u->enabled)
          {
            result.push_back(u->formula);
          }
        }
        return result;
      }

      std::vector<pbes_expression> successor_formulas() const
      {
        std::vector<pbes_expression> result;
        for (const vertex* u: successors)
        {
          if (u->enabled)
          {
            result.push_back(u->formula);
          }
        }
        return result;
      }
    };

    typedef std::unordered_set<const vertex*> vertex_set;

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
      throw std::runtime_error("Unknown pbes_expression " + pp(x));
    }

  public:
    // insert the variable corresponding to the equation x = phi; overwrites existing value
    const vertex& insert_variable(const pbes_expression& x, const pbes_expression& psi, std::size_t k)
    {
      vertex& u = m_vertices[x];
      u = vertex(x, get_decoration(psi), k);
      return u;
    }

    // insert the variable x; does not overwrite existing value
    const vertex& insert_variable(const pbes_expression& x)
    {
      auto i = m_vertices.find(x);
      if (i != m_vertices.end())
      {
        return i->second;
      }
      else
      {
        vertex& u = m_vertices[x];
        u = vertex(x);
        return u;
      }
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

    const std::unordered_map<pbes_expression, vertex>& vertex_map() const
    {
      return m_vertices;
    }

    vertex_set vertices() const
    {
      vertex_set result;
      for (const auto& p: m_vertices)
      {
        result.insert(&p.second);
      }
      return result;
    }

    const vertex& get_vertex(const pbes_expression& x) const
    {
      auto i = m_vertices.find(x);
      if (i != m_vertices.end())
      {
        return i->second;
      }
      throw std::runtime_error("vertex " + pp(x) + " not found in structure_graph!");
    }
};

inline
std::ostream& operator<<(std::ostream& out, const structure_graph::decoration_type& decoration)
{
  switch (decoration)
  {
    case structure_graph::d_conjunction : { out << "conjunction"; break; }
    case structure_graph::d_disjunction : { out << "disjunction"; break; }
    case structure_graph::d_true        : { out << "true";        break; }
    case structure_graph::d_false       : { out << "false";       break; }
    default                             : { out << "none";        break; }
  }
  return out;
}

inline
std::ostream& operator<<(std::ostream& out, const structure_graph::vertex& u)
{
  out << "vertex(formula = " << u.formula
      << ", decoration = " << u.decoration
      << ", rank = " << (u.rank == data::undefined_index() ? std::string("undefined") : std::to_string(u.rank))
      << ", predecessors = " << core::detail::print_list(u.predecessor_formulas())
      << ", successors = " << core::detail::print_list(u.successor_formulas())
      << ")";
  return out;
}

inline
std::ostream& operator<<(std::ostream& out, const structure_graph::vertex_set& V)
{
  for (const structure_graph::vertex* v: V)
  {
    out << *v << std::endl;
  }
  return out;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_STRUCTURE_GRAPH_H
