// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_dependency_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_DEPENDENCY_GRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_DEPENDENCY_GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/sequence.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct dependency_vertex
{
  core::identifier_string X;
  std::size_t p;

  dependency_vertex(const core::identifier_string& X_, std::size_t p_)
    : X(X_), p(p_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "(" << std::string(X) << ", " << p << ")";
    return out.str();
  }

  bool operator<(const dependency_vertex& other)
  {
    if (X != other.X)
    {
      return X < other.X;
    }
    return (p < other.p);
  }
};

struct dependency_graph
{
  typedef std::map<std::size_t, dependency_vertex*> vertex_map;
  typedef std::map<core::identifier_string, const dependency_vertex*> constraint_map;

  std::vector<dependency_vertex> m_vertices;

  // needs to be initialized after inserting vertices!
  std::map<core::identifier_string, vertex_map> m_index;

  void set_index()
  {
    for (std::vector<dependency_vertex>::iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      m_index[i->X][i->p] = &(*i);
    }
  }

  dependency_graph()
  {}

  dependency_graph(const dependency_graph& other)
    : m_vertices(other.m_vertices)
  {
  }

  // @pre: (X, p) is in the graph
  dependency_vertex& find_vertex(const core::identifier_string& X, std::size_t p)
  {
    if (m_index.find(X) == m_index.end())
    {
      std::cerr << "error: entry " << std::string(X) << " not found in the graph!" << std::endl;
      assert(false);
    }
    vertex_map& m = m_index[X];
    if (m.find(p) == m.end())
    {
      std::cerr << "vertex not found: (X, p) = (" << std::string(X) << ", " << p << ")" << std::endl;
      assert(false);
    }
    return *(m_index[X][p]);
  }

  // @pre: (X, p) is not in the graph
  void insert_vertex(const core::identifier_string& X, std::size_t p)
  {
    mCRL2log(log::debug1, "stategraph") << "insert vertex (" << std::string(X) << ", " << p << ")" << std::endl;
    m_vertices.push_back(dependency_vertex(X, p));
  }

  std::string print()
  {
    std::ostringstream out;
    for (std::vector<dependency_vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      const dependency_vertex& u = *i;
      out << u.print() << " edges = ";
      for (std::set<dependency_vertex*>::const_iterator j = u.outgoing_edges.begin(); j != u.outgoing_edges.end(); ++j)
      {
        const dependency_vertex& v = **j;
        out << " (" << u.print() << ", " << v.print() << ")";
      }
      out << std::endl;
    }
    return out.str();
  }

  const std::vector<dependency_vertex>& vertices() const
  {
    return m_vertices;
  }

  std::vector<dependency_vertex>& vertices()
  {
    return m_vertices;
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_DEPENDENCY_GRAPH_H
