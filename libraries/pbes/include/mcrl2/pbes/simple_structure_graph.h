// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/simple_structure_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SIMPLE_STRUCTURE_GRAPH_H
#define MCRL2_PBES_SIMPLE_STRUCTURE_GRAPH_H

#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/pbes/structure_graph.h"



namespace mcrl2::pbes_system {

// A structure graph
class simple_structure_graph
{
  public:
    using decoration_type = structure_graph::decoration_type;
    using index_type = structure_graph::index_type;
    using vertex = structure_graph::vertex;

  protected:
    const structure_graph::vertex_vector& m_vertices;

  public:
    explicit simple_structure_graph(const structure_graph::vertex_vector& vertices)
      : m_vertices(vertices)
    {}

    decoration_type decoration(index_type u) const
    {
      return find_vertex(u).decoration;
    }

    std::size_t extent() const
    {
      return m_vertices.size();
    }

    std::size_t rank(index_type u) const
    {
      return find_vertex(u).rank;
    }

    const structure_graph::vertex_vector& vertices() const
    {
      return m_vertices;
    }

    const structure_graph::vertex_vector& all_vertices() const
    {
      return m_vertices;
    }

    const std::vector<index_type>& predecessors(index_type u) const
    {
      return find_vertex(u).predecessors;
    }

    const std::vector<index_type>& successors(index_type u) const
    {
      return find_vertex(u).successors;
    }

    index_type strategy(index_type u) const
    {
      return find_vertex(u).strategy;
    }

    const vertex& find_vertex(index_type u)
    {
      return m_vertices[u];
    }

    const vertex& find_vertex(index_type u) const
    {
      return m_vertices[u];
    }

    bool contains(index_type /* u */) const
    {
      return true;
    }

    bool is_empty() const
    {
      return m_vertices.empty();
    }

    std::size_t size() const
    {
      return m_vertices.size();
    }
};

inline
std::ostream& operator<<(std::ostream& out, const simple_structure_graph& G)
{
  return print_structure_graph(out, G);
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_SIMPLE_STRUCTURE_GRAPH_H
