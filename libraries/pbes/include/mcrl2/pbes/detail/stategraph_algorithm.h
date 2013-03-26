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

#include <sstream>
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/significant_variables.h"
#include "mcrl2/pbes/detail/stategraph_graph.h"
#include "mcrl2/utilities/sequence.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the computation of the stategraph graph
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

    typedef control_flow_graph::vertex_iterator vertex_iterator;

  protected:
    // the pbes that is considered
    stategraph_pbes m_pbes;

    // a data rewriter
    data::rewriter m_datar;

    // the control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_control_flow;

    // the local control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_LCFP;

    // the global control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_GCFP;

    // for readability
    std::set<data::variable> FV(const data::data_expression& x) const
    {
      return data::find_free_variables(x);
    }

    // for readability
    template <typename Set>
    bool contains(const Set& S, const typename Set::value_type& s) const
    {
      return S.find(s) != S.end();
    }

    propositional_variable find_propvar(const pbes<>& p, const core::identifier_string& X) const
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

    std::string print_control_flow_parameters()
    {
      std::ostringstream out;
      out << "--- control flow parameters ---" << std::endl;
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
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

      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      std::map<core::identifier_string, std::vector<data::variable> > V;

      // initialize all control flow parameters to true
      // initalize V_km to the empty set
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        m_is_control_flow[X.name()] = std::vector<bool>(d_X.size(), true);
        V[X.name()] = std::vector<data::variable>(d_X.size(), data::variable());
      }

      // pass 1
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (std::vector<predicate_variable>::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          const propositional_variable_instantiation& Y = i->X;
          data::data_expression_list e = Y.parameters();
          std::size_t index = 0;
          for (data::data_expression_list::const_iterator q = e.begin(); q != e.end(); ++q, ++index)
          {
            if (data::is_variable(*q))
            {
              std::vector<data::variable>::const_iterator found = std::find(d_X.begin(), d_X.end(), *q);
              if (found != d_X.end())
              {
                if (V[Y.name()][index] == data::variable())
                {
                  V[Y.name()][index] = *q;
                  mCRL2log(log::debug, "stategraph") << print_stategraph_assignment(true, index, X, Y, "pass 1");
                }
                else
                {
                  bool is_same_value = (V[Y.name()][index] == *q);
                  m_is_control_flow[Y.name()][index] = is_same_value;
                  mCRL2log(log::debug, "stategraph") << print_stategraph_assignment(is_same_value, index, X, Y, "pass 1", V[Y.name()][index]);
                }
              }
            }
          }
        }
      }

      // pass 2
      std::set<core::identifier_string> todo;
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        todo.insert(k->variable().name());
      }

      while (!todo.empty())
      {
        core::identifier_string name = *todo.begin();
        todo.erase(todo.begin());
        const stategraph_equation& eqn = *find_equation(m_pbes, name);
        propositional_variable X = eqn.variable();
        const std::vector<data::variable>& d_X = eqn.parameters();
        const std::vector<predicate_variable>& predvars = eqn.predicate_variables();
        for (std::vector<predicate_variable>::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          const propositional_variable_instantiation& Y = i->X;
          data::data_expression_list e = Y.parameters();
          std::size_t index = 0;
          for (data::data_expression_list::const_iterator q = e.begin(); q != e.end(); ++q, ++index)
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
                if (m_is_control_flow[Y.name()][index] != false)
                {
                  m_is_control_flow[Y.name()][index] = false;
                  todo.insert(Y.name());
                  mCRL2log(log::debug, "stategraph") << print_stategraph_assignment(false, index, X, Y, "pass 2");
                }
              }
              else
              {
                if (X.name() == Y.name() && (found != d_X.begin() + index))
                {
                  if (m_is_control_flow[Y.name()][index] != false)
                  {
                    m_is_control_flow[Y.name()][index] = false;
                    todo.insert(Y.name());
                    mCRL2log(log::debug, "stategraph") << print_stategraph_assignment(false, index, X, Y, "pass 2");
                  }
                }
                else
                {
                  if (!m_is_control_flow[X.name()][found - d_X.begin()])
                  {
                    if (m_is_control_flow[Y.name()][index] != false)
                    {
                      m_is_control_flow[Y.name()][index] = false;
                      todo.insert(Y.name());
                      mCRL2log(log::debug, "stategraph") << print_stategraph_assignment(false, index, X, Y, "pass 2");
                    }
                  }
                }
              }
            }
            else
            {
              if (m_is_control_flow[Y.name()][index] != false)
              {
                m_is_control_flow[Y.name()][index] = false;
                todo.insert(Y.name());
                mCRL2log(log::debug, "stategraph") << print_stategraph_assignment(false, index, X, Y, "pass 2");
              }
            }
          }
        }
      }
      m_pbes.compute_source_dest_copy(m_is_control_flow);
      mCRL2log(log::debug) << print_control_flow_parameters();
      mCRL2log(log::verbose) << "Computed control flow parameters" << std::endl;
    }

    void compute_local_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute local control flow parameters ===" << std::endl;

      const std::vector<stategraph_equation>& equations = m_pbes.equations();

      // initialize all control flow parameters to true
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        m_is_LCFP[X.name()] = std::vector<bool>(d_X.size(), true);
      }

      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          const predicate_variable& pred_X_i = *i;
          if (pred_X_i.X.name() != X.name())
          {
            continue;
          }

          for (std::size_t n = 0; n < d_X.size(); n++)
          {
            std::map<std::size_t, std::size_t>::const_iterator ci = pred_X_i.copy.find(n);
            if (ci != pred_X_i.copy.end() && ci->second != n)
            {
              continue;
            }
            if (pred_X_i.source.find(n) == pred_X_i.source.end() || pred_X_i.dest.find(n) == pred_X_i.dest.end())
            {
              m_is_LCFP[X.name()][n] = false;
            }
          }
        }
      }
    }

    void compute_global_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute local control flow parameters ===" << std::endl;

      m_is_GCFP = m_is_LCFP;

      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          const predicate_variable& pred_X_i = *i;
          std::set<std::size_t> copied;
          for (auto j = pred_X_i.copy.begin(); j != pred_X_i.copy.end(); ++j)
          {
            copied.insert(j->second);
          }
          if (pred_X_i.X.name() == X.name())
          {
            continue;
          }
          for (std::size_t n = 0; n < d_X.size(); n++)
          {
            if (pred_X_i.dest.find(n) == pred_X_i.dest.end() && copied.find(n) == copied.end())
            {
              m_is_GCFP[X.name()][n] = false;
            }
          }
        }
      }
    }

    const std::vector<bool>& stategraph_values(const core::identifier_string& X) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator i = m_is_control_flow.find(X);
      assert (i != m_is_control_flow.end());
      return i->second;
    }

    bool is_control_flow_parameter(const core::identifier_string& X, std::size_t i) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator j = m_is_control_flow.find(X);
      assert(j != m_is_control_flow.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
      // return stategraph_values(X)[i];
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
      const std::vector<data::variable>& d_X = eqn.parameters();
      for (std::size_t k = 0; k < d_X.size(); k++)
      {
        if (is_control_flow_parameter(X, k))
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
      const std::vector<data::variable>& d_X = eqn.parameters();
      for (std::size_t k = 0; k < d_X.size(); k++)
      {
        if (!is_control_flow_parameter(X, k))
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
      data::data_expression_list d_X = x.parameters();
      const std::vector<bool>& b = stategraph_values(X);
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
      const std::vector<bool>& b = stategraph_values(X);
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

    template <typename Substitution>
    propositional_variable_instantiation apply_substitution(const propositional_variable_instantiation& X, Substitution sigma) const
    {
      return propositional_variable_instantiation(X.name(), data::replace_free_variables(X.parameters(), sigma));
    }

  public:

    stategraph_algorithm(const pbes<>& p, data::rewriter::strategy rewrite_strategy = data::jitty)
    {
      m_pbes = stategraph_pbes(p);
      m_datar = data::rewriter(p.data(), rewrite_strategy);
    }

    /// \brief Computes the control flow graph
    void run()
    {
      simplify(m_pbes);
      compute_control_flow_parameters();
      mCRL2log(log::debug, "stategraph") << "--- source, dest, copy ---\n" << m_pbes.print_source_dest_copy() << std::endl;
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
