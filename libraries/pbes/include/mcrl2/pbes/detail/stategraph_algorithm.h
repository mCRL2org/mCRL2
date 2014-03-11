// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_ALGORITHM_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_ALGORITHM_H

#include <algorithm>
#include <sstream>
#include <boost/bind.hpp>
#include "mcrl2/data/undefined.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/sequence.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Remove an element from v, and return it.
template <typename Container>
typename Container::value_type pick_element(Container& v)
{
  auto i = v.begin();
  typename Container::value_type result = *i;
  v.erase(i);
  return result;
}

inline
std::string print_index(std::size_t index)
{
  std::ostringstream out;
  if (index == data::undefined_index())
  {
    out << "undefined";
  }
  else
  {
    out << index;
  }
  return out.str();
}

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

// Vertex in a local control flow graph.
class local_control_flow_graph_vertex: public control_flow_graph_vertex
{
  protected:
    data::data_expression m_value;
    std::set<data::variable> m_sig;
    mutable std::set<data::variable> m_marking;    // used in the reset variables procedure
    std::vector<bool> m_marked_parameters; // will be set after computing the marking

    mutable std::map<const local_control_flow_graph_vertex*, std::set<std::size_t> > m_outgoing_edges;
    // the mapped values are the edge labels; note that there can be multiple edges with different labels

    mutable std::map<const local_control_flow_graph_vertex*, std::set<std::size_t> > m_incoming_edges;

  public:
    local_control_flow_graph_vertex(const core::identifier_string& name, std::size_t index, const data::variable& variable, const data::data_expression& value)
      : control_flow_graph_vertex(name, index, variable), m_value(value)
    {}

    local_control_flow_graph_vertex(const core::identifier_string& name, const data::data_expression& value)
      : control_flow_graph_vertex(name, data::undefined_index(), data::undefined_variable()), m_value(value)
    {}

    bool has_variable() const
    {
      return m_index != data::undefined_index();
    }

    const data::data_expression& value() const
    {
      return m_value;
    }

    const std::set<data::variable>& signature() const
    {
      return m_sig;
    }

    const std::set<data::variable>& marking() const
    {
      return m_marking;
    }

