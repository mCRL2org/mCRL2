// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H

#include <algorithm>
#include <sstream>
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Base class for local and global control flow graph vertex
class control_flow_graph_vertex
{
  protected:
    core::identifier_string m_name;
    std::size_t m_index;
    data::variable m_variable;

  public:
    control_flow_graph_vertex(const core::identifier_string& name, std::size_t index, const data::variable& variable)
      : m_name(name), m_index(index), m_variable(variable)
    {}

    const core::identifier_string& name() const
    {
      return m_name;
    }

    std::size_t index() const
    {
      return m_index;
    }

    const data::variable& variable() const
    {
      return m_variable;
    }

    bool has_variable() const
    {
      return m_index != data::undefined_index();
    }
};

inline
std::ostream& operator<<(std::ostream& out, const control_flow_graph_vertex& u)
{
  return out << '(' << u.name() << ", " << u.index() << ", " << data::pp(u.variable()) << ')';
}

// Vertex in the global control flow graph.
class global_control_flow_graph_vertex: public control_flow_graph_vertex
{
  protected:
    std::set<global_control_flow_graph_vertex*> m_neighbors;

  public:
    global_control_flow_graph_vertex(const core::identifier_string& name, std::size_t index, const data::variable& variable)
      : control_flow_graph_vertex(name, index, variable)
    {}

    const std::set<global_control_flow_graph_vertex*>& neighbors() const
    {
      return m_neighbors;
    }

    std::set<global_control_flow_graph_vertex*>& neighbors()
    {
      return m_neighbors;
    }
};

class local_control_flow_graph_vertex;
std::ostream& operator<<(std::ostream&, const local_control_flow_graph_vertex&);

// Vertex in a local control flow graph.
class local_control_flow_graph_vertex: public control_flow_graph_vertex
{
  protected:
    data::data_expression m_value;
    mutable std::set<data::variable> m_marking; // used in the reset variables procedure

    mutable std::map<const local_control_flow_graph_vertex*, std::set<std::size_t> > m_outgoing_edges;
    // the mapped values are the edge labels; note that there can be multiple edges with different labels

    mutable std::map<const local_control_flow_graph_vertex*, std::set<std::size_t> > m_incoming_edges;

    // (i, l) is mapped to FV(rewr(e[l], [d_X[n] := z])) intersect {d | (X, d) in B},
    // where Y(e) = PVI(phi_X, i), d_X[n] = variable(), z = value(), and B is the belongs relation
    // corresponding to the graph of this vertex
    mutable std::map<std::pair<std::size_t, data::variable>, std::set<data::variable> > m_marking_update;

  public:
    local_control_flow_graph_vertex(const core::identifier_string& name, std::size_t index, const data::variable& variable, const data::data_expression& value)
      : control_flow_graph_vertex(name, index, variable), m_value(value)
    {}

    local_control_flow_graph_vertex(const core::identifier_string& name, const data::data_expression& value)
      : control_flow_graph_vertex(name, data::undefined_index(), data::undefined_variable()), m_value(value)
    {}

    const data::data_expression& value() const
    {
      return m_value;
    }

    const std::set<data::variable>& marking() const
    {
      return m_marking;
    }

    void set_marking(const std::set<data::variable>& marking) const
    {
      m_marking = marking;
    }

    void extend_marking(const std::set<data::variable>& marking) const
    {
      m_marking.insert(marking.begin(), marking.end());
    }

    const std::map<const local_control_flow_graph_vertex*, std::set<std::size_t> >& outgoing_edges() const
    {
      return m_outgoing_edges;
    }

    void insert_outgoing_edge(const local_control_flow_graph_vertex* u, std::size_t label) const
    {
      m_outgoing_edges[u].insert(label);
    }

    void insert_incoming_edge(const local_control_flow_graph_vertex* u, std::size_t label) const
    {
      m_incoming_edges[u].insert(label);
    }

    const std::map<const local_control_flow_graph_vertex*, std::set<std::size_t> >& incoming_edges() const
    {
      return m_incoming_edges;
    }

    bool operator==(const local_control_flow_graph_vertex& other) const
    {
      return m_name == other.m_name && m_index == other.m_index && m_value == other.m_value;
    }

    std::string print_marking() const
    {
      std::ostringstream out;
      out << "vertex " << *this << " marking = " << core::detail::print_set(m_marking);
      return out.str();
    }

    std::string print_outgoing_edges() const;

    void set_marking_update(std::size_t i, const data::variable& d, const std::set<data::variable>& V) const
    {
      std::pair<std::size_t, data::variable> p(i, d);
      m_marking_update[p] = V;
    }

    const std::map<std::pair<std::size_t, data::variable>, std::set<data::variable> >& marking_update() const
    {
      return m_marking_update;
    }
};

