// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_global_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_GRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_GRAPH_H

#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/pbes/detail/stategraph_influence.h"





namespace mcrl2::pbes_system::detail {

struct stategraph_vertex;

// edge of the control flow graph
struct stategraph_edge
{
  stategraph_vertex* source;
  stategraph_vertex* target;
  std::size_t label;

  stategraph_edge(stategraph_vertex* source_,
                  stategraph_vertex* target_,
                  std::size_t label_
                 )
   : source(source_),
     target(target_),
     label(label_)
   {}

  bool operator<(const stategraph_edge& other) const
  {
    if (source != other.source)
    {
      return source < other.source;
    }
    if (target != other.target)
    {
      return target < other.target;
    }
    return label < other.label;
  }

  std::string print() const;
};

struct stategraph_vertex;
std::ostream& operator<<(std::ostream& out, const stategraph_vertex& u);

// vertex of the control flow graph
struct stategraph_vertex
{
  propositional_variable_instantiation X;
  std::set<stategraph_edge> incoming_edges;
  std::set<stategraph_edge> outgoing_edges;
  mutable std::set<data::variable> m_sig;
  mutable std::set<data::variable> m_marking;    // used in the reset variables procedure
  mutable std::vector<bool> m_marked_parameters; // will be set after computing the marking

  stategraph_vertex(const propositional_variable_instantiation& X_)
    : X(X_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << pbes_system::pp(X);
    out << " edges:";
    for (const stategraph_edge& e: outgoing_edges)
    {
      out << " " << pbes_system::pp(e.target->X);
    }
    out << " sig: " << core::detail::print_set(m_sig);
    return out.str();
  }

  // also print the parameters
  std::string print(const data::variable_list& d_X) const
  {
    std::ostringstream out;
    out << core::pp(X.name());
    out << "(";
    out << data::pp(data::make_assignment_list(d_X, X.parameters()));
    out << ")";
    out << " edges:";
    for (const stategraph_edge& e: outgoing_edges)
    {
      out << " " << e.print();
    }
    out << " sig: " << core::detail::print_set(m_sig);
    return out.str();
  }

