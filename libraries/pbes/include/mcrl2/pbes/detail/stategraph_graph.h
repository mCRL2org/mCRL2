// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H

#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Vertex in the graph of LCFP parameters
class LCFP_vertex
{
  protected:
    core::identifier_string m_name;
    std::size_t m_index;
    data::variable m_variable;

  public:
    LCFP_vertex(const core::identifier_string& name, std::size_t index = data::undefined_index(), const data::variable& variable = data::undefined_variable())
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
std::ostream& operator<<(std::ostream& out, const LCFP_vertex& u)
{
  out << '(' << u.name() << ", ";
  if (u.index() == data::undefined_index())
  {
    out << "?";
  }
  else
  {
    out << u.index();
  }
  out << ", ";
  if (u.variable() == data::undefined_variable())
  {
    out << "?";
  }
  else
  {
    out << u.variable();
  }
  return out << ')';
}

// Vertex in the graph of GCFP parameters
class GCFP_vertex: public LCFP_vertex
{
  protected:
    std::set<GCFP_vertex*> m_neighbors;

  public:
    GCFP_vertex(const core::identifier_string& name, std::size_t index, const data::variable& variable)
      : LCFP_vertex(name, index, variable)
    {}

    const std::set<GCFP_vertex*>& neighbors() const
    {
      return m_neighbors;
    }

    std::set<GCFP_vertex*>& neighbors()
    {
      return m_neighbors;
    }
};

class GCFP_graph;
std::ostream& operator<<(std::ostream& out, const GCFP_graph& G);

class GCFP_graph
{
  protected:
    std::vector<GCFP_vertex> m_vertices;

  public:
    const std::vector<GCFP_vertex>& vertices() const
    {
      return m_vertices;
    }

    const GCFP_vertex& vertex(std::size_t i) const
    {
      return m_vertices[i];
    }

    void add_vertex(const GCFP_vertex& u)
    {
      m_vertices.push_back(u);
    }

    GCFP_vertex& find_vertex(const core::identifier_string& X, std::size_t n)
    {
      for (GCFP_vertex& u: m_vertices)
      {
        if (u.name() == X && u.index() == n)
        {
          return u;
        }
      }
      std::ostringstream out;
      out << "vertex (" << X << ", " << n << ") not found in GCFP graph";
      throw mcrl2::runtime_error(out.str());
    }

    std::size_t index(const GCFP_vertex& u) const
    {
      return &u - &(m_vertices.front());
    }
};

inline
std::ostream& operator<<(std::ostream& out, const GCFP_graph& G)
{
  for (const auto& v: G.vertices())
  {
    out << v << std::endl;
  }
  return out;
}

// This class is used to add labeled edges to a vertex
template <typename Vertex>
class add_edges
{
  protected:
    mutable std::map<const Vertex*, std::set<std::size_t> > m_outgoing_edges;
    mutable std::map<const Vertex*, std::set<std::size_t> > m_incoming_edges;
    // the mapped values are the edge labels; note that there can be multiple edges with different labels

  public:
    const std::map<const Vertex*, std::set<std::size_t> >& outgoing_edges() const
    {
      return m_outgoing_edges;
    }

    const std::map<const Vertex*, std::set<std::size_t> >& incoming_edges() const
    {
      return m_incoming_edges;
    }

    void insert_outgoing_edge(const Vertex* u, std::size_t label) const
    {
      m_outgoing_edges[u].insert(label);
    }

    void insert_incoming_edge(const Vertex* u, std::size_t label) const
    {
      m_incoming_edges[u].insert(label);
    }

    std::string print_outgoing_edges() const
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

    void remove_edges()
    {
      m_outgoing_edges.clear();
      m_incoming_edges.clear();
    }
};

// Vertex in a local control flow graph.
class local_control_flow_graph_vertex: public LCFP_vertex, public add_edges<local_control_flow_graph_vertex>
{
  protected:
    typedef add_edges<local_control_flow_graph_vertex> super;

    data::data_expression m_value;
    mutable std::set<data::variable> m_marking; // used in the reset variables procedure

