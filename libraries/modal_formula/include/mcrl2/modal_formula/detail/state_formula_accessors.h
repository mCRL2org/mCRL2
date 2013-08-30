// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_formula_accessors.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_STATE_FORMULA_ACCESSORS_H
#define MCRL2_MODAL_FORMULA_DETAIL_STATE_FORMULA_ACCESSORS_H

#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2
{

namespace state_formulas
{

namespace detail
{

namespace accessors
{

/// \brief Returns the argument of a data expression
/// \param t A modal formula
/// \return The argument of a data expression
inline
data::data_expression val(const state_formula& x)
{
  return data::data_expression(x);
}

/// \brief Returns the state formula argument of an expression of type
/// not, mu, nu, exists, forall, must or may.
/// \param t A modal formula
/// \return The state formula argument of an expression of type
inline
const state_formula& arg(const state_formula& t)
{
  if (state_formulas::is_not(t))
  {
    return core::static_down_cast<const state_formula&>(t[0]);
  }
  if (state_formulas::is_mu(t) || state_formulas::is_nu(t))
  {
    return core::static_down_cast<const state_formula&>(t[2]);
  }
  assert(state_formulas::is_exists(t) ||
         state_formulas::is_forall(t) ||
         state_formulas::is_must(t)   ||
         state_formulas::is_may(t)
        );
  return core::static_down_cast<const state_formula&>(t[1]);
}

/// \brief Returns the left hand side of an expression of type and/or/imp
/// \param t A modal formula
/// \return The left hand side of an expression of type and/or/imp
inline
const state_formula& left(const state_formula& t)
{
  assert(state_formulas::is_and(t) || state_formulas::is_or(t) || state_formulas::is_imp(t));
  return core::static_down_cast<const state_formula&>(t[0]);
}

/// \brief Returns the right hand side of an expression of type and/or/imp.
/// \param t A modal formula
/// \return The right hand side of an expression of type and/or/imp.
inline
const state_formula& right(const state_formula& t)
{
  assert(state_formulas::is_and(t) || state_formulas::is_or(t) || state_formulas::is_imp(t));
  return core::static_down_cast<const state_formula&>(t[1]);
}

/// \brief Returns the variables of a quantification expression
/// \param t A modal formula
/// \return The variables of a quantification expression
inline
const data::variable_list& var(const state_formula& t)
{
  assert(state_formulas::is_exists(t) || state_formulas::is_forall(t));
  return atermpp::aterm_cast<const data::variable_list>(t[0]);
}

/// \brief Returns the time of a delay or yaled expression
/// \param t A modal formula
/// \return The time of a delay or yaled expression
inline
data::data_expression time(const state_formula& t)
{
  assert(state_formulas::is_delay_timed(t) || state_formulas::is_yaled_timed(t));
  return atermpp::aterm_cast<data::data_expression>(t[0]);
}

/// \brief Returns the name of a variable expression
/// \param t A modal formula
/// \return The name of a variable expression
inline
const core::identifier_string& name(const state_formula& t)
{
  assert(state_formulas::is_variable(t) ||
         state_formulas::is_mu(t)  ||
         state_formulas::is_nu(t)
        );
  return atermpp::aterm_cast<core::identifier_string>(atermpp::arg1(t));
}

/// \brief Returns the parameters of a variable expression
/// \param t A modal formula
/// \return The parameters of a variable expression
inline
const data::data_expression_list& param(const state_formula& t)
{
  assert(state_formulas::is_variable(t));
  return atermpp::aterm_cast<data::data_expression_list>(t[1]);
}

/// \brief Returns the parameters of a mu or nu expression
/// \param t A modal formula
/// \return The parameters of a mu or nu expression
inline
const data::assignment_list& ass(const state_formula& t)
{
  assert(state_formulas::is_mu(t) || state_formulas::is_nu(t));
  return atermpp::aterm_cast<const data::assignment_list>(t[1]);
}

/// \brief Returns the regular formula of a must or may expression
/// \param t A modal formula
/// \return The regular formula of a must or may expression
inline
regular_formulas::regular_formula act(const state_formula& t)
{
  assert(state_formulas::is_must(t) || state_formulas::is_may(t));
  return core::static_down_cast<const regular_formulas::regular_formula&>(t[0]);
}

} // namespace accessors

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_STATE_FORMULA_ACCESSORS_H
