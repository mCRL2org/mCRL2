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
lps::action_list mult_params(action_formula t)
{
  assert(core::detail::gsIsMultAct(t));
  return lps::action_list(atermpp::list_arg1(t));
}

/// \brief Returns the action formula argument of an expression of type not, at, exists or forall.
/// \param t An action formula
/// \return The action formula argument of an expression of type not, at, exists or forall.
inline
action_formula arg(action_formula t)
{
  if (core::detail::gsIsActNot(t) || core::detail::gsIsActAt(t))
  {
    return atermpp::arg1(t);
  }
  assert(core::detail::gsIsActExists(t) || core::detail::gsIsActForall(t));
  return atermpp::arg2(t);
}

/// \brief Returns the left hand side of an expression of type and/or/imp
/// \param t An action formula
/// \return The left hand side of an expression of type and/or/imp
inline
action_formula left(action_formula t)
{
  assert(core::detail::gsIsActAnd(t) || core::detail::gsIsActOr(t) || core::detail::gsIsActImp(t));
  return atermpp::arg1(t);
}

/// \brief Returns the right hand side of an expression of type and/or/imp.
/// \param t An action formula
/// \return The right hand side of an expression of type and/or/imp.
inline
action_formula right(action_formula t)
{
  assert(core::detail::gsIsActAnd(t) || core::detail::gsIsActOr(t) || core::detail::gsIsActImp(t));
  return atermpp::arg2(t);
}

/// \brief Returns the variables of a quantification expression
/// \param t An action formula
/// \return The variables of a quantification expression
inline
data::variable_list var(action_formula t)
{
  assert(core::detail::gsIsActExists(t) || core::detail::gsIsActForall(t));
  return data::variable_list(t(0));
  /* return data::variable_list(
           atermpp::term_list_iterator< data::variable >(atermpp::list_arg1(t)),
           atermpp::term_list_iterator< data::variable >());  */
}

/// \brief Returns the time of an at expression
/// \param t An action formula
/// \return The time of an at expression
inline
data::data_expression time(action_formula t)
{
  assert(core::detail::gsIsActAt(t));
  return data::data_expression(atermpp::arg2(t));
}

} // namespace accessors

} // namespace detail

} // namespace action_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_ACTION_FORMULA_ACCESSORS_H
