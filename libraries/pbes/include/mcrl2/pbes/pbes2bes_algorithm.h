// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes2bes_algorithm.h
/// \brief Algorithm for instantiating a PBES.

#include <set>
#include <iostream>
#include "mcrl2/atermpp/set.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/detail/pbes2bes_rewriter.h"

#ifndef MCRL2_PBES_PBES2BES_ALGORITHM_H
#define MCRL2_PBES_PBES2BES_ALGORITHM_H

namespace mcrl2 {

namespace pbes_system {

  using detail::pbes2bes_substitution_function;
  using detail::pbes2bes_rewriter;

  std::ostream& operator<<(std::ostream& out, const pbes2bes_substitution_function& sigma)
  {
    for (pbes2bes_substitution_function::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
    {
      out << "  " << core::pp(i->first) << " -> " << core::pp(i->second) << std::endl;
    }
    return out;
  }

  /// Substitution function for the pbes2bes rewriter.
  inline
  pbes2bes_substitution_function make_pbes2bes_substitution(data::data_variable_list v, data::data_expression_list e)
  {
    assert(v.size() == e.size());
    pbes2bes_substitution_function sigma;
    data::data_variable_list::iterator i = v.begin();
    data::data_expression_list::iterator j = e.begin();

    for (; i != v.end(); ++i, ++j)
    {
      sigma[*i] = *j;
    }
    return sigma;
  }

  /// Class for instantiating a pbes.
  class pbes2bes_algorithm
  {
    protected:
      /// \brief The rewriter.
      pbes2bes_rewriter R;

      /// \brief The number of generated equations.
      int equation_count;

      /// Propositional variable instantiations that need to be handled.
      atermpp::set<propositional_variable_instantiation> todo;

      /// Propositional variable instantiations that have been handled.
      atermpp::set<propositional_variable_instantiation> done;

      /// Data structure for storing the result. E[i] corresponds to the equations
      /// generated from the i-th PBES equation.
      std::vector<atermpp::vector<pbes_equation> > E;

      /// \brief The initial value.
      propositional_variable_instantiation init;

      /// \brief A lookup map for PBES equations.
      std::map<core::identifier_string, int> equation_index;

      /// Print the equations to standard out.
      bool m_print_equations;

    public:
      pbes2bes_algorithm(data::data_specification data_spec, data::rewriter::strategy rewriter_strategy = data::rewriter::jitty, bool print_equations = false, bool print_rewriter_output = false)
        : R(data_spec, rewriter_strategy, print_rewriter_output), equation_count(0), m_print_equations(print_equations)
      {}

      void run(pbes<>& p)
      {
        if (!p.instantiate_free_variables())
        {
          core::gsErrorMsg("Instantiatiation of free variables failed!\n");
          throw mcrl2::runtime_error("exit!");
        }

        // initialize equation_index and E
        int eqn_index = 0;
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          equation_index[i->variable().name()] = eqn_index++;
          E.push_back(atermpp::vector<pbes_equation>());
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
          pbes2bes_substitution_function sigma = make_pbes2bes_substitution(eqn.variable().parameters(), X.parameters());
          pbes_expression phi = eqn.formula();
          pbes_expression_with_propositional_variables psi_e = R(phi, sigma);
          for (propositional_variable_instantiation_list::iterator i = psi_e.propositional_variables().begin(); i != psi_e.propositional_variables().end(); ++i)
          {
            if (done.find(*i) == done.end())
            {
              todo.insert(*i);
            }
          }
          pbes_equation new_eqn(eqn.symbol(), propositional_variable(X_e.name(), data::data_variable_list()), psi_e);
          if (m_print_equations)
          {
            std::cerr << core::pp(eqn.symbol()) << " " << core::pp(X_e) << " = " << core::pp(psi_e) << std::endl;
          }
          E[index].push_back(new_eqn);
          if (++equation_count % 1000 == 0)
          {
            core::gsVerboseMsg("At equation %d\n", equation_count);
          }
        }
      }

      pbes<> get_result()
      {
        pbes<> result;
        for (std::vector<atermpp::vector<pbes_equation> >::iterator i =  E.begin(); i != E.end(); ++i)
        {
          result.equations().insert(result.equations().end(), i->begin(), i->end());
        }
        result.initial_state() = init;
        return result;
      }

      bool& print_equations()
      {
        return m_print_equations;
      }

      pbes2bes_rewriter& rewriter()
      {
        return R;
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES2BES_ALGORITHM_H