    // (i, l) is mapped to FV(rewr(e[l], [d_X[n] := z])) intersect {d | (X, d) in B},
    // where Y(e) = PVI(phi_X, i), d_X[n] = variable(), z = value(), and B is the belongs relation
    // corresponding to the graph of this vertex
    mutable std::map<std::pair<std::size_t, data::variable>, std::set<data::variable> > m_marking_update;

  public:
    using super::incoming_edges;
    using super::outgoing_edges;
    using super::print_outgoing_edges;
    using super::insert_outgoing_edge;
    using super::insert_incoming_edge;
    using super::remove_edges;

    local_control_flow_graph_vertex(const core::identifier_string& name, std::size_t index, const data::variable& variable, const data::data_expression& value)
      : LCFP_vertex(name, index, variable), m_value(value)
    {}

    local_control_flow_graph_vertex(const core::identifier_string& name, const data::data_expression& value)
      : LCFP_vertex(name), m_value(value)
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

template <typename Vertex>
struct control_flow_graph
{
  std::set<Vertex> vertices;

  // an index for the vertices in the control flow graph with a given name
  std::map<core::identifier_string, std::set<const Vertex*> > m_graph_index;

  const Vertex& find_vertex(const Vertex& u) const
  {
    auto i = vertices.find(u);
    if (i == vertices.end())
    {
      std::cout << "could not find vertex " << u << " in the graph\n" << *this << std::endl;
      throw mcrl2::runtime_error("control_flow_graph::find_vertex: vertex not found!");
    }
    return *i;
  }

  void compute_index()
  {
    m_graph_index.clear();

    // create an index for the vertices in the control flow graph with a given name
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
    {
      const Vertex& u = *i;
      m_graph_index[u.name()].insert(&u);
    }
  }

