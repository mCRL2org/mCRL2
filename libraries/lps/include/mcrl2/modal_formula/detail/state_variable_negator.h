// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_variable_negator.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_STATE_VARIABLE_NEGATOR_H
#define MCRL2_MODAL_FORMULA_DETAIL_STATE_VARIABLE_NEGATOR_H

#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/state_formula_builder.h"

namespace mcrl2 {

namespace state_formulas {

namespace detail {

/// Visitor that negates propositional variable instantiations with a given name.
struct state_variable_negator: public mcrl2::state_formulas::state_formula_builder<>
{
  core::identifier_string m_name;

  state_variable_negator(const core::identifier_string& name)
    : m_name(name)
  {}

  /// \brief Visit propositional_variable node
  /// \param x A term
  /// \return The result of visiting the node
  state_formula visit_variable(const variable& x)
  {
    // TODO: why is this cast needed???
    ATermAppl tmp = x;
    state_formula tmp1(tmp);

    if (x.name() == m_name)
    {
      return state_formulas::not_(tmp1);
    }
    return tmp1;
  }
};

inline
/// \brief Negates propositional variable instantiations in a pbes expression.
/// \param name The name of the variables that should be negated
state_formula negate_propositional_variable(const core::identifier_string& name, const state_formula& x)
{
  state_variable_negator visitor(name);
  return visitor.visit(x);
}

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_STATE_VARIABLE_NEGATOR_H