inline
std::ostream& operator<<(std::ostream& out, const local_control_flow_graph_vertex& u)
{
  if (u.index() == data::undefined_index())
  {
    return out << '(' << u.name() << ", ?, ?=?)";
    assert(u.value() == data::undefined_data_expression());
  }
  return out << '(' << u.name() << ", " << u.index() << ", " << data::pp(u.variable()) << "=" << data::pp(u.value()) << ')';
}

// The implementation is pulled outside the class, since the stream operator for local_control_flow_graph_vertex is needed.
inline
std::string local_control_flow_graph_vertex::print_outgoing_edges() const
{
  std::ostringstream out;
  for (auto i = m_outgoing_edges.begin(); i != m_outgoing_edges.end(); ++i)
  {
    if (i != m_outgoing_edges.begin())
    {
      out << "; ";
    }
    out << *i->first << " (labels = " << core::detail::print_set(i->second) << ")";
  }
  return out.str();
}

inline
bool operator<(const local_control_flow_graph_vertex& u, const local_control_flow_graph_vertex& v)
{
  if (u.name() != v.name())
  {
    return u.name() < v.name();
  }
  if (u.index() != v.index())
  {
    return u.index() < v.index();
  }
  return u.value() < v.value();
}

struct local_control_flow_graph
{
  std::set<local_control_flow_graph_vertex> vertices;

  // an index for the vertices in the control flow graph with a given name
  std::map<core::identifier_string, std::set<const local_control_flow_graph_vertex*> > m_stategraph_index;

  const local_control_flow_graph_vertex& find_vertex(const local_control_flow_graph_vertex& u) const;

  // finds the vertex with given name and index
  const local_control_flow_graph_vertex& find_vertex(const core::identifier_string& X, std::size_t p) const;

  // finds the vertex with given name
  const local_control_flow_graph_vertex& find_vertex(const core::identifier_string& X) const;