  std::set<std::size_t> marking_variable_indices(const stategraph_pbes& p) const
  {
    std::set<std::size_t> result;
    for (const data::variable& v: m_marking)
    {
      // TODO: make this code more efficient
      const stategraph_equation& eqn = *find_equation(p, X.name());
      const std::vector<data::variable>& d = eqn.parameters();
      for (std::vector<data::variable>::const_iterator j = d.begin(); j != d.end(); ++j)
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

  void set_marking(const std::set<data::variable>& marking) const
  {
    m_marking = marking;
  }

  void set_significant_variables(const std::set<data::variable>& sig) const
  {
    m_sig = sig;
  }

  std::string print_marking() const
  {
    std::ostringstream out;
    out << "vertex " << pbes_system::pp(X) << " = " << core::detail::print_set(m_marking);
    return out.str();
  }

  const core::identifier_string& name() const
  {
    return X.name();
  }

  const data::data_expression_list& values() const
  {
    return X.parameters();
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
std::ostream& operator<<(std::ostream& out, const stategraph_vertex& u)
{
  return out << u.X;
}

inline
std::string stategraph_edge::print() const
{
  std::ostringstream out;
  out << "(" << pbes_system::pp(source->X) << ", " << pbes_system::pp(target->X) << ") label = " << label;
  return out.str();
}

struct stategraph_global_graph
{
  // vertices of the control flow graph
  std::map<propositional_variable_instantiation, stategraph_vertex> m_control_vertices;

  // an index for the vertices in the control flow graph with a given name
  std::map<core::identifier_string, std::set<stategraph_vertex*> > m_stategraph_index;

  using vertex_iterator = std::map<propositional_variable_instantiation, stategraph_vertex>::iterator;
  using vertex_const_iterator = std::map<propositional_variable_instantiation, stategraph_vertex>::const_iterator;
  using vertex_type = stategraph_vertex;

  void create_index()
  {
    // create an index for the vertices in the control flow graph with a given name
    for (auto &i: m_control_vertices)
    {
      stategraph_vertex& v = i.second;
      m_stategraph_index[v.X.name()].insert(&v);
    }
  }

  bool has_vertex(const stategraph_vertex* u)
  {
    for (auto& i: m_control_vertices)
    {
      stategraph_vertex& v = i.second;
      if (&v == u)
      {
        return true;
      }
    }
    return false;
  }

  // check internal state
  void self_check()
  {
    for (auto& i: m_control_vertices)
    {
      stategraph_vertex& u = i.second;
      for (const stategraph_edge& e: u.incoming_edges)
      {
        if (!has_vertex(e.source))
        {
          std::cout << "error: source not found!" << std::endl;
        }
        if (!has_vertex(e.target))
        {
          std::cout << "error: target not found!" << std::endl;
        }
      }
      for (const stategraph_edge& e: u.outgoing_edges)
      {
        if (!has_vertex(e.source))
        {
          std::cout << "error: source not found!" << std::endl;
        }
        if (!has_vertex(e.target))
        {
          std::cout << "error: target not found!" << std::endl;
        }
      }
    }
  }

  stategraph_global_graph() = default;

  stategraph_global_graph(const stategraph_global_graph& other)
    : m_control_vertices(other.m_control_vertices)
  {
    // reset the pointers
    for (std::map<propositional_variable_instantiation, stategraph_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      stategraph_vertex& u = i->second;

      std::vector<stategraph_edge> iedges(u.incoming_edges.begin(), u.incoming_edges.end());
      for (stategraph_edge& e: iedges)
      {
        stategraph_vertex& v = *(e.source);
        stategraph_vertex& v_new = m_control_vertices.find(v.X)->second;
        e.source = &v_new;
        stategraph_vertex& w = *(e.target);
        stategraph_vertex& w_new = m_control_vertices.find(w.X)->second;
        e.target = &w_new;
      }
      u.incoming_edges = std::set<stategraph_edge>(iedges.begin(), iedges.end());

      std::vector<stategraph_edge> oedges(u.outgoing_edges.begin(), u.outgoing_edges.end());
      for (stategraph_edge& e: oedges)
      {
        stategraph_vertex& v = *(e.source);
        stategraph_vertex& v_new = m_control_vertices.find(v.X)->second;
        e.source = &v_new;
        stategraph_vertex& w = *(e.target);
        stategraph_vertex& w_new = m_control_vertices.find(w.X)->second;
        e.target = &w_new;
      }
      u.outgoing_edges = std::set<stategraph_edge>(oedges.begin(), oedges.end());
    }
    create_index();
    // self_check();
  }

  // \pre x is not present in m_control_vertices
  vertex_iterator insert_vertex(const propositional_variable_instantiation& x)
  {
    std::pair<vertex_iterator, bool> p = m_control_vertices.insert(std::make_pair(x, stategraph_vertex(x)));
    assert(p.second);
    // self_check();
    return p.first;
  }

  vertex_iterator find(const propositional_variable_instantiation& x)
  {
    return m_control_vertices.find(x);
  }

  vertex_const_iterator find(const propositional_variable_instantiation& x) const
  {
    return m_control_vertices.find(x);
  }

  vertex_iterator begin()
  {
    return m_control_vertices.begin();
  }

  vertex_const_iterator begin() const
  {
    return m_control_vertices.begin();
  }

  vertex_iterator end()
  {
    return m_control_vertices.end();
  }

  vertex_const_iterator end() const
  {
    return m_control_vertices.end();
  }

  const std::set<stategraph_vertex*>& index(const core::identifier_string& X) const
  {
    std::map<core::identifier_string, std::set<stategraph_vertex*> >::const_iterator i = m_stategraph_index.find(X);
    assert(i != m_stategraph_index.end());
    return i->second;
  }

  // Returns true if there is an edge X(e) -- label --> Y(f) in the graph, for some e, f, Y.
  bool has_label(const core::identifier_string& X, std::size_t label) const
  {
    const std::set<stategraph_vertex*>& inst = index(X);
    for (stategraph_vertex* i: inst)
    {
      stategraph_vertex& u = *i;
      std::set<stategraph_edge>& E = u.outgoing_edges;
      for (const stategraph_edge& e: E)
      {
        if (e.label == label)
        {
          return true;
        }
      }
    }
    return false;
  }

  std::string print() const
  {
    std::ostringstream out;
    for (const auto& i: m_control_vertices)
    {
      out << "vertex " << i.second.print() << std::endl;
    }
    return out.str();
  }

  std::string print(const std::map<core::identifier_string, data::variable_list>& variable_map) const
  {
    std::ostringstream out;
    out << "--- control flow graph ---" << std::endl;
    for (const auto& i: m_control_vertices)
    {
      const data::variable_list& v = variable_map.find(i.first.name())->second;
      out << "vertex " << i.second.print(v) << std::endl;
    }
    return out.str();
  }

  std::string print_marking() const
  {
    std::ostringstream out;
    for (const auto& i: m_control_vertices)
    {
      const stategraph_vertex& v = i.second;
      out << v.print_marking() << std::endl;
    }
    return out.str();
  }
};

} // namespace mcrl2::pbes_system::detail



#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_GRAPH_H