  std::set<const Vertex*> index(const core::identifier_string& X) const
  {
    auto i = m_graph_index.find(X);
    if (i == m_graph_index.end())
    {
      return std::set<const Vertex*>();
    }
    else
    {
      return i->second;
    }
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
  control_flow_graph() = default;

  /// \brief Copy constructor N.B. The implementation is rather inefficient!
  control_flow_graph(const control_flow_graph<Vertex>& other)
  {
    // copy the vertices, but not the neighbors
    for (auto i = other.vertices.begin(); i != other.vertices.end(); ++i)
    {
      auto u = *i;
      u.remove_edges();
      vertices.insert(u);
    }

    // reconstruct the incoming and outgoing edges
    for (auto i = other.vertices.begin(); i != other.vertices.end(); ++i)
    {
      const Vertex& u = find_vertex(*i);
      auto const& outgoing_edges = i->outgoing_edges();
      for (auto j = outgoing_edges.begin(); j != outgoing_edges.end(); ++j)
      {
        const Vertex& v = find_vertex(*(j->first));
        std::set<std::size_t> labels = j->second;
        for (std::size_t label: labels)
        {
          u.insert_outgoing_edge(&v, label);
          v.insert_incoming_edge(&u, label);
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
        const Vertex& v = *(j->first);
        find_vertex(v);
      }

      auto const& incoming_edges = i->incoming_edges();
      for (auto j = incoming_edges.begin(); j != incoming_edges.end(); ++j)
      {
        const Vertex& v = *(j->first);
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
        out << "Illegal state in control flow graph::vertices";
        for (auto k = m_i.begin(); k != m_i.end(); ++k)
        {
          out <<  " (" << X << ", " << *k << ")";
        }
        out << " encountered";
        throw mcrl2::runtime_error(out.str());
      }
    }
  }

  std::pair<typename std::set<Vertex>::iterator, bool> insert(const Vertex& u)
  {
    auto result = vertices.insert(u);
    // self_check();
    return result;
  }

  const Vertex& insert_vertex(const Vertex& v_)
  {
    auto j = std::find(vertices.begin(), vertices.end(), v_);
    if (j == vertices.end())
    {
      mCRL2log(log::debug1) << " add vertex v = " << v_ << std::endl;
      auto k = vertices.insert(v_);
      j = k.first;
    }
    return *j;
  }

  void insert_edge(const Vertex& u,
                   std::size_t i,
                   const Vertex& v
                  )
  {
    // add edge (u, v)
    auto q = u.outgoing_edges().find(&v);
    if (u.outgoing_edges().find(&v) == u.outgoing_edges().end() || q->second.find(i) == q->second.end())
    {
      mCRL2log(log::debug1) << " add edge " << u << " -> " << v << std::endl;
      u.insert_outgoing_edge(&v, i);
      v.insert_incoming_edge(&u, i);
    }
    else
    {
      mCRL2log(log::debug1) << " edge already exists!" << std::endl;
    }
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

  typename std::set<Vertex>::const_iterator begin() const
  {
    return vertices.begin();
  }

  typename std::set<Vertex>::const_iterator end() const
  {
    return vertices.end();
  }
};

template <typename Vertex>
std::ostream& operator<<(std::ostream& out, const control_flow_graph<Vertex>& G)
{
  for (auto i = G.vertices.begin(); i != G.vertices.end(); ++i)
  {
    out << *i << " outgoing_edges: " << i->print_outgoing_edges() << std::endl;
  }
  return out;
}

struct local_control_flow_graph: public control_flow_graph<local_control_flow_graph_vertex>
{
  typedef control_flow_graph<local_control_flow_graph_vertex> super;

  using super::find_vertex;
  using super::has_label;
  using super::insert_edge;
  using super::insert_vertex;

  /// \brief Default constructor
  local_control_flow_graph() = default;

  /// \brief Copy constructor N.B. The implementation is rather inefficient!
  local_control_flow_graph(const local_control_flow_graph& other) = default;

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
    mCRL2log(log::debug1) << " insert_edge" << std::endl;
    const stategraph_equation& eq_Y = *find_equation(p, Y);
    const data::variable& d1 = (k1 == data::undefined_index() ? data::undefined_variable() : eq_Y.parameters()[k1]);

    std::size_t size = vertices.size();
    const local_control_flow_graph_vertex& v = insert_vertex(local_control_flow_graph_vertex(Y, k1, d1, e1));
    if (vertices.size() != size)
    {
      todo.insert(&v);
    }

    mCRL2log(log::debug1) << " u.outgoing_edges() = " << u.print_outgoing_edges() << std::endl;
    insert_edge(u, edge_label, v);
    // self_check();
  }

  std::string print_marking() const
  {
    std::ostringstream out;
    for (const auto& v: vertices)
    {
      out << v.print_marking() << std::endl;
    }
    return out.str();
  }

  // finds the vertex with given name and index
  const local_control_flow_graph_vertex& find_vertex(const core::identifier_string& X, std::size_t p) const
  {
    for (const auto& v: vertices)
    {
      if (v.name() == X && v.index() == p)
      {
        return v;
      }
    }
    throw mcrl2::runtime_error("stategraph_global_graph::find_vertex: vertex not found!");
  }

  // finds the vertex with given name
  const local_control_flow_graph_vertex& find_vertex(const core::identifier_string& X) const
  {
    for (const auto& v: vertices)
    {
      if (v.name() == X)
      {
        return v;
      }
    }
    throw mcrl2::runtime_error("stategraph_global_graph::find_vertex: vertex not found!");
  }
};

class global_control_flow_graph_vertex;
std::ostream& operator<<(std::ostream& out, const global_control_flow_graph_vertex&);

// Vertex in the global control flow graph.
class global_control_flow_graph_vertex: public add_edges<global_control_flow_graph_vertex>
{
  protected:
    typedef add_edges<global_control_flow_graph_vertex> super;
    core::identifier_string m_name;
    data::data_expression_list m_values;
    mutable std::set<data::variable> m_sig;
    mutable std::set<data::variable> m_marking;    // used in the reset variables procedure
    mutable std::vector<bool> m_marked_parameters; // will be set after computing the marking

  public:
    using super::incoming_edges;
    using super::outgoing_edges;
    using super::print_outgoing_edges;
    using super::insert_outgoing_edge;
    using super::insert_incoming_edge;
    using super::remove_edges;

    global_control_flow_graph_vertex(const core::identifier_string& name, const data::data_expression_list& values)
      : m_name(name), m_values(values)
    {}

    const core::identifier_string& name() const
    {
      return m_name;
    }

    const data::data_expression_list& values() const
    {
      return m_values;
    }

    void set_marking(const std::set<data::variable>& marking) const
    {
      m_marking = marking;
    }

    void extend_marking(const std::set<data::variable>& marking) const
    {
      m_marking.insert(marking.begin(), marking.end());
    }

    void set_significant_variables(const std::set<data::variable>& sig) const
    {
      m_sig = sig;
    }

    bool operator==(const global_control_flow_graph_vertex& other) const
    {
      return m_name == other.m_name && m_values == other.m_values;
    }

    std::string print() const
    {
      return print_outgoing_edges();
      // std::ostringstream out;
      // out << pbes_system::pp(X);
      // out << " edges:";
      // for (auto i = outgoing_edges.begin(); i != outgoing_edges.end(); ++i)
      // {
      //   out << " " << pbes_system::pp(i->target->X);
      // }
      // out << " sig: " << core::detail::print_set(sig);
      // return out.str();
    }

    // also print the parameters
    std::string print(const data::variable_list&) const
    {
      return print_outgoing_edges();
      // std::ostringstream out;
      // out << core::pp(X.name());
      // out << "(";
      // out << data::pp(data::make_assignment_list(d_X, X.parameters()));
      // out << ")";
      // out << " edges:";
      // for (auto i = outgoing_edges.begin(); i != outgoing_edges.end(); ++i)
      // {
      //   out << " " << i->print();
      // }
      // out << " sig: " << core::detail::print_set(sig);
      // return out.str();
    }

    std::set<std::size_t> marking_variable_indices(const stategraph_pbes& p) const
    {
      std::set<std::size_t> result;
      for (const data::variable& v: marking())
      {
        // TODO: make this code more efficient
        const stategraph_equation& eqn = *find_equation(p, m_name);
        const std::vector<data::variable>& d = eqn.parameters();
        for (auto j = d.begin(); j != d.end(); ++j)
        {
          if (v == *j)
          {
            result.insert(j - d.begin());
            break;
          }
        }
      }
      return result;
    }

    // returns true if the i-th parameter of X is marked
    bool is_marked_parameter(std::size_t i) const
    {
      return m_marked_parameters[i];
    }

    void add_marked_parameter(bool b) const
    {
      m_marked_parameters.push_back(b);
    }

    std::string print_marking() const
    {
      std::ostringstream out;
      out << "vertex " << *this << " = " << core::detail::print_set(m_marking);
      return out.str();
    }

    const std::set<data::variable>& sig() const
    {
      return m_sig;
    }

    const std::set<data::variable>& marking() const
    {
      return m_marking;
    }

    const std::vector<bool>& marked_parameters() const
    {
      return m_marked_parameters;
    }
};

inline
bool operator<(const global_control_flow_graph_vertex& x, const global_control_flow_graph_vertex& y)
{
  return x.name() < y.name() || (x.name() == y.name() && x.values() < y.values());
}

std::ostream& operator<<(std::ostream& out, const global_control_flow_graph_vertex& u)
{
  return out << u.name() << core::detail::print_container(u.values(), "(", ")", "", false, false);
}

struct global_control_flow_graph: public control_flow_graph<global_control_flow_graph_vertex>
{
  typedef control_flow_graph<global_control_flow_graph_vertex> super;
  typedef global_control_flow_graph_vertex vertex_type;

  /// \brief Default constructor
  global_control_flow_graph() = default;

  /// \brief Copy constructor N.B. The implementation is rather inefficient!
  global_control_flow_graph(const global_control_flow_graph& other) = default;

  std::string print_marking() const
  {
    std::ostringstream out;
    for (const auto& v: vertices)
    {
      out << v.print_marking() << std::endl;
    }
    return out.str();
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H