  void compute_index()
  {
    m_stategraph_index.clear();

    // create an index for the vertices in the control flow graph with a given name
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      const local_control_flow_graph_vertex& u = *i;
      m_stategraph_index[u.name()].insert(&u);
    }
  }

  const std::set<const local_control_flow_graph_vertex*>& index(const core::identifier_string& X) const
  {
    auto i = m_stategraph_index.find(X);
    assert(i != m_stategraph_index.end());
    return i->second;
  }

  bool has_vertex(const core::identifier_string& X, std::size_t p) const
  {
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      if (i->name() == X && i->index() == p)
      {
        return true;
      }
    }
    return false;
  }

  /// \brief Default constructor
  local_control_flow_graph()
  {}

  /// \brief Copy constructor N.B. The implementation is rather inefficient!
  local_control_flow_graph(const local_control_flow_graph& other)
  {
    // copy the vertices, but not the neighbors
    for (auto i = other.vertices.begin(); i != other.vertices.end(); ++i)
    {
      auto const& u = *i;
      vertices.insert(local_control_flow_graph_vertex(u.name(), u.index(), u.variable(), u.value()));
    }

    // reconstruct the incoming and outgoing edges
    for (auto i = other.vertices.begin(); i != other.vertices.end(); ++i)
    {
      const local_control_flow_graph_vertex& u = find_vertex(*i);
      auto const& outgoing_edges = i->outgoing_edges();
      for (auto j = outgoing_edges.begin(); j != outgoing_edges.end(); ++j)
      {
        const local_control_flow_graph_vertex& v = find_vertex(*(j->first));
        std::set<std::size_t> labels = j->second;
        for (auto k = labels.begin(); k != labels.end(); ++k)
        {
          u.insert_outgoing_edge(&v, *k);
          v.insert_incoming_edge(&u, *k);
        }
      }
    }
    compute_index();
  }

  // throws a runtime_error if an error is found in the internal representation
  void self_check() const
  {
    // check if all targets of outgoing edges are part of the graph
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      auto const& outgoing_edges = i->outgoing_edges();
      for (auto j = outgoing_edges.begin(); j != outgoing_edges.end(); ++j)
      {
        const local_control_flow_graph_vertex& v = *(j->first);
        find_vertex(v);
      }

      auto const& incoming_edges = i->incoming_edges();
      for (auto j = incoming_edges.begin(); j != incoming_edges.end(); ++j)
      {
        const local_control_flow_graph_vertex& v = *(j->first);
        find_vertex(v);
      }
    }

    // check if no two vertices (X, i, v) and (X, i', v') are in the graph with i != i'
    std::map<core::identifier_string, std::set<std::size_t> > m;
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      auto& m_i = m[i->name()];
      m_i.insert(i->index());
      if (m_i.size() > 1)
      {
        auto const& X = i->name();
        std::ostringstream out;
        out << "Illegal state in local control flow graph: vertices";
        for (auto k = m_i.begin(); k != m_i.end(); ++k)
        {
          out <<  " (" << X << ", " << *k << ")";
        }
        out << " encountered";
        throw mcrl2::runtime_error(out.str());
      }
    }
  }

  std::pair<std::set<local_control_flow_graph_vertex>::iterator, bool> insert(const local_control_flow_graph_vertex& u)
  {
    auto result = vertices.insert(u);
    // self_check();
    return result;
  }

  const local_control_flow_graph_vertex& insert_vertex(const local_control_flow_graph_vertex& v_)
  {
    auto j = std::find(vertices.begin(), vertices.end(), v_);
    if (j == vertices.end())
    {
      mCRL2log(log::debug1, "stategraph") << " add vertex v = " << v_ << std::endl;
      auto k = vertices.insert(v_);
      j = k.first;
    }
    return *j;
  }

  void insert_edge(const local_control_flow_graph_vertex& u,
                   std::size_t i,
                   const local_control_flow_graph_vertex& v
                  )
  {
    // add edge (u, v)
    auto q = u.outgoing_edges().find(&v);
    if (u.outgoing_edges().find(&v) == u.outgoing_edges().end() || q->second.find(i) == q->second.end())
    {
      mCRL2log(log::debug1, "stategraph") << " add edge " << u << " -> " << v << std::endl;
      u.insert_outgoing_edge(&v, i);
      v.insert_incoming_edge(&u, i);
    }
    else
    {
      mCRL2log(log::debug1, "stategraph") << " edge already exists!" << std::endl;
    }
  }

  // Inserts an edge between the vertex u and the vertex v = (Y, k1, e1).
  void insert_edge(std::set<const local_control_flow_graph_vertex*>& todo,
                   const stategraph_pbes& p,
                   const local_control_flow_graph_vertex& u,
                   const core::identifier_string& Y,
                   std::size_t k1,
                   const data::data_expression& e1,
                   std::size_t edge_label
                  )
  {
    mCRL2log(log::debug1, "stategraph") << " insert_edge" << std::endl;
    const stategraph_equation& eq_Y = *find_equation(p, Y);
    const data::variable& d1 = (k1 == data::undefined_index() ? data::undefined_variable() : eq_Y.parameters()[k1]);

    std::size_t size = vertices.size();
    const local_control_flow_graph_vertex& v = insert_vertex(local_control_flow_graph_vertex(Y, k1, d1, e1));
    if (vertices.size() != size)
    {
      todo.insert(&v);
    }

    mCRL2log(log::debug1, "stategraph") << " u.outgoing_edges() = " << u.print_outgoing_edges() << std::endl;
    insert_edge(u, edge_label, v);
    // self_check();
  }

  // Returns true if there is an edge X(e) -- label --> Y(f) in the graph, for some e, f, Y.
  bool has_label(const core::identifier_string& X, std::size_t label) const
  {
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      if (i->name() != X)
      {
        continue;
      }
      auto const& outgoing_edges = i->outgoing_edges();
      for (auto j = outgoing_edges.begin(); j != outgoing_edges.end(); ++j)
      {
        if (j->second.find(label) != j->second.end())
        {
          return true;
        }
      }
    }
    return false;
  }

  std::string print_marking() const
  {
    std::ostringstream out;
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      out << i->print_marking() << std::endl;
    }
    return out.str();
  }
};

inline
std::ostream& operator<<(std::ostream& out, const local_control_flow_graph& G)
{
  for (auto i = G.vertices.begin(); i != G.vertices.end(); ++i)
  {
    out << "vertex " << *i << " outgoing_edges: " << i->print_outgoing_edges() << std::endl;
  }
  return out;
}

inline
const local_control_flow_graph_vertex& local_control_flow_graph::find_vertex(const core::identifier_string& X, std::size_t p) const
{
  for (auto i = vertices.begin(); i != vertices.end(); ++i)
  {
    if (i->name() == X && i->index() == p)
    {
      return *i;
    }
  }
  throw mcrl2::runtime_error("local_control_flow_graph::find_vertex: vertex not found!");
}

inline
const local_control_flow_graph_vertex& local_control_flow_graph::find_vertex(const core::identifier_string& X) const
{
  for (auto i = vertices.begin(); i != vertices.end(); ++i)
  {
    if (i->name() == X)
    {
      return *i;
    }
  }
  throw mcrl2::runtime_error("local_control_flow_graph::find_vertex: vertex not found!");
}

inline
const local_control_flow_graph_vertex& local_control_flow_graph::find_vertex(const local_control_flow_graph_vertex& u) const
{
  auto i = vertices.find(u);
  if (i == vertices.end())
  {
    std::cout << "could not find vertex " << u << " in the graph\n" << *this << std::endl;
    throw mcrl2::runtime_error("local_control_flow_graph::find_vertex: vertex not found!");
  }
  return *i;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H
