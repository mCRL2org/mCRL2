// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/negate_variables.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_NEGATE_VARIABLES_H
#define MCRL2_MODAL_FORMULA_NEGATE_VARIABLES_H

#include "mcrl2/core/builder.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2
{

namespace state_formulas
{

namespace detail
{

/// Visitor that negates propositional variable instantiations with a given name.
template <typename Derived>
struct state_variable_negator: public state_formulas::state_formula_builder<Derived>
{
  typedef state_formulas::state_formula_builder<Derived> super;
  using super::apply;

  core::identifier_string m_name;

  state_variable_negator(const core::identifier_string& name)
    : m_name(name)
  {}

  /// \brief Visit variable node.
  /// \param x A term.
  /// \return The result of visiting the node.
  state_formula apply(const variable& x)
  {
    if (x.name() == m_name)
    {
      return state_formulas::not_(x);
    }
    return x;
  }
};

} // namespace detail

inline
/// \brief Negates variable instantiations in a state formula with a given name.
/// \param name The name of the variables that should be negated.
/// \param x The state formula. 
state_formula negate_variables(const core::identifier_string& name, const state_formula& x)
{
  return core::make_apply_builder_arg1<detail::state_variable_negator>(name).apply(x);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_NEGATE_VARIABLES_H
