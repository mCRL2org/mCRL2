// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/structure_graph_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_STRUCTURE_GRAPH_BUILDER_H
#define MCRL2_PBES_STRUCTURE_GRAPH_BUILDER_H

#include <mcrl2/atermpp/standard_containers/unordered_map.h>
#include "mcrl2/pbes/pbessolve_vertex_set.h"

namespace mcrl2::pbes_system::detail {

struct structure_graph_builder
{
  using index_type = structure_graph::index_type;

  structure_graph& m_graph;
  atermpp::utilities::unordered_map<pbes_expression,
      index_type,
      std::hash<atermpp::detail::reference_aterm<pbes_expression>>,
      std::equal_to<>,
      std::allocator<std::pair<const atermpp::detail::reference_aterm<pbes_expression>,
          atermpp::detail::reference_aterm<index_type>>>,
      true>
      m_vertex_map;
  pbes_expression m_initial_state; // The initial state.

  explicit structure_graph_builder(structure_graph& G)
    : m_graph(G), m_initial_state(data::undefined_data_expression())
  {}

  std::size_t extent() const
  {
    return m_graph.extent();
  }

  structure_graph::vertex_vector& vertices()
  {
    return m_graph.m_vertices;
  }

  const structure_graph::vertex_vector& vertices() const
  {
    return m_graph.m_vertices;
  }

  structure_graph::vertex& vertex(index_type u)
  {
    return m_graph.m_vertices[u];
  }

  const structure_graph::vertex& vertex(index_type u) const
  {
    return m_graph.m_vertices[u];
  }

  structure_graph::decoration_type decoration(const pbes_expression& x) const
  {
    if (is_true(x))
    {
      return structure_graph::d_true;
    }
    else if (is_false(x))
    {
      return structure_graph::d_false;
    }
    else if (is_propositional_variable_instantiation(x))
    {
      return structure_graph::d_none;
    }
    else if (is_and(x))
    {
      return structure_graph::d_conjunction;
    }
    else if (is_or(x))
    {
      return structure_graph::d_disjunction;
    }
    throw std::runtime_error("structure_graph_builder: encountered unsupported pbes_expression " + pp(x));
  }

  index_type create_vertex(const pbes_expression& x)
  {
    assert(m_vertex_map.find(x) == m_vertex_map.end());

    vertices().emplace_back(x, decoration(x));
    index_type index = vertices().size() - 1;
    m_vertex_map.insert({ x, index });
    return index;
  }

  // insert the variable corresponding to the equation x = phi; overwrites existing value, but leaves pred/succ intact
  index_type insert_variable(const pbes_expression& x, const pbes_expression& psi, std::size_t k)
  {
    auto i = m_vertex_map.find(x);
    index_type ui = i == m_vertex_map.end() ? create_vertex(x) : static_cast<unsigned int>(i->second);
    auto& u = vertex(ui);
    u.decoration = decoration(psi);
    u.rank = k;
    return ui;
  }

  // insert the variable x; does not overwrite existing value
  index_type insert_variable(const pbes_expression& x)
  {
    auto i = m_vertex_map.find(x);
    if (i != m_vertex_map.end())
    {
      return i->second;
    }
    else
    {
      return create_vertex(x);
    }
  }

  index_type insert_vertex(const pbes_expression& x)
  {
    // if the vertex already exists, return it
    auto i = m_vertex_map.find(x);
    if (i != m_vertex_map.end())
    {
      return i->second;
    }

    // create a new vertex, and return it
    return create_vertex(x);
  }

  void insert_edge(index_type ui, index_type vi)
  {
    using utilities::detail::contains;
    auto& u = vertex(ui);
    auto& v = vertex(vi);
    if (!contains(u.successors, vi))
    {
      u.successors.push_back(vi);
      v.predecessors.push_back(ui);
    }
  }

  void set_initial_state(const propositional_variable_instantiation& x)
  {
    m_initial_state = x;
  }

