// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TRANSLATE_USER_NOTATION_H
#define MCRL2_MODAL_FORMULA_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/translate_user_notation.h"
#include "mcrl2/modal_formula/builder.h"

namespace mcrl2
{

namespace action_formulas
{

template <typename T>
void translate_user_notation(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<action_formulas::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<action_formulas::data_expression_builder>(data::detail::translate_user_notation_function()).apply(result, x);
  return result;
}

} // namespace action_formulas

namespace regular_formulas
{

template <typename T>
void translate_user_notation(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<regular_formulas::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<regular_formulas::data_expression_builder>(data::detail::translate_user_notation_function()).apply(result, x);
  return result;
}

} // namespace regular_formulas

namespace state_formulas
{

template <typename T>
void translate_user_notation(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<state_formulas::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<state_formulas::data_expression_builder>(data::detail::translate_user_notation_function()).apply(result, x);
  return result;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TRANSLATE_USER_NOTATION_H
