// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_TRANSLATE_USER_NOTATION_H
#define MCRL2_LPS_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/translate_user_notation.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2::lps
{

template <typename T>
void translate_user_notation(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  core::make_update_apply_builder<lps::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<lps::data_expression_builder>(data::detail::translate_user_notation_function()).apply(result, x);
  return result;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_TRANSLATE_USER_NOTATION_H
