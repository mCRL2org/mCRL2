// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_algorithm.h
/// \brief Algorithm for instantiating a PBES.

#include <cassert>
#include <set>
#include <iostream>
#include <sstream>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/detail/pbesinst_rewriter.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"

#ifdef PBESINST_FINITE_ALGORITHM
#include "mcrl2/pbes/detail/pbesinst_finite_builder.h"
#endif

#ifndef MCRL2_PBES_PBESINST_ALGORITHM_H
#define MCRL2_PBES_PBESINST_ALGORITHM_H

namespace mcrl2
{

namespace pbes_system
{

using detail::pbesinst_substitution_function;
using detail::pbesinst_rewriter;

/// \brief Stream operator
/// \param out An output stream
/// \param sigma A pbesinst substitution function
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const pbesinst_substitution_function& sigma)
{
  for (pbesinst_substitution_function::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
  {
    out << "  " << data::pp(i->first) << " -> " << data::pp(i->second) << std::endl;
  }
  return out;
}

/// \brief Creates a substitution function for the pbesinst rewriter.
/// \param v A sequence of data variables
/// \param e A sequence of data expressions
/// \return The substitution that maps the i-th element of \p v to the i-th element of \p e
inline
pbesinst_substitution_function make_pbesinst_substitution(data::variable_list v, data::data_expression_list e)
{
  assert(v.size() == e.size());
  pbesinst_substitution_function sigma;
  data::variable_list::iterator i = v.begin();
  data::data_expression_list::iterator j = e.begin();

  for (; i != v.end(); ++i, ++j)
  {
    sigma[*i] = *j;
  }
  return sigma;
}

/// \brief Algorithm class for the pbesinst instantiation algorithm.
class pbesinst_algorithm
{
  protected:
    /// \brief The rewriter.
    pbesinst_rewriter R;

    /// \brief The number of generated equations.
    int m_equation_count;

    /// \brief Propositional variable instantiations that need to be handled.
    std::set<propositional_variable_instantiation> todo;

    /// \brief Propositional variable instantiations that have been handled.
    std::set<propositional_variable_instantiation> done;

    /// \brief Data structure for storing the result. E[i] corresponds to the equations
    /// generated from the i-th PBES equation.
    std::vector<std::vector<pbes_equation> > E;

    /// \brief The initial value.
    propositional_variable_instantiation init;

    /// \brief A lookup map for PBES equations.
    std::map<core::identifier_string, int> equation_index;

    /// \brief Print the equations to standard out.
    bool m_print_equations;

    /// \brief Prints a log message for every 1000-th equation
    std::string print_equation_count(size_t size) const
    {
      if (size > 0 && size % 1000 == 0)
      {
        std::ostringstream out;
        out << "Generated " << size << " BES equations" << std::endl;
        return out.str();
      }
      return "";
    }

  public:

    /// \brief Constructor.
    /// \param data_spec A data specification
    /// \param rewriter_strategy A strategy for the data rewriter
    /// \param print_equations If true, the generated equations are printed
    /// \param print_rewriter_output If true, invocations of the rewriter are printed
    pbesinst_algorithm(data::data_specification const& data_spec,
                       data::rewriter::strategy rewriter_strategy = data::jitty,
                       bool print_equations = false,
                       bool print_rewriter_output = false
                      )
      : R(data_spec, rewriter_strategy, print_rewriter_output),
        m_equation_count(0),
        m_print_equations(print_equations)
    {}

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    /// \param p A PBES
    void run(pbes<>& p)
    {
      pbes_system::detail::instantiate_global_variables(p);

      // initialize equation_index and E
      int eqn_index = 0;
      for (std::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        equation_index[i->variable().name()] = eqn_index++;
        E.push_back(std::vector<pbes_equation>());
      }
      pbes_expression_with_propositional_variables Xinit = R(p.initial_state());
      assert(Xinit.propositional_variables().size() == 1);
      init = Xinit;
      todo.insert(Xinit.propositional_variables().front());
      while (!todo.empty())
      {
        propositional_variable_instantiation X = *todo.begin();
        todo.erase(todo.begin());
        done.insert(X);
        propositional_variable_instantiation X_e = R.rename(X);
        int index = equation_index[X.name()];
        const pbes_equation& eqn = p.equations()[index];
        pbesinst_substitution_function sigma = make_pbesinst_substitution(eqn.variable().parameters(), X.parameters());
        pbes_expression phi = eqn.formula();
        pbes_expression_with_propositional_variables psi_e = R(phi, sigma);
        for (propositional_variable_instantiation_list::iterator i = psi_e.propositional_variables().begin(); i != psi_e.propositional_variables().end(); ++i)
        {
          if (done.find(*i) == done.end())
          {
            todo.insert(*i);
          }
        }
        pbes_equation new_eqn(eqn.symbol(), propositional_variable(X_e.name(), data::variable_list()), psi_e);
        if (m_print_equations)
        {
          mCRL2log(log::info) << pbes_system::pp(eqn.symbol()) << " " << pbes_system::pp(X_e) << " = " << pbes_system::pp(psi_e) << std::endl;
        }
        E[index].push_back(new_eqn);
        mCRL2log(log::verbose) << print_equation_count(++m_equation_count);
        detail::check_bes_equation_limit(m_equation_count);
      }
    }

    /// \brief Returns the computed bes in pbes format
    /// \return The computed bes in pbes format
    pbes<> get_result()
    {
      pbes<> result;
      for (std::vector<std::vector<pbes_equation> >::iterator i =  E.begin(); i != E.end(); ++i)
      {
        result.equations().insert(result.equations().end(), i->begin(), i->end());
      }
      result.initial_state() = init;
      return result;
    }

    /// \brief Returns the flag for printing the generated bes equations
    /// \return The flag for printing the generated bes equations
    bool& print_equations()
    {
      return m_print_equations;
    }

    /// \brief Returns the flag for printing rewriter invocations
    /// \return The flag for printing rewriter invocations
    pbesinst_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_ALGORITHM_H
