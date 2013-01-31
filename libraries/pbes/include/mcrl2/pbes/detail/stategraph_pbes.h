// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H

#include <cassert>
#include <iostream>
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

typedef atermpp::vector<std::pair<propositional_variable_instantiation, pbes_expression> > predicate_variable_vector;

class stategraph_equation: public pbes_equation
{
  protected:
    predicate_variable_vector m_predvars;
    std::vector<data::variable> m_parameters;

  public:
    stategraph_equation(const pbes_equation& eqn)
      : pbes_equation(eqn)
    {
      pbes_system::detail::guard_traverser f;
      f(eqn.formula());
      if (!f.expression_stack.back().is_simple())
      {
        m_predvars = f.expression_stack.back().guards;
      }
      data::variable_list params = variable().parameters();
      m_parameters = std::vector<data::variable>(params.begin(), params.end());
    }

    const std::vector<data::variable>& parameters() const
    {
    	return m_parameters;
    }

    const predicate_variable_vector& predicate_variables() const
    {
    	return m_predvars;
    }

    predicate_variable_vector& predicate_variables()
    {
    	return m_predvars;
    }

    // // computes the equation with the implications replaced by new_implication
    // pbes_equation apply_implication(const pbes_expression& new_implications) const
    // {
    //   pbes_expression phi = new_implications;
    //   phi = and_(m_h, phi);
    //
    //   // apply quantifiers
    //   for (atermpp::vector<pfnf_quantifier>::const_reverse_iterator i = m_quantifiers.rbegin(); i != m_quantifiers.rend(); ++i)
    //   {
    //     phi = i->apply(phi);
    //   }
    //   return pbes_equation(m_symbol, m_X, phi);
    // }
};

// explicit representation of a pbes in STATEGRAPH format
class stategraph_pbes
{
  protected:
    data::data_specification m_data;
    atermpp::vector<stategraph_equation> m_equations;
    atermpp::set<data::variable> m_global_variables;
    pbes_expression m_initial_state;

  public:
    stategraph_pbes()
    {}

    /// \brief Constructor
    /// \pre The pbes p must be in STATEGRAPH format
    stategraph_pbes(const pbes<>& p)
      : m_data(p.data()), m_global_variables(p.global_variables()), m_initial_state(p.initial_state())
    {
      const atermpp::vector<pbes_equation>& equations = p.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        m_equations.push_back(stategraph_equation(*i));
      }
    }

    const atermpp::vector<stategraph_equation>& equations() const
    {
      return m_equations;
    }

    atermpp::vector<stategraph_equation>& equations()
    {
      return m_equations;
    }

    const atermpp::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    atermpp::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    const pbes_expression& initial_state() const
    {
      return m_initial_state;
    }

    const data::data_specification& data() const
    {
      return m_data;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
