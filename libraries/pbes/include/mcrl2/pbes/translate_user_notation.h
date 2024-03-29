// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TRANSLATE_USER_NOTATION_H
#define MCRL2_PBES_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/translate_user_notation.h"
#include "mcrl2/pbes/builder.h"

namespace mcrl2
{

namespace pbes_system
{

template <typename T>
void translate_user_notation(T& x,
                             typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
                            )
{
  core::make_update_apply_builder<pbes_system::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x,
                          typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = 0
                         )
{
  T result;
  core::make_update_apply_builder<pbes_system::data_expression_builder>(data::detail::translate_user_notation_function()).apply(result, x);
  return result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRANSLATE_USER_NOTATION_H
