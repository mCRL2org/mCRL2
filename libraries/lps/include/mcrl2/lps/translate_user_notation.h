// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/translate_user_notation.h
/// \brief Translates user notation in the data expressions of an LPS.

#ifndef MCRL2_LPS_TRANSLATE_USER_NOTATION_H
#define MCRL2_LPS_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/translate_user_notation.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2::lps
{

template <typename T>
  requires(!std::is_base_of_v<atermpp::aterm, T>)
void translate_user_notation(T& x)
{
  core::make_update_apply_builder<lps::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
  requires(std::is_base_of_v<atermpp::aterm, T>)
T translate_user_notation(const T& x)
{
  T result;
  core::make_update_apply_builder<lps::data_expression_builder>(data::detail::translate_user_notation_function()).apply(result, x);
  return result;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_TRANSLATE_USER_NOTATION_H
