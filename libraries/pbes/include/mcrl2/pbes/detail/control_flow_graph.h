// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_GRAPH_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_GRAPH_H

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
#include "mcrl2/pbes/detail/pfnf_pbes.h"
#include "mcrl2/pbes/detail/control_flow_influence.h"
#include "mcrl2/pbes/detail/control_flow_destination.h"
#include "mcrl2/pbes/detail/control_flow_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct control_flow_vertex;

// edge of the control flow graph
struct control_flow_edge
{
  control_flow_vertex* source;
  control_flow_vertex* target;
  propositional_variable_instantiation label;

  control_flow_edge(control_flow_vertex* source_,
                    control_flow_vertex* target_,
                    const propositional_variable_instantiation& label_
                   )
   : source(source_),
     target(target_),
     label(label_)
   {}

  bool operator<(const control_flow_edge& other) const
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

  void protect() const
  {
    label.protect();
  }

  void unprotect() const
  {
    label.unprotect();
  }

  void mark() const
  {
    label.mark();
  }
};

// vertex of the control flow graph
struct control_flow_vertex
{
  propositional_variable_instantiation X;
  atermpp::set<control_flow_edge> incoming_edges;
  atermpp::set<control_flow_edge> outgoing_edges;
  atermpp::set<pbes_expression> guards;
  std::set<data::variable> marking;    // used in the reset variables procedure
  std::vector<bool> marked_parameters; // will be set after computing the marking

  control_flow_vertex(const propositional_variable_instantiation& X_)
    : X(X_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << pbes_system::pp(X);
    out << " edges:";
    for (atermpp::set<control_flow_edge>::const_iterator i = outgoing_edges.begin(); i != outgoing_edges.end(); ++i)
    {
      out << " " << pbes_system::pp(i->target->X);
    }
    out << " guards: " << print_pbes_expressions(guards);
    return out.str();
  }

  std::set<data::variable> free_guard_variables() const
  {
    std::set<data::variable> result;
    for (atermpp::set<pbes_expression>::const_iterator i = guards.begin(); i != guards.end(); ++i)
    {
      pbes_system::find_free_variables(*i, std::inserter(result, result.end()));
    }
    return result;
  }

