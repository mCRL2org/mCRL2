// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_symbolic.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_SYMBOLIC_H
#define MCRL2_PBES_PBESINST_SYMBOLIC_H

#include "mcrl2/pbes/algorithms.h"

#include "mcrl2/pbes/pbesinst_algorithm.h"



namespace mcrl2::pbes_system {

/// \brief Algorithm class for the symbolic_exploration instantiation algorithm.
class pbesinst_symbolic_algorithm
{
  public:
    using state_type = propositional_variable_instantiation;

  protected:
    /// \brief The PBES that is being instantiated.
    pbes& m_pbes;

    /// \brief Data rewriter.
    data::rewriter datar;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief Propositional variable instantiations that need to be handled.
    std::set<state_type> todo;

    /// \brief Propositional variable instantiations that have been handled.
    std::set<state_type> done;

    /// \brief Data structure for storing the result. E[i] corresponds to the equations
    /// generated from the i-th PBES equation.
    std::multimap<state_type, state_type> edges;

    /// \brief The initial value.
    state_type init;

    /// \brief A lookup map for PBES equations.
    std::map<core::identifier_string, std::size_t> m_equation_index;

  public:
    pbesinst_symbolic_algorithm(pbes& p, data::rewriter::strategy rewrite_strategy = data::jitty)
      : m_pbes(p),
        datar(p.data(), rewrite_strategy),
        R(datar, p.data())
    {
      pbes_system::algorithms::instantiate_global_variables(p);

      // initialize m_equation_index
      std::size_t eqn_index = 0;
      for (const pbes_equation& eqn: p.equations())
      {
        m_equation_index[eqn.variable().name()] = eqn_index++;
      }
    }

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    void run()
    {
      init = atermpp::down_cast<propositional_variable_instantiation>(R(m_pbes.initial_state()));
      todo.insert(init);
      mCRL2log(log::debug) << "discovered vertex " << init << std::endl;

      while (!todo.empty())
      {
        state_type X = *todo.begin();
        mCRL2log(log::debug) << "handling vertex " << X << std::endl;
        todo.erase(todo.begin());
        done.insert(X);
        std::size_t index = m_equation_index[X.name()];
        const pbes_equation& eqn = m_pbes.equations()[index];
        const pbes_expression& phi = eqn.formula();
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X.parameters(), sigma);
        pbes_expression psi = R(phi, sigma);
        R.clear_identifier_generator();
        for (const propositional_variable_instantiation& v: find_propositional_variable_instantiations(psi))
        {
          if (done.find(v) == done.end())
          {
            todo.insert(v);
            mCRL2log(log::debug) << "discovered vertex " << v << std::endl;
          }
        }
      }
    }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_PBESINST_SYMBOLIC_H
