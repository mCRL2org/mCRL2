// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/action_formula_accessors.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_ACTION_FORMULA_ACCESSORS_H
#define MCRL2_MODAL_FORMULA_DETAIL_ACTION_FORMULA_ACCESSORS_H

#include "mcrl2/modal_formula/action_formula.h"

namespace mcrl2
{

namespace action_formulas
{

namespace detail
{

namespace accessors
{
/// \brief Returns the parameters of an action formula
/// \param t An action formula
/// \return The parameters of an action formula
inline
const lps::action_list& mult_params(const action_formula& t)
{
  assert(core::detail::gsIsMultAct(t));
  return atermpp::aterm_cast<const lps::action_list>(t[0]);
}

/// \brief Returns the action formula argument of an expression of type not, at, exists or forall.
/// \param t An action formula
/// \return The action formula argument of an expression of type not, at, exists or forall.
inline
const action_formula& arg(const action_formula& t)
{
  if (core::detail::gsIsActNot(t) || core::detail::gsIsActAt(t))
  {
    return core::static_down_cast<const action_formula&>(t[0]);
  }
  assert(core::detail::gsIsActExists(t) || core::detail::gsIsActForall(t));
  return core::static_down_cast<const action_formula&>(t[1]);
}

/// \brief Returns the left hand side of an expression of type and/or/imp
/// \param t An action formula
/// \return The left hand side of an expression of type and/or/imp
inline
const action_formula& left(const action_formula& t)
{
  assert(core::detail::gsIsActAnd(t) || core::detail::gsIsActOr(t) || core::detail::gsIsActImp(t));
  return core::static_down_cast<const action_formula&>(t[0]);
}

/// \brief Returns the right hand side of an expression of type and/or/imp.
/// \param t An action formula
/// \return The right hand side of an expression of type and/or/imp.
inline
const action_formula& right(const action_formula& t)
{
  assert(core::detail::gsIsActAnd(t) || core::detail::gsIsActOr(t) || core::detail::gsIsActImp(t));
  return core::static_down_cast<const action_formula&>(t[1]);
}

/// \brief Returns the variables of a quantification expression
/// \param t An action formula
/// \return The variables of a quantification expression
inline
const data::variable_list& var(const action_formula& t)
{
  assert(core::detail::gsIsActExists(t) || core::detail::gsIsActForall(t));
  return atermpp::aterm_cast<const data::variable_list>(t[0]);
}

/// \brief Returns the time of an at expression
/// \param t An action formula
/// \return The time of an at expression
inline
const data::data_expression& time(const action_formula& t)
{
  assert(core::detail::gsIsActAt(t));
  return core::static_down_cast<const data::data_expression&>(t[1]);
}

} // namespace accessors

} // namespace detail

} // namespace action_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_ACTION_FORMULA_ACCESSORS_H