  std::set<std::size_t> marking_variable_indices(const pfnf_pbes& p) const
  {
    std::set<std::size_t> result;
    for (std::set<data::variable>::const_iterator i = marking.begin(); i != marking.end(); ++i)
    {
      // TODO: make this code more efficient
      const pfnf_equation& eqn = *find_equation(p, X.name());
      const atermpp::vector<data::variable>& d = eqn.parameters();
      for (atermpp::vector<data::variable>::const_iterator j = d.begin(); j != d.end(); ++j)
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

  void protect() const
  {
    X.protect();
  }

  void unprotect() const
  {
    X.unprotect();
  }

  void mark() const
  {
    X.mark();
  }
};

} // namespace detail
} // namespace pbes_system
} // namespace mcrl2
/// \cond INTERNAL_DOCS
namespace atermpp
{
template<>
struct aterm_traits<mcrl2::pbes_system::detail::control_flow_vertex>
{
  static void protect(const mcrl2::pbes_system::detail::control_flow_vertex& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::control_flow_vertex& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::control_flow_vertex& t)
  {
    t.mark();
  }
};

template<>
struct aterm_traits<mcrl2::pbes_system::detail::control_flow_edge>
{
  static void protect(const mcrl2::pbes_system::detail::control_flow_edge& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::control_flow_edge& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::control_flow_edge& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond
namespace mcrl2 {
namespace pbes_system {
namespace detail {

inline
std::string control_flow_edge::print() const
{
  std::ostringstream out;
  out << "(" << pbes_system::pp(source->X) << ", " << pbes_system::pp(target->X) << ") label = " << pbes_system::pp(label);
  return out.str();
}

/// \brief Algorithm class for the computation of the control_flow graph
class control_flow_graph_algorithm
{
  public:
    // simplify and rewrite the expression x
    pbes_expression simplify(const pbes_expression& x) const
    {
      data::detail::simplify_rewriter r;
      control_flow_simplifying_rewriter<pbes_expression, data::detail::simplify_rewriter> R(r);
      return R(x);
    }

    // simplify and rewrite the guards of the pbes p
    void simplify(pfnf_pbes& p) const
    {
      atermpp::vector<pfnf_equation>& equations = p.equations();
      for (atermpp::vector<pfnf_equation>::iterator k = equations.begin(); k != equations.end(); ++k)
      {
        simplify(k->h());
        atermpp::vector<pfnf_implication>& implications = k->implications();
        for (atermpp::vector<pfnf_implication>::iterator i = implications.begin(); i != implications.end(); ++i)
        {
          i->g() = simplify(i->g());
        }
      }
    }

  protected:
    typedef atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator vertex_iterator;

    // vertices of the control flow graph
    atermpp::map<propositional_variable_instantiation, control_flow_vertex> m_control_vertices;

    // the pbes that is considered
    pfnf_pbes m_pbes;

    // a data rewriter
    data::rewriter m_datar;

    // the control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_control_flow;

    // an index for the vertices in the control flow graph with a given name
    std::map<core::identifier_string, std::set<control_flow_vertex*> > m_control_flow_index;

    propositional_variable find_propvar(const pbes<>& p, const core::identifier_string& X) const
    {
      const atermpp::vector<pbes_equation>& equations = p.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        if (i->variable().name() == X)
        {
          return i->variable();
        }
      }
      throw mcrl2::runtime_error("find_propvar failed!");
      return propositional_variable();
    }

    std::string print_control_flow_parameters()
    {
      std::ostringstream out;
      out << "--- control flow parameters ---" << std::endl;
      const atermpp::vector<pfnf_equation>& equations = m_pbes.equations();
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const atermpp::vector<data::variable>& d_X = k->parameters();
        const std::vector<bool>& cf = m_is_control_flow[X.name()];

        out << core::pp(X.name()) << " ";
        for (std::size_t i = 0; i < cf.size(); ++i)
        {
          if (cf[i])
          {
            out << data::pp(d_X[i]) << " ";
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

    std::string print_control_flow_assignment(bool control_flow_value,
                                              std::size_t index,
                                              const pbes_system::propositional_variable& X,
                                              const pbes_system::propositional_variable_instantiation& Xij,
                                              const std::string& message,
                                              const data::variable& previous_value = data::variable()
                                             ) const
    {
      std::ostringstream out;
      out << "[cf] " << message << ": " << print_propvar_parameter(Xij.name(), index) << " -> " << std::boolalpha << control_flow_value;
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
      out << " = ... " << pbes_system::pp(Xij) << " index = " << index << " " << data::pp(previous_value) << std::endl;
      return out.str();
    }

    void compute_control_flow_parameters()
    {
      const atermpp::vector<pfnf_equation>& equations = m_pbes.equations();
      std::map<core::identifier_string, std::vector<data::variable> > V;

      // initialize all control flow parameters to true
      // initalize V_km to the empty set
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const atermpp::vector<data::variable>& d_X = k->parameters();
        m_is_control_flow[X.name()] = std::vector<bool>(d_X.size(), true);
        V[X.name()] = std::vector<data::variable>(d_X.size(), data::variable());
      }

      // pass 1
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const atermpp::vector<data::variable>& d_X = k->parameters();
        const atermpp::vector<pfnf_implication>& implications = k->implications();
        for (atermpp::vector<pfnf_implication>::const_iterator i = implications.begin(); i != implications.end(); ++i)
        {
          const atermpp::vector<propositional_variable_instantiation>& propvars = i->variables();
          for (atermpp::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
          {
            const propositional_variable_instantiation& Xij = *j;
            data::data_expression_list d = Xij.parameters();
            std::size_t index = 0;
            for (data::data_expression_list::const_iterator q = d.begin(); q != d.end(); ++q, ++index)
            {
              if (data::is_variable(*q))
              {
                std::vector<data::variable>::const_iterator found = std::find(d_X.begin(), d_X.end(), *q);
                if (found != d_X.end())
                {
                  if (V[Xij.name()][index] == data::variable())
                  {
                    V[Xij.name()][index] = *q;
                    mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(true, index, X, Xij, "pass 1");
                  }
                  else
                  {
                    bool is_same_value = (V[Xij.name()][index] == *q);
                    m_is_control_flow[Xij.name()][index] = is_same_value;
                    mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(is_same_value, index, X, Xij, "pass 1", V[Xij.name()][index]);
                  }
                }
              }
            }
          }
        }
      }

      // pass 2
      std::set<core::identifier_string> todo;
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        todo.insert(k->variable().name());
      }

      while (!todo.empty())
      {
        core::identifier_string name = *todo.begin();
        todo.erase(todo.begin());
        const pfnf_equation& eqn = *find_equation(m_pbes, name);
        propositional_variable X = eqn.variable();
        const atermpp::vector<data::variable>& d_X = eqn.parameters();
        const atermpp::vector<pfnf_implication>& implications = eqn.implications();
        for (atermpp::vector<pfnf_implication>::const_iterator i = implications.begin(); i != implications.end(); ++i)
        {
          const atermpp::vector<propositional_variable_instantiation>& propvars = i->variables();
          for (atermpp::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
          {
            const propositional_variable_instantiation& Xij = *j;
            data::data_expression_list d = Xij.parameters();
            std::size_t index = 0;
            for (data::data_expression_list::const_iterator q = d.begin(); q != d.end(); ++q, ++index)
            {
              if (is_constant(*q))
              {
                continue;
              }
              else if (data::is_variable(*q))
              {
                std::vector<data::variable>::const_iterator found = std::find(d_X.begin(), d_X.end(), *q);
                if (found == d_X.end())
                {
                  if (m_is_control_flow[Xij.name()][index] != false)
                  {
                    m_is_control_flow[Xij.name()][index] = false;
                    todo.insert(Xij.name());
                    mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                  }
                }
                else
                {
                  if (X.name() == Xij.name() && (found != d_X.begin() + index))
                  {
                    if (m_is_control_flow[Xij.name()][index] != false)
                    {
                      m_is_control_flow[Xij.name()][index] = false;
                      todo.insert(Xij.name());
                      mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                    }
                  }
                  else
                  {
                    if (!m_is_control_flow[X.name()][found - d_X.begin()])
                    {
                      if (m_is_control_flow[Xij.name()][index] != false)
                      {
                        m_is_control_flow[Xij.name()][index] = false;
                        todo.insert(Xij.name());
                        mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                      }
                    }
                  }
                }
              }
              else
              {
                if (m_is_control_flow[Xij.name()][index] != false)
                {
                  m_is_control_flow[Xij.name()][index] = false;
                  todo.insert(Xij.name());
                  mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                }
              }
            }
          }
        }
      }
    }

    const std::vector<bool>& control_flow_values(const core::identifier_string& X) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator i = m_is_control_flow.find(X);
      assert (i != m_is_control_flow.end());
      return i->second;
    }

    // returns the control flow parameters of the propositional variable with name X
    std::set<data::variable> control_flow_parameters(const core::identifier_string& X) const
    {
      std::set<data::variable> result;
      const std::vector<bool>& b = control_flow_values(X);
      const pfnf_equation& eqn = *find_equation(m_pbes, X);
      const atermpp::vector<data::variable>& d = eqn.parameters();
      std::size_t index = 0;
      for (atermpp::vector<data::variable>::const_iterator i = d.begin(); i != d.end(); ++i, index++)
      {
        if (b[index])
        {
          result.insert(*i);
        }
      }
      return result;
    }

    // returns true if the i-th parameter of X is a control flow parameter
    bool is_control_flow_parameter(const core::identifier_string& X, std::size_t i) const
    {
      return control_flow_values(X)[i];
    }

    // returns the parameters of the propositional variable with name X
    std::set<data::variable> propvar_parameters(const core::identifier_string& X) const
    {
      const pfnf_equation& eqn = *find_equation(m_pbes, X);
      const atermpp::vector<data::variable>& d = eqn.parameters();
      return std::set<data::variable>(d.begin(), d.end());
    }

    // removes parameter values that do not correspond to a control flow parameter
    propositional_variable_instantiation project(const propositional_variable_instantiation& x) const
    {
      core::identifier_string X = x.name();
      data::data_expression_list d_X = x.parameters();
      const std::vector<bool>& b = control_flow_values(X);
      std::size_t index = 0;
      std::vector<data::data_expression> d;
      for (data::data_expression_list::iterator i = d_X.begin(); i != d_X.end(); ++i, index++)
      {
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
      data::variable_list d_X = x.parameters();
      const std::vector<bool>& b = control_flow_values(X);
      std::size_t index = 0;
      std::vector<data::variable> d;
      for (data::variable_list::iterator i = d_X.begin(); i != d_X.end(); ++i, index++)
      {
        if (b[index])
        {
          d.push_back(*i);
        }
      }
      return propositional_variable(X, data::variable_list(d.begin(), d.end()));
    }

    // x is a projected value
    // \pre x is not present in m_control_vertices
    vertex_iterator insert_control_flow_vertex(const propositional_variable_instantiation& X)
    {
      std::pair<vertex_iterator, bool> p = m_control_vertices.insert(std::make_pair(X, control_flow_vertex(X)));
      assert(p.second);
      return p.first;
    }

    template <typename Substitution>
    propositional_variable_instantiation apply_substitution(const propositional_variable_instantiation& X, Substitution sigma) const
    {
      return propositional_variable_instantiation(X.name(), data::replace_free_variables(X.parameters(), sigma));
    }

    void compute_control_flow_graph()
    {
      compute_control_flow_parameters();

      data::rewriter datar(m_pbes.data());
      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);

      std::set<control_flow_vertex*> todo;

      // handle the initial state
      propositional_variable_instantiation Xinit = project(m_pbes.initial_state());
      vertex_iterator i = insert_control_flow_vertex(Xinit);
      todo.insert(&(i->second));
      mCRL2log(log::debug, "control_flow") << "[cf] Xinit = " << pbes_system::pp(m_pbes.initial_state()) << " -> " << pbes_system::pp(Xinit) << std::endl;

      while (!todo.empty())
      {
        std::set<control_flow_vertex*>::iterator i = todo.begin();
        todo.erase(i);
        control_flow_vertex& u = **i;
        control_flow_vertex* source = &u;
        mCRL2log(log::debug, "control_flow") << "[cf] selected todo element " << pbes_system::pp(u.X) << std::endl;

        const pfnf_equation& eqn = *find_equation(m_pbes, u.X.name());
        propositional_variable X = project_variable(eqn.variable());
        mCRL2log(log::debug, "control_flow") << "[cf] X = " << pbes_system::pp(X) << std::endl;
        mCRL2log(log::debug, "control_flow") << "[cf] u.X = " << pbes_system::pp(u.X) << std::endl;
        data::variable_list d = X.parameters();
        data::data_expression_list e = u.X.parameters();
        data::sequence_sequence_substitution<data::variable_list, data::data_expression_list> sigma(d, e);
        mCRL2log(log::debug, "control_flow") << "[cf] sigma = " << data::print_substitution(sigma) << std::endl;
        const atermpp::vector<pfnf_implication>& implications = eqn.implications();
        if (implications.empty())
        {
          u.guards.insert(eqn.h());
        }
        for (atermpp::vector<pfnf_implication>::const_iterator i = implications.begin(); i != implications.end(); ++i)
        {
          const atermpp::vector<propositional_variable_instantiation>& propvars = i->variables();
          pbes_expression guard = pbesr(and_(eqn.h(), i->g()), sigma);
          if (is_false(guard))
          {
            continue;
          }

          for (atermpp::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
          {
            mCRL2log(log::debug, "control_flow") << "[cf] Xij = " << pbes_system::pp(*j) << std::endl;
            propositional_variable_instantiation Xij = apply_substitution(*j, sigma);
            mCRL2log(log::debug, "control_flow") << "[cf] Xij_sigma = " << pbes_system::pp(Xij) << std::endl;
            propositional_variable_instantiation Y = project(Xij);
            mCRL2log(log::debug, "control_flow") << "[cf] Xij_sigma_projected = " << pbes_system::pp(Y) << std::endl;
            propositional_variable_instantiation label = Xij;
            vertex_iterator q = m_control_vertices.find(Y);
            if (q == m_control_vertices.end())
            {
              // vertex Y does not yet exist
              mCRL2log(log::debug, "control_flow") << "[cf] discovered " << pbes_system::pp(Y) << std::endl;
              vertex_iterator k = insert_control_flow_vertex(Y);
              control_flow_vertex& v = k->second;
              u.guards.insert(guard);
              todo.insert(&v);
              control_flow_vertex* target = &v;
              control_flow_edge e(source, target, label);
              mCRL2log(log::debug, "control_flow") << "[cf] insert edge " << e.print() << std::endl;
              u.outgoing_edges.insert(e);
              v.incoming_edges.insert(e);
            }
            else
            {
              control_flow_vertex& v = q->second;
              u.guards.insert(guard);
              control_flow_vertex* target = &v;
              control_flow_edge e(source, target, label);
              mCRL2log(log::debug, "control_flow") << "[cf] insert edge " << e.print() << std::endl;
              u.outgoing_edges.insert(e);
              v.incoming_edges.insert(e);
            }
          }
        }
      }

      // create an index for the vertices in the control flow graph with a given name
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        m_control_flow_index[v.X.name()].insert(&v);
      }
    }

    std::string print_control_flow_graph() const
    {
      std::ostringstream out;
      out << "--- control flow graph ---" << std::endl;
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        out << "vertex " << i->second.print() << std::endl;
      }
      return out.str();
    }

    /// \brief Computes the control flow graph
    void run(const pbes<>& p)
    {
      m_pbes = pfnf_pbes(p);
      m_datar = data::rewriter(p.data());
      simplify(m_pbes);

      control_flow_influence_graph_algorithm ialgo(m_pbes);
      ialgo.run();

      control_flow_destination_algorithm sdalgo(m_pbes);
      sdalgo.compute_source();
      mCRL2log(log::debug) << sdalgo.print_source();
      sdalgo.rewrite_propositional_variables();
      // N.B. This modifies m_pbes. It is needed as a precondition for the
      // function compute_control_flow_parameters().

      sdalgo.compute_destination();
      mCRL2log(log::debug) << sdalgo.print_destination();

      compute_control_flow_graph();
      mCRL2log(log::debug) << print_control_flow_parameters();
      mCRL2log(log::verbose) << print_control_flow_graph();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_GRAPH_H
