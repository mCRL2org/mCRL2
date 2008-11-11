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
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/detail/pbes2bes_rewriter.h"

#ifndef MCRL2_PBES_PBES2BES_ALGORITHM_H
#define MCRL2_PBES_PBES2BES_ALGORITHM_H

/// \cond INTERNAL_DOCS
// Some hacking is needed to define vectors of vectors containing ATerms...
namespace atermpp {
template <>
struct aterm_traits<std::vector<mcrl2::pbes_system::pbes_equation> >
{
  static void mark(const std::vector<mcrl2::pbes_system::pbes_equation>& t)
  {
    for (std::vector<mcrl2::pbes_system::pbes_equation>::const_iterator i = t.begin(); i != t.end(); ++i)
    {
      aterm_traits<mcrl2::pbes_system::pbes_equation>::mark(*i);
    }
  }
};
} // namespace atermpp
/// \endcond

namespace mcrl2 {

namespace pbes_system {

  using detail::pbes2bes_substitution_function;
  using detail::pbes2bes_rewriter;

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
      /// The rewriter.
      pbes2bes_rewriter R;

      /// Propositional variable instantiations that need to be handled.
      atermpp::vector<propositional_variable_instantiation> todo;
        
      /// Propositional variable instantiations that have been handled.
      atermpp::set<propositional_variable_instantiation> done;
        
      /// Data structure for storing the result. E[i] corresponds to the equations
      /// generated from the i-th PBES equation.
      atermpp::vector<std::vector<pbes_equation> > E;

      /// The initial value.
      propositional_variable_instantiation init;

      /// A lookup map for PBES equations.
      std::map<core::identifier_string, int> equation_index;

    public:
      pbes2bes_algorithm(data::data_specification data_spec)
        : R(data_spec)
      {}

      void run(const pbes<>& p)
      {
        // initialize equation_index and E
        int eqn_index = 0;
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          equation_index[i->variable().name()] = eqn_index++;
          E.push_back(std::vector<pbes_equation>());
        }
        pbes_expression_with_propositional_variables Xinit = R(p.initial_state());
        assert(Xinit.propositional_variables().size() == 1);
        init = Xinit;
        todo.push_back(Xinit.propositional_variables().front());
        while (!todo.empty())
        {
          propositional_variable_instantiation X = todo.back();
          todo.pop_back();
          done.insert(X);
          propositional_variable_instantiation X_e = R(X);
          int index = equation_index[X.name()];
          const pbes_equation& eqn = p.equations()[index];
          pbes2bes_substitution_function sigma = make_pbes2bes_substitution(eqn.variable().parameters(), X.parameters());
          pbes_expression phi = eqn.formula();
          pbes_expression_with_propositional_variables psi_e = R(phi, sigma);
          for (propositional_variable_instantiation_list::iterator i = psi_e.propositional_variables().begin(); i != psi_e.propositional_variables().end(); ++i)
          {
            if (done.find(*i) == done.end())
            {
              todo.push_back(*i);
            }
          }         
          pbes_equation new_eqn(eqn.symbol(), propositional_variable(X_e.name(), data::data_variable_list()), psi_e);
          E[index].push_back(new_eqn);
        }
      }
      
      pbes<> get_result()
      {
        pbes<> result;
        for (atermpp::vector<std::vector<pbes_equation> >::iterator i =  E.begin(); i != E.end(); ++i)
        {
          result.equations().insert(result.equations().end(), i->begin(), i->end());
        }
        result.initial_state() = init;
        return result;
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES2BES_ALGORITHM_H