    void set_marking(const std::set<data::variable>& marking) const
    {
      m_marking = marking;
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

    std::set<std::size_t> marking_variable_indices(const stategraph_pbes& p) const
    {
      std::set<std::size_t> result;
      for (auto i = m_marking.begin(); i != m_marking.end(); ++i)
      {
        // TODO: make this code more efficient
        const stategraph_equation& eqn = *find_equation(p, m_name);
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
      return m_marked_parameters[i];
    }

    std::string print_marking() const
    {
      std::ostringstream out;
      out << "vertex " << m_name << " = " << core::detail::print_set(m_marking);
      return out.str();
    }

    std::string print_outgoing_edges() const;
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
      auto outgoing_edges = i->outgoing_edges();
      for (auto j = outgoing_edges.begin(); j != outgoing_edges.end(); ++j)
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
    local_control_flow_graph_vertex v_(Y, k1, d1, e1);

    // check if v_ already exists in vertices; if not it is added
    auto j = std::find(vertices.begin(), vertices.end(), v_);
    if (j == vertices.end())
    {
      mCRL2log(log::debug1, "stategraph") << " add vertex v = " << v_ << std::endl;
      auto k = vertices.insert(v_);
      todo.insert(&(*k.first));
      j = k.first;
    }
    const local_control_flow_graph_vertex& v = *j; // v == v_, and v is an element of the set vertices

    mCRL2log(log::debug1, "stategraph") << " u.outgoing_edges() = " << u.print_outgoing_edges() << std::endl;

    // add edge (u, v)
    auto q = u.outgoing_edges().find(&v);
    if (u.outgoing_edges().find(&v) == u.outgoing_edges().end() || q->second.find(edge_label) == q->second.end())
    {
      mCRL2log(log::debug1, "stategraph") << " add edge " << u << " -> " << v << std::endl;
      u.insert_outgoing_edge(&v, edge_label);
      v.insert_incoming_edge(&u, edge_label);
    }
    else
    {
      mCRL2log(log::debug1, "stategraph") << " edge already exists!" << std::endl;
    }
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

/// \brief Algorithm class for the computation of the local and global control flow graphs
class stategraph_algorithm
{
  public:
    // simplify and rewrite the expression x
    pbes_expression simplify(const pbes_expression& x) const
    {
      data::detail::simplify_rewriter r;
      stategraph_simplifying_rewriter<pbes_expression, data::detail::simplify_rewriter> R(r);
      return R(x);
    }

    // simplify and rewrite the guards of the pbes p
    void simplify(stategraph_pbes& p) const
    {
      std::vector<stategraph_equation>& equations = p.equations();
      for (std::size_t k = 0; k < equations.size(); k++)
      {
        stategraph_equation& eqn = equations[k];
        std::vector<predicate_variable>& predvars = eqn.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          predvars[i].guard = simplify(predvars[i].guard);
        }
      }
    }

  protected:
    // the pbes that is considered
    stategraph_pbes m_pbes;

    // a data rewriter
    data::rewriter m_datar;

    // determines how local control flow parameters are computed
    //
    // Keuze uit twee alternatieven voor de berekening van lokale CFPs.
    //  * Een parameter d^X[n] is een LCFP indien voor alle i waarvoor geldt pred(phi_X,i) = X, danwel:
    //          a. copy(X,i,n) is undefined en source(X,i,n) and dest(X,i,n) zijn beide defined, of
    //          b. copy(X,i,n) is defined (en gelijk aan n) en source(X,i,n) en dest(X,i,n) zijn beide undefined.
    //  * Een parameter d^X[n] is een LCFP indien voor alle i waarvoor geldt pred(phi_X,i) = X, danwel:
    //          a. source(X,i,n) and dest(X,i,n) zijn beide defined, of
    //          b. copy(X,i,n) is defined en gelijk aan n.
    //
    // De eerste definieert in feite een exclusive or, terwijl de tweede een standaard or is.
    bool m_use_alternative_lcfp_criterion;

    // determines how global control flow parameters are related
    //
    // Keuze uit twee alternatieven voor het relateren van CFPs.
    //  * Parameters d^X[n] and d^Y[m] zijn gerelateerd als danwel:
    //         a. er is een i z.d.d. copy(X, i, n) = m, of
    //         b. er is een i z.d.d. copy(Y, i, m) = n
    //  * Parameters d^X[n] and d^Y[m] zijn gerelateerd als danwel:
    //         a. er is een i z.d.d. copy(X, i, n) = m, en dest(X, i, m) is ongedefinieerd, of
    //         b. er is een i z.d.d. copy(Y, i, m) = n en dest(Y, i, n) is ongedefinieerd.
    // Hier zit het verschil er dus in dat we, in het tweede geval, parameters alleen relateren als er een copy is
    // van de een naar de ander EN de dest in dat geval ongedefinieerd is.
    bool m_use_alternative_gcfp_relation;

    // determines how global control flow parameters are selected
    //
    // Keuze voor de selectie van globale CFPs (of globale consistentie eisen).
    //  * Een set van CFPs is consistent als voor elke d^X[n], en voor alle Y in bnd(E)\{X} (dus in alle andere vergelijkingen), voor alle i waarvoor geldt pred(phi_Y, i) = X, danwel:
    //         a. dest(Y, i, n) is gedefinieerd, of
    //         b. copy(Y, i, m) = n voor een of andere globale CFP d^Y[m]
    // Deze eis is in principe voldoende om globale CFPs te identificeren. Als we echter een strikte scheiding tussen control flow parameters en data parameters willen bewerkstelligen, dan moet hier optioneel de volgende eis aan toegevoegd worden:
    //  * Een set van CFPs is consistent als de voorgaande eisen gelden, en bovendien voor elke d^X[n] geldt dat voor alle i waarvoor pred(phi_X, i) = Y != X, als d^X[n] affects data(phi_X, i)[m], dan is d^Y[m] een globale control flow parameter.
    // Waar de eerste gemarkeerd is als "detect conflicts for parameters of Y in equations of the form X(d) = ... Y(e)"
    // en de tweede als "detect conflicts for parameters of X in equations of the form X(d) = ... Y(e)".
    bool m_use_alternative_gcfp_consistency;

    // the local control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_LCFP;

    // the global control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_GCFP;

    // the vertices of the control flow graph
    std::vector<global_control_flow_graph_vertex> m_global_control_flow_graph_vertices;

    // the connected components in the control flow graph; a component contains the indices in the vector m_global_control_flow_graph_vertices
    std::vector<std::set<std::size_t> > m_connected_components;

    // the possible values of the connected components in the control flow graph
    std::vector<std::set<data::data_expression> > m_connected_components_values;

    // the local control flow graph(s)
    std::vector<local_control_flow_graph> m_local_control_flow_graphs;

    // for readability
    std::set<data::variable> FV(const data::data_expression& x) const
    {
      return data::find_free_variables(x);
    }

    propositional_variable find_propvar(const pbes& p, const core::identifier_string& X) const
    {
      const std::vector<pbes_equation>& equations = p.equations();
      for (std::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        if (i->variable().name() == X)
        {
          return i->variable();
        }
      }
      throw mcrl2::runtime_error("find_propvar failed!");
      return propositional_variable();
    }

  public:
    std::string print_control_flow_parameters() const
    {
      std::ostringstream out;
      out << "--- control flow parameters ---" << std::endl;
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& dX = k->parameters();
        const std::vector<bool>& cf = utilities::detail::map_element(m_is_GCFP, X.name());

        out << core::pp(X.name()) << " ";
        for (std::size_t i = 0; i < cf.size(); ++i)
        {
          if (cf[i])
          {
            out << data::pp(dX[i]) << " ";
          }
        }
        out << std::endl;
      }
      return out.str();
    }

    std::string print_propvar_parameter(const core::identifier_string& X, std::size_t index) const
    {
      return "(" + core::pp(X) + ", " + data::pp(find_equation(m_pbes, X)->parameters()[index]) + ")";
    }

    std::string print_stategraph_assignment(bool stategraph_value,
                                              std::size_t index,
                                              const pbes_system::propositional_variable& X,
                                              const pbes_system::propositional_variable_instantiation& Y,
                                              const std::string& message,
                                              const data::variable& previous_value = data::variable()
                                             ) const
    {
      std::ostringstream out;
      out << message << ": " << print_propvar_parameter(Y.name(), index) << " -> " << std::boolalpha << stategraph_value;
      out << " because of equation " << core::pp(X.name());
      data::variable_list v = X.parameters();
      if (v.size() > 0)
      {
        out << "(";
        for (data::variable_list::iterator i = v.begin(); i != v.end(); ++i)
        {
          if (i != v.begin())
          {
            out << ", ";
          }
          out << core::pp(i->name());
        }
        out << ")";
      }
      out << " = ... " << pbes_system::pp(Y) << " index = " << index << " " << data::pp(previous_value) << std::endl;
      return out.str();
    }

    void compute_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute control flow parameters ===" << std::endl;
      compute_local_control_flow_parameters();
      compute_global_control_flow_parameters();
      compute_related_global_control_flow_parameters();
      compute_connected_components();
      mCRL2log(log::verbose, "stategraph") << print_control_flow_parameters();
    }

