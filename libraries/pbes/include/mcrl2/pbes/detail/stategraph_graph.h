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
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/detail/stategraph_influence.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
std::string print_variable_set(const std::set<data::variable>& v)
{
  std::ostringstream out;
  out << "{";
  for (std::set<data::variable>::const_iterator j = v.begin(); j != v.end(); ++j)
  {
    if (j != v.begin())
    {
      out << ", ";
    }
    out << data::pp(*j);
  }
  out << "}";
  return out.str();
}

struct stategraph_vertex;

// edge of the control flow graph
struct stategraph_edge
{
  stategraph_vertex* source;
  stategraph_vertex* target;
  propositional_variable_instantiation label;

  stategraph_edge(stategraph_vertex* source_,
                  stategraph_vertex* target_,
                  const propositional_variable_instantiation& label_
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

// vertex of the control flow graph
struct stategraph_vertex
{
  propositional_variable_instantiation X;
  std::set<stategraph_edge> incoming_edges;
  std::set<stategraph_edge> outgoing_edges;
  std::set<data::variable> sig;
  std::set<data::variable> marking;    // used in the reset variables procedure
  std::vector<bool> marked_parameters; // will be set after computing the marking

  stategraph_vertex(const propositional_variable_instantiation& X_)
    : X(X_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << pbes_system::pp(X);
    out << " edges:";
    for (std::set<stategraph_edge>::const_iterator i = outgoing_edges.begin(); i != outgoing_edges.end(); ++i)
    {
      out << " " << pbes_system::pp(i->target->X);
    }
    out << " sig: " << print_variable_set(sig);
    return out.str();
  }

  std::set<std::size_t> marking_variable_indices(const stategraph_pbes& p) const
  {
    std::set<std::size_t> result;
    for (std::set<data::variable>::const_iterator i = marking.begin(); i != marking.end(); ++i)
    {
      // TODO: make this code more efficient
      const stategraph_equation& eqn = *find_equation(p, X.name());
      const std::vector<data::variable>& d = eqn.parameters();
      for (std::vector<data::variable>::const_iterator j = d.begin(); j != d.end(); ++j)
      {
        if (*i == *j)
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
    return marked_parameters[i];
  }

  std::string print_marking() const
  {
    std::ostringstream out;
    out << "vertex " << pbes_system::pp(X) << " = " << print_variable_set(marking);
    return out.str();
  }

};

inline
std::string stategraph_edge::print() const
{
  std::ostringstream out;
  out << "(" << pbes_system::pp(source->X) << ", " << pbes_system::pp(target->X) << ") label = " << pbes_system::pp(label);
  return out.str();
}

struct control_flow_graph
{
  // vertices of the control flow graph
  std::map<propositional_variable_instantiation, stategraph_vertex> m_control_vertices;

  // an index for the vertices in the control flow graph with a given name
  std::map<core::identifier_string, std::set<stategraph_vertex*> > m_stategraph_index;

  typedef std::map<propositional_variable_instantiation, stategraph_vertex>::iterator vertex_iterator;

  void create_index()
  {
    // create an index for the vertices in the control flow graph with a given name
    for (std::map<propositional_variable_instantiation, stategraph_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      stategraph_vertex& v = i->second;
      m_stategraph_index[v.X.name()].insert(&v);
    }
  }

  bool has_vertex(const stategraph_vertex* u)
  {
    for (vertex_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      stategraph_vertex& v = i->second;
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
    for (vertex_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      stategraph_vertex& u = i->second;
      for (std::set<stategraph_edge>::iterator j = u.incoming_edges.begin(); j != u.incoming_edges.end(); ++j)
      {
        if (!has_vertex(j->source))
        {
          std::cout << "error: source not found!" << std::endl;
        }
        if (!has_vertex(j->target))
        {
          std::cout << "error: target not found!" << std::endl;
        }
      }
      for (std::set<stategraph_edge>::iterator j = u.outgoing_edges.begin(); j != u.outgoing_edges.end(); ++j)
      {
        if (!has_vertex(j->source))
        {
          std::cout << "error: source not found!" << std::endl;
        }
        if (!has_vertex(j->target))
        {
          std::cout << "error: target not found!" << std::endl;
        }
      }
    }
  }

  control_flow_graph()
  {}

  control_flow_graph(const control_flow_graph& other)
    : m_control_vertices(other.m_control_vertices)
  {
    // reset the pointers
    for (std::map<propositional_variable_instantiation, stategraph_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      stategraph_vertex& u = i->second;

      std::vector<stategraph_edge> iedges(u.incoming_edges.begin(), u.incoming_edges.end());
      for (std::vector<stategraph_edge>::iterator j = iedges.begin(); j != iedges.end(); ++j)
      {
        stategraph_vertex& v = *(j->source);
        stategraph_vertex& v_new = m_control_vertices.find(v.X)->second;
        j->source = &v_new;
        stategraph_vertex& w = *(j->target);
        stategraph_vertex& w_new = m_control_vertices.find(w.X)->second;
        j->target = &w_new;
      }
      u.incoming_edges = std::set<stategraph_edge>(iedges.begin(), iedges.end());

      std::vector<stategraph_edge> oedges(u.outgoing_edges.begin(), u.outgoing_edges.end());
      for (std::vector<stategraph_edge>::iterator j = oedges.begin(); j != oedges.end(); ++j)
      {
        stategraph_vertex& v = *(j->source);
        stategraph_vertex& v_new = m_control_vertices.find(v.X)->second;
        j->source = &v_new;
        stategraph_vertex& w = *(j->target);
        stategraph_vertex& w_new = m_control_vertices.find(w.X)->second;
        j->target = &w_new;
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

  vertex_iterator begin()
  {
    return m_control_vertices.begin();
  }

  vertex_iterator end()
  {
    return m_control_vertices.end();
  }

  std::set<stategraph_vertex*>& index(const propositional_variable_instantiation& X)
  {
    return m_stategraph_index[X.name()];
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "--- control flow graph ---" << std::endl;
    for (std::map<propositional_variable_instantiation, stategraph_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      out << "vertex " << i->second.print() << std::endl;
    }
    return out.str();
  }

  std::string print_marking() const
  {
    std::ostringstream out;
    for (std::map<propositional_variable_instantiation, stategraph_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
    {
      const stategraph_vertex& v = i->second;
      out << v.print_marking() << std::endl;
    }
    return out.str();
  }
};

} // namespace detail
} // namespace pbes_system
} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_H
