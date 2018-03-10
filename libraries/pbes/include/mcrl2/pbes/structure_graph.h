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

#include <boost/dynamic_bitset.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

//--- workaround for missing dynamic_bitset::all() function in boost versions prior to 1.56 ---//
template <typename T>
auto call_dynamic_bitset_all_helper(T t, int) -> decltype(t.all())
{
  return t.all();
}

// inefficient alternative for all()
template <typename T>
auto call_dynamic_bitset_all_helper(T t, long) -> int
{
  return ~t.none();
}

template <typename T>
int call_dynamic_bitset_all(const T& t)
{
  return call_dynamic_bitset_all_helper(t, 0);
}

struct structure_graph_builder;

} // namespace detail

class structure_graph
{
  friend struct detail::structure_graph_builder;

  public:
    enum decoration_type
    {
      d_disjunction = 0,
      d_conjunction = 1,
      d_true,
      d_false,
      d_none
    };

    struct vertex
    {
      pbes_expression formula;
      decoration_type decoration;
      std::size_t rank;
      std::vector<int> predecessors;
      std::vector<int> successors;
      mutable int strategy;

      vertex(const pbes_expression& formula_,
             decoration_type decoration_ = structure_graph::d_none,
             std::size_t rank_ = data::undefined_index(),
             std::vector<int> pred_ = std::vector<int>(),
             std::vector<int> succ_ = std::vector<int>(),
             int strategy_ = -1
            )
        : formula(formula_),
          decoration(decoration_),
          rank(rank_),
          predecessors(pred_),
          successors(succ_),
          strategy(strategy_)
      {}
    };

  protected:
    std::vector<vertex> m_vertices;
    int m_initial_vertex;
    boost::dynamic_bitset<> m_exclude;

    struct integers_not_contained_in
    {
      const boost::dynamic_bitset<>& subset;

      integers_not_contained_in(const boost::dynamic_bitset<>& subset_)
        : subset(subset_)
      {}

      bool operator()(int i) const
      {
        return !subset[i];
      }
    };

    struct vertices_not_contained_in
    {
      const std::vector<vertex>& vertices;
      const boost::dynamic_bitset<>& subset;

      vertices_not_contained_in(const std::vector<vertex>& vertices_, const boost::dynamic_bitset<>& subset_)
        : vertices(vertices_),
          subset(subset_)
      {}

      bool operator()(const vertex& v) const
      {
        auto i = &v - &(vertices.front());
        return !subset[i];
      }
    };

  public:
    structure_graph() = default;

    int initial_vertex() const
    {
      return m_initial_vertex;
    }

    decoration_type decoration(int u) const
    {
      return m_vertices[u].decoration;
    }

    std::size_t rank(int u) const
    {
      return m_vertices[u].rank;
    }

    const std::vector<vertex>& all_vertices() const
    {
      return m_vertices;
    }

    const std::vector<int>& all_predecessors(int u) const
    {
      return m_vertices[u].predecessors;
    }

    const std::vector<int>& all_successors(int u) const
    {
      return m_vertices[u].successors;
    }

    boost::filtered_range<vertices_not_contained_in, const std::vector<vertex>> vertices() const
    {
      return all_vertices() | boost::adaptors::filtered(vertices_not_contained_in(m_vertices, m_exclude));
    }

    boost::filtered_range<integers_not_contained_in, const std::vector<int>> predecessors(int u) const
    {
      return all_predecessors(u) | boost::adaptors::filtered(integers_not_contained_in(m_exclude));
    }

    boost::filtered_range<integers_not_contained_in, const std::vector<int>> successors(int u) const
    {
      return all_successors(u) | boost::adaptors::filtered(integers_not_contained_in(m_exclude));
    }

    int strategy(int u) const
    {
      return m_vertices[u].strategy;
    }

    const vertex& find_vertex(int u) const
    {
      return m_vertices[u];
    }

    const boost::dynamic_bitset<>& exclude() const
    {
      return m_exclude;
    }

    boost::dynamic_bitset<>& exclude()
    {
      return m_exclude;
    }

    bool contains(int u) const
    {
      return !m_exclude[u];
    }

    // TODO: avoid this linear time check
    bool is_empty() const
    {
      // This requires boost 1.56
      // return m_exclude.all();
      return detail::call_dynamic_bitset_all(m_exclude);
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
      << ", predecessors = " << core::detail::print_list(u.predecessors)
      << ", successors = " << core::detail::print_list(u.successors)
      << ", strategy = " << u.strategy
      << ")";
  return out;
}

// inline
// std::ostream& operator<<(std::ostream& out, const structure_graph::vertex_set& V)
// {
//   for (const structure_graph::vertex* v: V)
//   {
//     if (v->enabled)
//     {
//       out << *v << std::endl;
//     }
//   }
//   return out;
// }
//
// inline
// std::string pp(const structure_graph& G)
// {
//   std::ostringstream out;
//   for (const structure_graph::vertex* v: G.vertices())
//   {
//     out << *v << std::endl;
//   }
//   return out.str();
// }

namespace detail {

struct structure_graph_builder
{
  structure_graph& m_graph;
  std::vector<structure_graph::vertex> m_vertices;
  std::unordered_map<pbes_expression, int> m_vertex_map;
  pbes_expression m_initial_state; // The initial state.

  structure_graph_builder(structure_graph& G)
    : m_graph(G)
  {}

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

  int create_vertex(const pbes_expression& x)
  {
    assert(m_vertex_map.find(x) == m_vertex_map.end());
    m_vertices.emplace_back(x, decoration(x));
    int index = m_vertices.size() - 1;
    m_vertex_map.insert({ x, index });
    return index;
  }

  // insert the variable corresponding to the equation x = phi; overwrites existing value, but leaves pred/succ intact
  int insert_variable(const pbes_expression& x, const pbes_expression& psi, std::size_t k)
  {
    auto i = m_vertex_map.find(x);
    int ui = i == m_vertex_map.end() ? create_vertex(x) : i->second;
    auto& u = m_vertices[ui];
    u.decoration = decoration(psi);
    u.rank = k;
    return ui;
  }

  // insert the variable x; does not overwrite existing value
  int insert_variable(const pbes_expression& x)
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

  int insert_vertex(const pbes_expression& x)
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

  void insert_edge(int ui, int vi)
  {
    auto& u = m_vertices[ui];
    auto& v = m_vertices[vi];
    u.successors.push_back(vi);
    v.predecessors.push_back(ui);
  }

  void set_initial_state(const propositional_variable_instantiation& x)
  {
    m_initial_state = x;
  }

  // call at the end, to put the results into m_graph
  void finalize()
  {
    std::size_t N = m_vertices.size();

    std::swap(m_graph.m_vertices, m_vertices);

    auto i = m_vertex_map.find(m_initial_state);
    assert (i != m_vertex_map.end());
    m_graph.m_initial_vertex = i->second;

    m_graph.m_exclude = boost::dynamic_bitset<>(N);
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_STRUCTURE_GRAPH_H
