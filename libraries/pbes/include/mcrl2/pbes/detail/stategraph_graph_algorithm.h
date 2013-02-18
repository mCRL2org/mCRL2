// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_graph_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_ALGORITHM_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_ALGORITHM_H

#include "mcrl2/pbes/detail/stategraph_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the computation of the stategraph graph
class stategraph_graph_algorithm
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
      atermpp::vector<stategraph_equation>& equations = p.equations();
      for (std::size_t k = 0; k < equations.size(); k++)
      {
        stategraph_equation& eqn = equations[k];
        predicate_variable_vector& predvars = eqn.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          std::pair<propositional_variable_instantiation, pbes_expression>& pvar = predvars[i];
          pvar.second = simplify(pvar.second);
        }
      }
    }

    typedef control_flow_graph::vertex_iterator vertex_iterator;

  protected:
    // the control flow graph
    control_flow_graph m_control_flow_graph;

    // the pbes that is considered
    stategraph_pbes m_pbes;

    // a data rewriter
    data::rewriter m_datar;

    // the control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_control_flow;

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

    std::string print_stategraph_parameters()
    {
      std::ostringstream out;
      out << "--- control flow parameters ---" << std::endl;
      const atermpp::vector<stategraph_equation>& equations = m_pbes.equations();
      for (atermpp::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
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

    void compute_stategraph_parameters()
    {
      const atermpp::vector<stategraph_equation>& equations = m_pbes.equations();
      std::map<core::identifier_string, std::vector<data::variable> > V;

      // initialize all control flow parameters to true
      // initalize V_km to the empty set
      for (atermpp::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        m_is_control_flow[X.name()] = std::vector<bool>(d_X.size(), true);
        V[X.name()] = std::vector<data::variable>(d_X.size(), data::variable());
      }

      // pass 1
      for (atermpp::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const std::vector<data::variable>& d_X = k->parameters();
        const predicate_variable_vector& predvars = k->predicate_variables();
        for (predicate_variable_vector::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          const propositional_variable_instantiation& Y = i->first;
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
      for (atermpp::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
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
        const predicate_variable_vector& predvars = eqn.predicate_variables();
        for (predicate_variable_vector::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          const propositional_variable_instantiation& Y = i->first;
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
      mCRL2log(log::debug) << print_stategraph_parameters();
    }

    const std::vector<bool>& stategraph_values(const core::identifier_string& X) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator i = m_is_control_flow.find(X);
      assert (i != m_is_control_flow.end());
      return i->second;
    }

    // returns the control flow parameters of the propositional variable with name X
    std::set<data::variable> stategraph_parameters(const core::identifier_string& X) const
    {
      std::set<data::variable> result;
      const std::vector<bool>& b = stategraph_values(X);
      const stategraph_equation& eqn = *find_equation(m_pbes, X);
      const std::vector<data::variable>& d = eqn.parameters();
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
      return stategraph_values(X)[i];
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

    void compute_stategraph_graph()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute state graph ===" << std::endl;

      compute_stategraph_parameters();

      data::rewriter datar(m_pbes.data());
      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);

      std::set<stategraph_vertex*> todo;

      // handle the initial state
      propositional_variable_instantiation Xinit = project(m_pbes.initial_state());
      vertex_iterator i = m_control_flow_graph.insert_vertex(Xinit);
      todo.insert(&(i->second));
      mCRL2log(log::debug, "stategraph") << "Xinit = " << pbes_system::pp(m_pbes.initial_state()) << " -> " << pbes_system::pp(Xinit) << std::endl;

      while (!todo.empty())
      {
        std::set<stategraph_vertex*>::iterator i = todo.begin();
        todo.erase(i);
        stategraph_vertex& u = **i;
        stategraph_vertex* source = &u;
        mCRL2log(log::debug, "stategraph") << "selected todo element " << pbes_system::pp(u.X) << std::endl;

        const stategraph_equation& eqn = *find_equation(m_pbes, u.X.name());
        propositional_variable X = project_variable(eqn.variable());
        mCRL2log(log::debug, "stategraph") << "X = " << pbes_system::pp(X) << std::endl;
        mCRL2log(log::debug, "stategraph") << "u.X = " << pbes_system::pp(u.X) << std::endl;
        data::variable_list d = X.parameters();
        data::data_expression_list e = u.X.parameters();
        data::sequence_sequence_substitution<data::variable_list, data::data_expression_list> sigma(d, e);
        mCRL2log(log::debug, "stategraph") << "sigma = " << data::print_substitution(sigma) << std::endl;
        mCRL2log(log::debug, "stategraph") << eqn.print() << std::endl;

        const predicate_variable_vector& predvars = eqn.predicate_variables();
        if (eqn.is_simple())
        {
          mCRL2log(log::debug, "stategraph") << "insert guard " << pbes_system::pp(eqn.formula()) << " in vertex " << pbes_system::pp(u.X) << " (empty case)" << std::endl;
          u.guards.insert(eqn.simple_guard());
        }
        for (predicate_variable_vector::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          const pbes_expression& guard = pbesr(i->second, sigma);
          mCRL2log(log::debug, "stategraph") << "guard = " << pbes_system::pp(guard) << std::endl;
          if (is_false(guard))
          {
            continue;
          }
          mCRL2log(log::debug, "stategraph") << "Y = " << pbes_system::pp(i->first) << std::endl;
          propositional_variable_instantiation Ye = apply_substitution(i->first, sigma);
          mCRL2log(log::debug, "stategraph") << "sigma(Y) = " << pbes_system::pp(Ye) << std::endl;
          propositional_variable_instantiation Y = project(Ye);
          mCRL2log(log::debug, "stategraph") << "project(sigma(Y)) = " << pbes_system::pp(Y) << std::endl;
          propositional_variable_instantiation label = Ye;

          vertex_iterator q = m_control_flow_graph.find(Y);
          bool has_vertex = q != m_control_flow_graph.end();
          if (!has_vertex)
          {
            mCRL2log(log::debug, "stategraph") << "discovered " << pbes_system::pp(Y) << std::endl;
            q = m_control_flow_graph.insert_vertex(Y);
            todo.insert(&(q->second));
          }
          stategraph_vertex& v = q->second;
          u.guards.insert(guard);
          mCRL2log(log::debug, "stategraph") << "insert guard " << pbes_system::pp(guard) << " in vertex " << pbes_system::pp(u.X) << std::endl;
          stategraph_vertex* target = &v;
          stategraph_edge e(source, target, label);
          mCRL2log(log::debug, "stategraph") << "insert edge " << e.print() << std::endl;
          u.outgoing_edges.insert(e);
          v.incoming_edges.insert(e);
        }
      }

      m_control_flow_graph.create_index();
    }

    /// \brief Computes the control flow graph
    void run(const pbes<>& p)
    {
      m_pbes = stategraph_pbes(p);
      m_datar = data::rewriter(p.data());
      simplify(m_pbes);

      stategraph_influence_graph_algorithm ialgo(m_pbes);
      ialgo.run();

      //stategraph_destination_algorithm sdalgo(m_pbes);
      stategraph_source_algorithm sdalgo(m_pbes);
      sdalgo.compute_source();
      mCRL2log(log::debug) << sdalgo.print_source();
      sdalgo.rewrite_propositional_variables();

      //sdalgo.compute_destination();
      //mCRL2log(log::debug) << sdalgo.print_destination();

      compute_stategraph_graph();
      mCRL2log(log::verbose) << m_control_flow_graph.print();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GRAPH_ALGORITHM_H
