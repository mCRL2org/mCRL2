// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_symbolic.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_SYMBOLIC_H
#define MCRL2_PBES_PBESINST_SYMBOLIC_H

#include <cassert>
#include <set>
#include <iostream>
#include <sstream>
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

class pbesinst_symbolic_rewriter
{
  protected:
    data::rewriter m_rewriter;
    data::data_enumerator m_enumerator;
    data::rewriter_with_variables m_rewriter_with_variables;
    bool m_enumerate_infinite_sorts;
    bool m_skip_data;

  public:
    typedef pbes_expression_with_propositional_variables term_type;
    typedef data::data_expression_with_variables data_term_type;
    typedef data::variable variable_type;

    pbesinst_symbolic_rewriter(const data::data_specification& dataspec, data::rewrite_strategy strategy = data::jitty, bool enumerate_infinite_sorts = true, bool skip_data = false)
      : m_rewriter(dataspec, strategy),
        m_enumerator(dataspec, m_rewriter),
        m_rewriter_with_variables(m_rewriter),
        m_enumerate_infinite_sorts(enumerate_infinite_sorts),
        m_skip_data(skip_data)
    {}

    term_type operator()(const term_type& x) const
    {
      typedef data::mutable_map_substitution<std::map< variable_type, data_term_type> > substitution_function;
      typedef core::term_traits<term_type> tr;
      substitution_function sigma;
      detail::enumerate_quantifiers_builder<term_type, data::rewriter_with_variables, data::data_enumerator, substitution_function> r(m_rewriter_with_variables, m_enumerator, m_enumerate_infinite_sorts, m_skip_data);
      term_type result = r(x, sigma);
      return result;
    }

    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      typedef data::mutable_substitution_composer<SubstitutionFunction> substitution_function;
      typedef core::term_traits<term_type> tr;
      detail::enumerate_quantifiers_builder<term_type, data::rewriter_with_variables, data::data_enumerator, substitution_function> r(m_rewriter_with_variables, m_enumerator, m_enumerate_infinite_sorts, m_skip_data);
      term_type result = r(x, substitution_function(sigma));
      return result;
    }
};

/// \brief Algorithm class for the symbolic_exploration instantiation algorithm.
class pbesinst_symbolic_algorithm
{
  public:
    typedef propositional_variable_instantiation state_type;
    typedef core::term_traits<pbes_expression> tr;

  protected:
    /// \brief The PBES that is being instantiated.
    pbes& m_pbes;

    /// \brief The rewriter.
    pbesinst_symbolic_rewriter m_rewriter;

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
    pbesinst_symbolic_algorithm(pbes& p)
      : m_pbes(p),
        m_rewriter(p.data())
    {
      pbes_system::algorithms::instantiate_global_variables(p);

      // initialize m_equation_index
      std::size_t eqn_index = 0;
      for (std::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        m_equation_index[i->variable().name()] = eqn_index++;
      }
    }

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    void run()
    {
      init = tr::term2propvar(m_rewriter(m_pbes.initial_state()));
      todo.insert(init);
      mCRL2log(log::debug, "symbolic") << "discovered vertex " << pbes_system::pp(init) << std::endl;

      while (!todo.empty())
      {
        state_type X = *todo.begin();
        mCRL2log(log::debug, "symbolic") << "handling vertex " << pbes_system::pp(X) << std::endl;
        todo.erase(todo.begin());
        done.insert(X);
        std::size_t index = m_equation_index[X.name()];
        const pbes_equation& eqn = m_pbes.equations()[index];
        pbes_expression phi = eqn.formula();
        pbes_expression_with_propositional_variables psi = m_rewriter(phi, make_sequence_sequence_substitution(eqn.variable().parameters(), X.parameters()));
        for (propositional_variable_instantiation_list::iterator i = psi.propositional_variables().begin(); i != psi.propositional_variables().end(); ++i)
        {
          if (done.find(*i) == done.end())
          {
            todo.insert(*i);
            mCRL2log(log::debug, "symbolic") << "discovered vertex " << pbes_system::pp(*i) << std::endl;
          }
        }
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_SYMBOLIC_H
