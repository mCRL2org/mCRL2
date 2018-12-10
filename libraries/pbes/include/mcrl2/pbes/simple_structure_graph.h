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

#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

// A structure graph
class simple_structure_graph
{
  public:
    typedef structure_graph::decoration_type decoration_type;
    typedef structure_graph::index_type index_type;
    typedef structure_graph::vertex vertex;

  protected:
    const std::vector<vertex>& m_vertices;

  public:
    explicit simple_structure_graph(const std::vector<vertex>& vertices)
      : m_vertices(vertices)
    {}

    decoration_type decoration(index_type u) const
    {
      return m_vertices[u].decoration;
    }

    std::size_t extent() const
    {
      return m_vertices.size();
    }

    std::size_t rank(index_type u) const
    {
      return m_vertices[u].rank;
    }

    const std::vector<vertex>& vertices() const
    {
      return m_vertices;
    }

    const std::vector<vertex>& all_vertices() const
    {
      return m_vertices;
    }

    const std::vector<index_type>& predecessors(index_type u) const
    {
      return m_vertices[u].predecessors;
    }

    const std::vector<index_type>& successors(index_type u) const
    {
      return m_vertices[u].successors;
    }

    index_type strategy(index_type u) const
    {
      return m_vertices[u].strategy;
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SIMPLE_STRUCTURE_GRAPH_H
