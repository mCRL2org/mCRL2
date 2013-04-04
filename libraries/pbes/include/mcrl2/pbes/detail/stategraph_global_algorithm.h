// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_global_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_ALGORITHM_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_ALGORITHM_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/stategraph_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the global variant of the stategraph algorithm
class stategraph_global_algorithm: public stategraph_algorithm
{
  public:
    typedef stategraph_algorithm super;

  protected:
    // the control flow graph
    control_flow_graph m_control_flow_graph;

    void compute_control_flow_graph()
    {
      mCRL2log(log::debug, "stategraph") << "=== compute control flow graph ===" << std::endl;

      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);

      std::set<stategraph_vertex*> todo;

      // handle the initial state
      propositional_variable_instantiation u0 = project(m_pbes.initial_state());
      vertex_iterator i = m_control_flow_graph.insert_vertex(u0);
      todo.insert(&(i->second));
      mCRL2log(log::debug, "stategraph") << "u0 = " << pbes_system::pp(u0) << std::endl;

      while (!todo.empty())
      {
        std::set<stategraph_vertex*>::iterator ti = todo.begin();
        stategraph_vertex& u = **ti;
        todo.erase(ti);
        stategraph_vertex* source = &u;
        mCRL2log(log::debug, "stategraph") << "selected todo element u = " << pbes_system::pp(u.X) << std::endl;

        const stategraph_equation& eqn = *find_equation(m_pbes, u.X.name());
        mCRL2log(log::debug, "stategraph") << "eqn = " << eqn.print() << std::endl;
        propositional_variable X = project_variable(eqn.variable());
        data::variable_list d = X.parameters();
        data::data_expression_list e = u.X.parameters();
        data::sequence_sequence_substitution<data::variable_list, data::data_expression_list> sigma(d, e);

        u.sig = pbes_system::algorithms::significant_variables(pbesr(eqn.formula(), sigma));

        const std::vector<predicate_variable>& predvars = eqn.predicate_variables();
        // for (std::vector<predicate_variable>::const_iterator j = predvars.begin(); j != predvars.end(); ++j)
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          const predicate_variable& PV = predvars[i];
          mCRL2log(log::debug, "stategraph") << "Y(e) = " << pbes_system::pp(PV.X) << std::endl;
          pbes_expression g = pbesr(PV.guard, sigma);
          mCRL2log(log::debug, "stategraph") << "g = " << pbes_system::pp(PV.guard) << data::print_substitution(sigma) << " = " << pbes_system::pp(g) << std::endl;
          if (is_false(g))
          {
            continue;
          }
          propositional_variable_instantiation Ye = apply_substitution(PV.X, sigma);
          propositional_variable_instantiation Y = project(Ye);

          mCRL2log(log::debug, "stategraph") << "v = " << pbes_system::pp(Y) << std::endl;

          vertex_iterator q = m_control_flow_graph.find(Y);
          bool has_vertex = q != m_control_flow_graph.end();
          if (!has_vertex)
          {
            mCRL2log(log::debug, "stategraph") << "insert vertex v" << std::endl;
            q = m_control_flow_graph.insert_vertex(Y);
            todo.insert(&(q->second));
          }
          stategraph_vertex& v = q->second;
          stategraph_vertex* target = &v;
          stategraph_edge e(source, target, i);
          mCRL2log(log::debug, "stategraph") << "insert edge (u, v) with label " << i << std::endl;
          u.outgoing_edges.insert(e);
          v.incoming_edges.insert(e);
        }
      }

      m_control_flow_graph.create_index();
    }

    // used for printing the control flow graph
    std::map<core::identifier_string, data::variable_list> print_map() const
    {
      std::map<core::identifier_string, data::variable_list> result;
      const std::vector<stategraph_equation>& eqn = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator i = eqn.begin(); i != eqn.end(); ++i)
      {
        propositional_variable X = project_variable(i->variable());
        result[X.name()] = X.parameters();
      }
      return result;
    }

  public:
    stategraph_global_algorithm(const pbes& p, data::rewriter::strategy rewrite_strategy = data::jitty)
      : stategraph_algorithm(p, rewrite_strategy)
    { }

    /// \brief Computes the control flow graph
    void run()
    {
      super::run();
      compute_control_flow_graph();
      mCRL2log(log::verbose) << "Computed control flow graph" << std::endl;
      mCRL2log(log::debug) << m_control_flow_graph.print(print_map());
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_ALGORITHM_H