    std::string print_local_control_flow_parameters()
    {
      std::ostringstream out;
      out << "--- local control flow parameters ---" << std::endl;
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& dX = k->parameters();
        const std::vector<bool>& cf = m_is_LCFP[X.name()];

        out << core::pp(X.name()) << " ";
        for (std::size_t i = 0; i < cf.size(); ++i)
        {
          if (cf[i])
          {
            out << data::pp(dX[i]) << " ";
          }
        }
        out << std::endl;
      }
      return out.str();
    }

    // returns true if m is not a key in the map
    template <typename Map>
    bool is_undefined(const Map& m, const typename Map::key_type& key) const
    {
      return m.find(key) == m.end();
    }

    // returns true if the mapping m maps key to value
    template <typename Map>
    bool is_mapped_to(const Map& m, const typename Map::key_type& key, const typename Map::mapped_type& value) const
    {
      auto i = m.find(key);
      return i != m.end() && i->second == value;
    }

    // if key is in m, the corresponding value is returned, otherwise undefined_value
    template <typename Map>
    typename Map::mapped_type mapped_value(const Map& m, const typename Map::key_type& key, const typename Map::mapped_type& undefined_value) const
    {
      auto i = m.find(key);
      if (i != m.end())
      {
        return i->second;
      }
      return undefined_value;
    }

    bool maps_to_and_is_GFCP(const std::map<std::size_t, std::size_t>& m, std::size_t value, const core::identifier_string& X)
    {
      for (auto i = m.begin(); i != m.end(); ++i)
      {
        if (i->second == value && m_is_GCFP[X][i->first])
        {
          return true;
        }
      }
      return false;
    }

    void compute_local_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute local control flow parameters ===" << std::endl;

      const std::vector<stategraph_equation>& equations = m_pbes.equations();

      // initialize all control flow parameters to true
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        const core::identifier_string& X = k->variable().name();
        const std::vector<data::variable>& dX = k->parameters();
        m_is_LCFP[X] = std::vector<bool>(dX.size(), true);
      }

      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        const core::identifier_string& X = k->variable().name();
        const std::vector<data::variable>& dX = k->parameters();
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          const predicate_variable& PVI_X_i = *i;
          if (PVI_X_i.X.name() == X)
          {
            for (std::size_t n = 0; n < dX.size(); n++)
            {
              if (m_use_alternative_lcfp_criterion)
              {
                // Een parameter d^X[n] is een LCFP indien voor alle i waarvoor geldt pred(phi_X,i) = X, danwel:
                // 1. copy(X,i,n) is undefined en source(X,i,n) and dest(X,i,n) zijn beide defined, of
                // 2. copy(X,i,n) is defined en source(X,i,n) en dest(X,i,n) zijn beide undefined.
                if (PVI_X_i.copy.find(n) == PVI_X_i.copy.end())
                {
                  // copy(X,i,n) is undefined
                  if (is_undefined(PVI_X_i.source, n) || is_undefined(PVI_X_i.dest, n))
                  {
                    mCRL2log(log::debug, "stategraph") << "(" << X << ", " << n << ") is not an LCFP because of predicate variable " << pbes_system::pp(PVI_X_i.X) << " in equation " << X << " (copy and source/dest undefined)" << std::endl;
                    m_is_LCFP[X][n] = false;
                  }
                }
                else
                {
                  // copy(X,i,n) is defined
                  if (!is_undefined(PVI_X_i.source, n) || !is_undefined(PVI_X_i.dest, n))
                  {
                    mCRL2log(log::debug, "stategraph") << "(" << X << ", " << n << ") is not an LCFP because of predicate variable " << pbes_system::pp(PVI_X_i.X) << " in equation " << X << " (copy defined and source/dest defined)" << std::endl;
                    m_is_LCFP[X][n] = false;
                  }
                }
              }
              else
              {
                if ((is_undefined(PVI_X_i.source, n) || is_undefined(PVI_X_i.dest, n)) && !is_mapped_to(PVI_X_i.copy, n, n))
                {
                  mCRL2log(log::debug, "stategraph") << "parameter (" << X << ", " << n << ") is not an LCFP due to " << pbes_system::pp(PVI_X_i.X) << "(source and dest undefined, and no copy to self)" << std::endl;
                  m_is_LCFP[X][n] = false;
                }
              }
            }
          }
        }
      }
      mCRL2log(log::debug, "stategraph") << print_local_control_flow_parameters();
    }

    std::string print_global_control_flow_parameters()
    {
      std::ostringstream out;
      out << "--- global control flow parameters ---" << std::endl;
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& dX = k->parameters();
        const std::vector<bool>& cf = m_is_GCFP[X.name()];

        out << core::pp(X.name()) << " ";
        for (std::size_t i = 0; i < cf.size(); ++i)
        {
          if (cf[i])
          {
            out << data::pp(dX[i]) << " ";
          }
        }
        out << std::endl;
      }
      return out.str();
    }

    // Computes the global control flow parameters. The result is stored in m_is_GCFP.
    void compute_global_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute global control flow parameters ===" << std::endl;

      m_is_GCFP = m_is_LCFP;

      const std::vector<stategraph_equation>& equations = m_pbes.equations();

      bool changed;
      do
      {
        changed = false;

        // Detect conflicts for parameters of Y in equations of the form X(d) = ... Y(e)
        for (auto k = equations.begin(); k != equations.end(); ++k)
        {
          const core::identifier_string& X = k->variable().name();
          const std::vector<predicate_variable>& predvars = k->predicate_variables();
          for (auto i = predvars.begin(); i != predvars.end(); ++i)
          {
            const predicate_variable& PVI_X_i = *i;
            const core::identifier_string& Y = PVI_X_i.X.name();
            if (Y != X)
            {
              const stategraph_equation& eq_Y = *find_equation(m_pbes, Y);
              const std::vector<data::variable>& d_Y = eq_Y.parameters();
              for (std::size_t n = 0; n < d_Y.size(); n++)
              {
                if (is_undefined(PVI_X_i.dest, n) && (!maps_to_and_is_GFCP(PVI_X_i.copy, n, X)))
                {
                  if (m_is_GCFP[Y][n])
                  {
                    m_is_GCFP[Y][n] = false;
                    changed = true;
                    mCRL2log(log::debug, "stategraph") << "(" << core::pp(Y) << ", " << n << ") is not a GCFP because of predicate variable " << pbes_system::pp(PVI_X_i.X) << " in equation " << core::pp(X) << std::endl;
                  }
                }
              }
            }
          }
        }

        // Detect conflicts for parameters of X in equations of the form X(d) = ... Y(e)
        if (m_use_alternative_gcfp_consistency)
        {
          for (auto k = equations.begin(); k != equations.end(); ++k)
          {
            const core::identifier_string& X = k->variable().name();
            const std::vector<predicate_variable>& predvars = k->predicate_variables();
            const std::vector<data::variable>& dX = k->parameters();
            std::size_t M = dX.size();

            for (auto i = predvars.begin(); i != predvars.end(); ++i)
            {
              const predicate_variable& PVI_X_i = *i;
              const core::identifier_string& Y = PVI_X_i.X.name();
              if (Y == X)
              {
                continue;
              }
              const data::data_expression_list& e = PVI_X_i.X.parameters();
              std::size_t n = 0;
              for (auto ei = e.begin(); ei != e.end(); ++ei, ++n)
              {
                std::set<data::variable> V = data::find_free_variables(*ei);
                for (std::size_t m = 0; m < M; m++)
                {
                  if (m_is_GCFP[X][m] && !m_is_GCFP[Y][n] && (V.find(dX[m]) != V.end()))
                  {
                    m_is_GCFP[X][m] = false;
                    changed = true;
                    mCRL2log(log::debug, "stategraph") << "(" << core::pp(X) << ", " << m << ") is not a GCFP because of predicate variable " << pbes_system::pp(PVI_X_i.X) << " in equation " << core::pp(X) << std::endl;
                  }
                }
              }
            }
          }
        }
      }
      while (changed);

      mCRL2log(log::verbose, "stategraph") << print_global_control_flow_parameters();
    }

    bool is_local_control_flow_parameter(const core::identifier_string& X, std::size_t i) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator j = m_is_LCFP.find(X);
      assert(j != m_is_LCFP.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
    }

    bool is_global_control_flow_parameter(const core::identifier_string& X, std::size_t i) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator j = m_is_GCFP.find(X);
      assert(j != m_is_GCFP.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
    }

    // returns an iterator to an element of m_global_control_flow_graph_vertices
    std::vector<global_control_flow_graph_vertex>::iterator find_vertex(const core::identifier_string& X, std::size_t n)
    {
      for (auto i = m_global_control_flow_graph_vertices.begin(); i != m_global_control_flow_graph_vertices.end(); ++i)
      {
        if (i->name() == X && i->index() == n)
        {
          return i;
        }
      }
      throw mcrl2::runtime_error("vertex not found in dependency graph");
      return m_global_control_flow_graph_vertices.end();
    }

    // relate (X, n) and (Y, m) in the dependency graph
    // \pre: the equation of X has a lower rank than the equation of Y
    void relate_control_flow_graph_vertices(const core::identifier_string& X, std::size_t n, const core::identifier_string& Y, std::size_t m)
    {
      global_control_flow_graph_vertex& u = *find_vertex(X, n);
      global_control_flow_graph_vertex& v = *find_vertex(Y, m);
      u.neighbors().insert(&v);
      v.neighbors().insert(&u);
    }

    // Determines which control flow parameters are related. This is done by assigning neighbors to the
    // vertices in m_global_control_flow_graph_vertices.
    void compute_related_global_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute related global control flow parameters ===" << std::endl;
      const std::vector<stategraph_equation>& equations = m_pbes.equations();

      // step 1: create vertices in m_global_control_flow_graph_vertices
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        const core::identifier_string& X = k->variable().name();
        const std::vector<data::variable>& dX = k->parameters();
        for (std::size_t n = 0; n < dX.size(); n++)
        {
          if (is_global_control_flow_parameter(X, n))
          {
            m_global_control_flow_graph_vertices.push_back(global_control_flow_graph_vertex(X, n, dX[n]));
          }
        }
      }

      // step 2: create edges between related vertices in m_global_control_flow_graph_vertices
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        const core::identifier_string& X = k->variable().name();
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          const predicate_variable& PVI_X_i = *i;
          const core::identifier_string& Y = PVI_X_i.X.name();
          for (auto j = PVI_X_i.copy.begin(); j != PVI_X_i.copy.end(); ++j)
          {
            std::size_t n = j->first;
            std::size_t m = j->second;
            if (is_global_control_flow_parameter(X, n) && is_global_control_flow_parameter(Y, m))
            {
              if (m_use_alternative_gcfp_relation)
              {
                mCRL2log(log::debug, "stategraph") << "(" << core::pp(X) << ", " << n << ") and (" << core::pp(Y) << ", " << m << ") are related "
                                                   << "because of recursion " << pp(PVI_X_i.X) << " in the equation for " << core::pp(X) << std::endl;
                relate_control_flow_graph_vertices(X, n, Y, m);
              }
              else
              {
                // Twee parameters zijn alleen gerelateerd als er een copy is van de een naar de ander,
                // EN dat de dest in dat geval ongedefinieerd is (dus we weten echt geen concrete waarde
                // voor de parameter op dat punt).
                if (is_undefined(PVI_X_i.dest, m))
                {
                  mCRL2log(log::debug, "stategraph") << "(" << core::pp(X) << ", " << n << ") and (" << core::pp(Y) << ", " << m << ") are related "
                                                       << "because of recursion " << pp(PVI_X_i.X) << " in the equation for " << core::pp(X) << " (dest is undefined)" << std::endl;
                  relate_control_flow_graph_vertices(X, n, Y, m);
                }
              }
            }
          }
        }
      }
    }

    // a connected component is valid if it does not contain two nodes (X, n) and (Y, m) with X == Y
    bool is_valid_connected_component(const std::set<std::size_t>& component) const
    {
      std::set<core::identifier_string> V;
      for (auto i = component.begin(); i != component.end(); ++i)
      {
        const core::identifier_string& X = m_global_control_flow_graph_vertices[*i].name();
        if (V.find(X) != V.end())
        {
          return false;
        }
        V.insert(X);
      }
      return true;
    }

    std::string print_connected_component(const std::set<std::size_t>& component) const
    {
      std::ostringstream out;
      out << "{";
      for (auto i = component.begin(); i != component.end(); ++i)
      {
        if (i != component.begin())
        {
          out << ", ";
        }
        out << m_global_control_flow_graph_vertices[*i];
      }
      out << "}";
      if (!is_valid_connected_component(component))
      {
        out << " invalid";
      }
      return out.str();
    }

    void print_connected_components() const
    {
      for (auto i = m_connected_components.begin(); i != m_connected_components.end(); ++i)
      {
        mCRL2log(log::debug, "stategraph") << print_connected_component(*i) << std::endl;
      }
    }

    // compute the connected component belonging to the vertex m_global_control_flow_graph_vertices[i]
    std::set<std::size_t> compute_connected_component(std::size_t i, std::vector<bool>& done) const
    {
      std::set<std::size_t> todo;
      std::set<std::size_t> component;

      todo.insert(i);
      while (!todo.empty())
      {
        std::size_t u_index = pick_element(todo);
        const global_control_flow_graph_vertex& u = m_global_control_flow_graph_vertices[u_index];
        done[u_index] = true;
        component.insert(u_index);

        for (auto k = u.neighbors().begin(); k != u.neighbors().end(); ++k)
        {
          const global_control_flow_graph_vertex* w = *k;
          std::size_t w_index = w - &(m_global_control_flow_graph_vertices.front());
          if (!done[w_index])
          {
            todo.insert(w_index);
          }
        }
      }
      return component;
    }

    void compute_connected_components()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute connected components ===" << std::endl;

      // done[i] means that m_global_control_flow_graph_vertices[i] has been processed
      std::vector<bool> done(m_global_control_flow_graph_vertices.size(), false);

      for (std::size_t i = 0; i < done.size(); i++)
      {
        if (done[i])
        {
          continue;
        }
        std::set<std::size_t> component = compute_connected_component(i, done);
        m_connected_components.push_back(component);
        mCRL2log(log::debug, "stategraph") << print_connected_component(component) << std::endl;
      }
    }

    // removes the connected components V for which !is_valid_connected_component(V)
    void remove_invalid_connected_components()
    {
      auto i = std::remove_if(m_connected_components.begin(), m_connected_components.end(), !boost::bind(&stategraph_algorithm::is_valid_connected_component, this, _1));
      m_connected_components.erase(i, m_connected_components.end());
      mCRL2log(log::debug, "stategraph") << "Removed invalid connected components. The remaining components are:" << std::endl;
      print_connected_components();
    }

    // Returns true if dX[m] is not only copied.
    //
    // A CFP dX[m] is not only copied if
    //    (1) for some i    : source(X, i, m) is defined
    // or (2) for some Y, i : pred(phi_Y , i) = X and dest(Y, i, m) is defined
    bool is_not_only_copied(const core::identifier_string& X, std::size_t m) const
    {
      // check (1)
      const stategraph_equation& eq_X = *find_equation(m_pbes, X);
      const std::vector<predicate_variable>& predvars = eq_X.predicate_variables();
      for (auto i = predvars.begin(); i != predvars.end(); ++i)
      {
        if (!is_undefined(i->source, m))
        {
          return true;
        }
      }

      // check (2)
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          if (i->X.name() == X && !is_undefined(i->dest, m))
          {
            return true;
          }
        }
      }
      return false;
    }

    // Returns true if all CFPs in component are 'only copied'
    bool has_only_copied_CFPs(const std::set<std::size_t>& component) const
    {
      for (auto i = component.begin(); i != component.end(); ++i)
      {
        const global_control_flow_graph_vertex& u = m_global_control_flow_graph_vertices[*i];
        auto X = u.name();
        auto m = u.index();
        if (is_not_only_copied(X, m))
        {
          return false;
        }
      }
      return true;
    }

    // Removes the connected components V that consist of CFPs that are only copied.
    void remove_only_copy_components()
    {
      auto i = std::remove_if(m_connected_components.begin(), m_connected_components.end(), boost::bind(&stategraph_algorithm::has_only_copied_CFPs, this, _1));
      m_connected_components.erase(i, m_connected_components.end());
      mCRL2log(log::debug, "stategraph") << "Removed only copy components. The remaining components are:" << std::endl;
      print_connected_components();
    }

    const std::vector<bool>& stategraph_values(const core::identifier_string& X) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator i = m_is_GCFP.find(X);
      assert (i != m_is_GCFP.end());
      return i->second;
    }

    // returns the control flow parameters of the propositional variable with name X
    std::set<data::variable> control_flow_parameters(const core::identifier_string& X) const
    {
      std::set<data::variable> result;
      const std::vector<bool>& b = stategraph_values(X);
      const stategraph_equation& eqn = *find_equation(m_pbes, X);
      const std::vector<data::variable>& d = eqn.parameters();
      std::size_t index = 0;
      for (std::vector<data::variable>::const_iterator i = d.begin(); i != d.end(); ++i, index++)
      {
        if (b[index])
        {
          result.insert(*i);
        }
      }
      return result;
    }

    std::vector<std::size_t> control_flow_parameter_indices(const core::identifier_string& X) const
    {
      std::vector<std::size_t> result;
      const stategraph_equation& eqn = *find_equation(m_pbes, X);
      const std::vector<data::variable>& dX = eqn.parameters();
      for (std::size_t k = 0; k < dX.size(); k++)
      {
        if (is_global_control_flow_parameter(X, k))
        {
          result.push_back(k);
        }
      }
      return result;
    }

    std::vector<std::size_t> data_parameter_indices(const core::identifier_string& X) const
    {
      std::vector<std::size_t> result;
      const stategraph_equation& eqn = *find_equation(m_pbes, X);
      const std::vector<data::variable>& dX = eqn.parameters();
      for (std::size_t k = 0; k < dX.size(); k++)
      {
        if (!is_global_control_flow_parameter(X, k))
        {
          result.push_back(k);
        }
      }
      return result;
    }

    // returns the parameters of the propositional variable with name X
    std::set<data::variable> propvar_parameters(const core::identifier_string& X) const
    {
      const stategraph_equation& eqn = *find_equation(m_pbes, X);
      const std::vector<data::variable>& d = eqn.parameters();
      return std::set<data::variable>(d.begin(), d.end());
    }

    // removes parameter values that do not correspond to a control flow parameter
    propositional_variable_instantiation project(const propositional_variable_instantiation& x) const
    {
      core::identifier_string X = x.name();
      data::data_expression_list dX = x.parameters();
      const std::vector<bool>& b = stategraph_values(X);
      std::size_t index = 0;
      std::vector<data::data_expression> d;
      for (data::data_expression_list::iterator i = dX.begin(); i != dX.end(); ++i, index++)
      {
        assert(index < b.size());
        if (b[index])
        {
          d.push_back(*i);
        }
      }
      return propositional_variable_instantiation(X, data::data_expression_list(d.begin(), d.end()));
    }

    // removes parameter values that do not correspond to a control flow parameter
    propositional_variable project_variable(const propositional_variable& x) const
    {
      core::identifier_string X = x.name();
      data::variable_list dX = x.parameters();
      const std::vector<bool>& b = stategraph_values(X);
      std::size_t index = 0;
      std::vector<data::variable> d;
      for (data::variable_list::iterator i = dX.begin(); i != dX.end(); ++i, index++)
      {
        if (b[index])
        {
          d.push_back(*i);
        }
      }
      return propositional_variable(X, data::variable_list(d.begin(), d.end()));
    }

    template <typename Substitution>
    propositional_variable_instantiation apply_substitution(const propositional_variable_instantiation& X, Substitution sigma) const
    {
      return propositional_variable_instantiation(X.name(), data::replace_free_variables(X.parameters(), sigma));
    }

    stategraph_algorithm(const pbes& p, data::rewriter::strategy rewrite_strategy = data::jitty,
                         bool use_alternative_lcfp_criterion = false,
                         bool use_alternative_gcfp_relation = false,
                         bool use_alternative_gcfp_consistency = false
                        )
      : m_datar(p.data(), rewrite_strategy),
        m_use_alternative_lcfp_criterion(use_alternative_lcfp_criterion),
        m_use_alternative_gcfp_relation(use_alternative_gcfp_relation),
        m_use_alternative_gcfp_consistency(use_alternative_gcfp_consistency)
    {
      m_pbes = stategraph_pbes(p);
    }

    /// \brief Returns the vertices of the global control flow graph
    const std::vector<global_control_flow_graph_vertex>& control_flow_graph_vertices() const
    {
      return m_global_control_flow_graph_vertices;
    }

    /// \brief Returns the connected components of the global control flow graph.
    const std::vector<std::set<std::size_t> >& connected_components() const
    {
      return m_connected_components;
    }

    std::string print(const global_control_flow_graph_vertex& u) const
    {
      std::ostringstream out;
      out << '(' << u.name() << ", " << find_equation(m_pbes, u.name())->parameters()[u.index()].name() << ')';
      return out.str();
    }

    /// \brief Computes the values that the CFPs in component can attain.
    std::set<data::data_expression> compute_connected_component_values(const std::set<std::size_t>& component)
    {
      std::set<data::data_expression> result;

      // search for a node that corresponds to a variable in the init of the PBES
      const propositional_variable_instantiation& init = m_pbes.initial_state();
      const core::identifier_string& Xinit = init.name();

      for (auto j = component.begin(); j != component.end(); ++j)
      {
        const global_control_flow_graph_vertex& u = m_global_control_flow_graph_vertices[*j];
        if (u.name() == Xinit)
        {
          data::data_expression d = nth_element(init.parameters(), u.index());
        }
      }

      // source(X, i, k) = v
      for (auto p = component.begin(); p != component.end(); ++p)
      {
        const global_control_flow_graph_vertex& u = m_global_control_flow_graph_vertices[*p];
        const core::identifier_string& X = u.name();
        std::size_t k = u.index();
        const stategraph_equation& eq_X = *find_equation(m_pbes, X);
        const std::vector<predicate_variable>& predvars = eq_X.predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          auto q = i->source.find(k);
          if (q != i->source.end())
          {
            const data::data_expression& v = q->second;
            result.insert(v);
          }
        }
      }

      // dest(X, i, k) = v
      for (auto p = component.begin(); p != component.end(); ++p)
      {
        const global_control_flow_graph_vertex& u = m_global_control_flow_graph_vertices[*p];
        const core::identifier_string& Y = u.name();
        std::size_t k = u.index();
        const stategraph_equation& eq_Y = *find_equation(m_pbes, Y);
        const std::vector<predicate_variable>& predvars = eq_Y.predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          if (i->X.name() != Y)
          {
            continue;
          }
          auto q = i->dest.find(k);
          if (q != i->dest.end())
          {
            const data::data_expression& v = q->second;
            result.insert(v);
          }
        }
      }

      return result;
    }

    void compute_connected_component_values()
    {
      mCRL2log(log::debug, "stategraph") << "Computing values for the components" << std::endl;
      for (auto i = m_connected_components.begin(); i != m_connected_components.end(); ++i)
      {
        std::set<data::data_expression> values = compute_connected_component_values(*i);
        m_connected_components_values.push_back(values);
        mCRL2log(log::debug, "stategraph") << print_connected_component(*i) << " values = " << core::detail::print_set(values) << std::endl;
      }
    }

    // Returns the possible values of the vertex (X, j). N.B. Very inefficient!
    std::vector<data::data_expression> compute_values(const core::identifier_string& X, std::size_t j) const
    {
      // search for a graph that contains (X, j)
      for (std::size_t k = 0; k < m_local_control_flow_graphs.size(); k++)
      {
        auto const& Gk = m_local_control_flow_graphs[k];
        if (Gk.has_vertex(X, j))
        {
          auto const& values = m_connected_components_values[k];
          return std::vector<data::data_expression>(values.begin(), values.end());
        }
      }
      throw mcrl2::runtime_error("error in compute_values: vertex not found");
    }

    void compute_local_control_flow_graph(const std::set<local_control_flow_graph_vertex>& U, const std::map<core::identifier_string, std::size_t>& component_index)
    {
      mCRL2log(log::debug, "stategraph") << "--- compute_local_control_flow_graph for vertices " << core::detail::print_set(U) << std::endl;
      local_control_flow_graph V;
      std::set<const local_control_flow_graph_vertex*> todo;

      for (auto i = U.begin(); i != U.end(); ++i)
      {
        auto k = V.insert(*i);
        todo.insert(&(*k.first));
      }

      while (!todo.empty())
      {
        // u = (X, d, e)
        auto const& u = *pick_element(todo);
        const core::identifier_string& X = u.name();
        const data::variable& d = u.variable();
        const data::data_expression& e = u.value();
        mCRL2log(log::debug1, "stategraph") << "choose todo element (X, d, e) = " << u << std::endl;
        std::size_t k = u.index();
        const stategraph_equation& eq_X = *find_equation(m_pbes, X);
        const std::vector<predicate_variable>& predvars = eq_X.predicate_variables();

        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          std::size_t edge_label = i - predvars.begin();
          const core::identifier_string& Y = i->X.name();
          auto q = component_index.find(Y);

          if (d == data::undefined_variable())
          {
            // case 1: (X, e) -> (Y, d', e')
            mCRL2log(log::debug1, "stategraph") << "case 1" << std::endl;
            if (q != component_index.end()) // (Y, k1) in C
            {
              std::size_t k1 = q->second;
              auto e1 = mapped_value(i->dest, k1, data::undefined_data_expression());
              if (e1 != data::undefined_data_expression())
              {
                // dest(X, i, k') = e'
                mCRL2log(log::debug1, "stategraph") << "case 1: (X, e) -> (Y, d', e') ; dest(X, i, k') = e' ; k' = " << print_index(k1) << std::endl;
                V.insert_edge(todo, m_pbes, u, Y, k1, e1, edge_label);
              }
            }
            // case 2: (X, ?) -> (Y, ?)
            else
            {
              if (X != Y)
              {
                std::size_t k1 = data::undefined_index();
                mCRL2log(log::debug1, "stategraph") << "case 2: (X, ?) -> (Y, ?) ; k' = " << print_index(k1) << std::endl;
                V.insert_edge(todo, m_pbes, u, Y, k1, data::undefined_data_expression(), edge_label);
              }
            }
          }
          else
          {
            // case 3: (X, d, e) -> (Y, d', e')
            if (q != component_index.end()) // (Y, k') in C
            {
              std::size_t k1 = q->second;
              if (is_mapped_to(i->source, k, e))
              {
                // source(X, i, k) = e && dest(X, i, k') = e'
                auto e1 = mapped_value(i->dest, k1, data::undefined_data_expression());
                mCRL2log(log::debug1, "stategraph") << "case 3a: (X, d, e) -> (Y, d', e') ; source(X, i, k) = e && dest(X, i, k') = e' ; k' = " << print_index(k1) << std::endl;
                if (e1 != data::undefined_data_expression())
                {
                  V.insert_edge(todo, m_pbes, u, Y, k1, e1, edge_label);
                }
              }
              else if (Y != X && is_undefined(i->source, k))
              {
                // Y != X && undefined(source(X, i, k)) && dest(X, i, k') = e'
                auto e1 = mapped_value(i->dest, k1, data::undefined_data_expression());
                if (e1 != data::undefined_data_expression())
                {
                  mCRL2log(log::debug1, "stategraph") << "case 3b: (X, d, e) -> (Y, d', e') ; Y != X && undefined(source(X, i, k)) && dest(X, i, k') = e' ; k' = " << print_index(k1) << std::endl;
                  V.insert_edge(todo, m_pbes, u, Y, k1, e1, edge_label);
                }

                // Y != X && undefined(source(X, i, k)) && copy(X, i, k) = k'
                if (mapped_value(i->copy, k, data::undefined_index()) == k1)
                {
                  mCRL2log(log::debug1, "stategraph") << "case 3c: (X, d, e) -> (Y, d', e') ; Y != X && undefined(source(X, i, k)) && copy(X, i, k) = k' ; k' = " << print_index(k1) << std::endl;
                  V.insert_edge(todo, m_pbes, u, Y, k1, e, edge_label);
                }
              }
            }
            // case 4: (X, d, e) -> (Y, ?)
            else
            {
              auto e1 = mapped_value(i->source, k, data::undefined_data_expression());
              if (e1 == e || e1 == data::undefined_data_expression())
              {
                std::size_t k1 = data::undefined_index();
                mCRL2log(log::debug1, "stategraph") << "case 4: (X, d, e) -> (Y, ?)" << std::endl;
                V.insert_edge(todo, m_pbes, u, Y, k1, data::undefined_data_expression(), edge_label);
              }
            }
          }
        }
      }
      V.self_check();
      m_local_control_flow_graphs.push_back(V);
      m_local_control_flow_graphs.back().compute_index();
      mCRL2log(log::debug, "stategraph") << m_local_control_flow_graphs.back() << std::endl;
    }

    void compute_local_control_flow_graph(const local_control_flow_graph_vertex& u, const std::map<core::identifier_string, std::size_t>& component_index)
    {
      std::set<local_control_flow_graph_vertex> U;
      U.insert(u);
      compute_local_control_flow_graph(U, component_index);
    }

    // Computes a local control flow graph that corresponds to the given component in m_global_control_flow_graph_vertices.
    void compute_local_control_flow_graph(const std::set<std::size_t>& component, const std::set<data::data_expression>& component_values)
    {
      mCRL2log(log::debug, "stategraph") << "Compute local control flow graphs for component " << print_connected_component(component) << std::endl;

      const propositional_variable_instantiation& init = m_pbes.initial_state();
      const core::identifier_string& Xinit = init.name();

      // preprocessing
      std::map<core::identifier_string, std::size_t> component_index;
      for (auto p = component.begin(); p != component.end(); ++p)
      {
        const global_control_flow_graph_vertex& w = m_global_control_flow_graph_vertices[*p];
        component_index[w.name()] = w.index();
      }

      bool found = false;
      for (auto p = component.begin(); p != component.end(); ++p)
      {
        const global_control_flow_graph_vertex& w = m_global_control_flow_graph_vertices[*p];
        const core::identifier_string& X = w.name();
        if (X == Xinit)
        {
          std::size_t j = w.index();
          data::data_expression e_j = nth_element(init.parameters(), j);
          local_control_flow_graph_vertex u(X, j, w.variable(), e_j);
          compute_local_control_flow_graph(u, component_index);
          found = true;
          break;
        }
      }

      // if no vertex was found corresponding to the initial state, then create a graph from all
      // combinations of vertices and values
      if (!found)
      {
        mCRL2log(log::debug1, "stategraph") << "no vertex found that corresponds to the initial state" << std::endl;
        std::set<local_control_flow_graph_vertex> U;
        for (auto p = component.begin(); p != component.end(); ++p)
        {
          const global_control_flow_graph_vertex& u = m_global_control_flow_graph_vertices[*p];
          for (auto q = component_values.begin(); q != component_values.end(); ++q)
          {
            U.insert(local_control_flow_graph_vertex(u.name(), u.index(), u.variable(), *q));
          }
        }
        compute_local_control_flow_graph(U, component_index);
      }
    }

    void print_local_control_flow_graphs() const
    {
      for (auto i = m_local_control_flow_graphs.begin(); i != m_local_control_flow_graphs.end(); ++i)
      {
        mCRL2log(log::debug, "stategraph") << *i << std::endl;
      }
    }

    void compute_local_control_flow_graphs()
    {
      for (std::size_t i = 0; i < m_connected_components.size(); i++)
      {
        compute_local_control_flow_graph(m_connected_components[i], m_connected_components_values[i]);
      }
    }

    /// \brief Computes the control flow graph
    void run()
    {
      simplify(m_pbes);
      m_pbes.compute_source_dest_copy();
      mCRL2log(log::debug, "stategraph") << "--- source, dest, copy ---\n" << m_pbes.print_source_dest_copy() << std::endl;
      compute_control_flow_parameters();
      remove_invalid_connected_components();
      remove_only_copy_components();
      compute_connected_component_values();
      compute_local_control_flow_graphs();
    }

    const stategraph_pbes& get_pbes() const
    {
      return m_pbes;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_ALGORITHM_H