  structure_graph::index_type initial_vertex() const
  {
    auto i = m_vertex_map.find(m_initial_state);
    assert (i != m_vertex_map.end());
    return i->second;
  }

  // call at the end, to put the results into m_graph
  void finalize()
  {
    m_graph.m_initial_vertex = initial_vertex();
    m_graph.m_exclude = boost::dynamic_bitset<>(m_graph.extent());
  }

  index_type find_vertex(const pbes_expression& x) const
  {
    auto i = m_vertex_map.find(x);
    if (i == m_vertex_map.end())
    {
      return undefined_vertex();
    }
    return i->second;
  }

  // Erases all vertices in the set U.
  void erase_vertices(const vertex_set& U)
  {
    // mCRL2log(log::debug) << "erasing nodes " << U << std::endl;

    using utilities::detail::contains;

    // compute new index for the vertices
    std::vector<index_type> index;
    structure_graph::index_type count = 0;
    for (index_type u = 0; u != vertices().size(); u++)
    {
      index.push_back(U.contains(u) ? undefined_vertex() : count++);
    }

    // computes new predecessors / successors
    auto update = [&](const std::vector<index_type>& V) {
      std::vector<index_type> result;
      for (auto v: V)
      {
        if (index[v] != undefined_vertex())
        {
          result.push_back(index[v]);
        }
      }
      return result;
    };

    for (index_type u = 0; u != vertices().size(); u++)
    {
      if (index[u] != undefined_vertex())
      {
        structure_graph::vertex& u_ = vertex(u);
        u_.predecessors = update(u_.predecessors);
        u_.successors = update(u_.successors);
        if (u_.strategy != undefined_vertex())
        {
          u_.strategy = index[u_.strategy];
        }
        if (index[u] != u)
        {
          std::swap(vertex(u), vertex(index[u]));
        }
      }
    }

    vertices().erase(vertices().begin() + vertices().size() - U.size(), vertices().end());

    // Recreate the index
    m_vertex_map.clear();
    for (std::size_t i = 0; i < vertices().size(); i++)
    {
      m_vertex_map.insert({vertex(i).formula(), i});
    }
  }
};

struct manual_structure_graph_builder
{
  using index_type = structure_graph::index_type;

  structure_graph& m_graph;
  structure_graph::vertex_vector m_vertices;
  index_type m_initial_state = 0U; // The initial state.

  explicit manual_structure_graph_builder(structure_graph& G)
    : m_graph(G)
  {}

  /// \brief Create a vertex, returns the index of the new vertex
  index_type insert_vertex(bool is_conjunctive, std::size_t rank)
  {
    m_vertices.emplace_back(pbes_expression(), is_conjunctive ? structure_graph::d_conjunction : structure_graph::d_disjunction, rank);
    return m_vertices.size() - 1;
  }

  void insert_edge(index_type ui, index_type vi)
  {
    using utilities::detail::contains;
    structure_graph::vertex& u = m_vertices[ui];
    structure_graph::vertex& v = m_vertices[vi];
    if (!contains(u.successors, vi))
    {
      u.successors.push_back(vi);
      v.predecessors.push_back(ui);
    }
  }

  void remove_edge(index_type ui, index_type vi)
  {
    structure_graph::vertex& u = m_vertices[ui];
    structure_graph::vertex& v = m_vertices[vi];
    u.remove_successor(vi);
    v.remove_predecessor(ui);
  }

  void set_initial_state(const index_type i)
  {
    m_initial_state = i;
  }

  /// \brief call at the end, to put the results into m_graph
  /// \details May be called more than once. Does not invalidate this builder.
  void finalize()
  {
    m_graph.m_vertices = m_vertices;
    m_graph.m_initial_vertex = m_initial_state;

    std::size_t N = m_vertices.size();
    m_graph.m_exclude = boost::dynamic_bitset<>(N);
  }
};

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_STRUCTURE_GRAPH_BUILDER_H
