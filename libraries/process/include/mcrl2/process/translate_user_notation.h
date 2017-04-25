// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TRANSLATE_USER_NOTATION_H
#define MCRL2_PROCESS_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/translate_user_notation.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

template <typename T>
void translate_user_notation(T& x,
                             typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value >::type* = nullptr
                            )
{
  core::make_update_apply_builder<process::data_expression_builder>(data::detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x,
                          typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value >::type* = nullptr
                         )
{
  return core::make_update_apply_builder<process::data_expression_builder>(data::detail::translate_user_notation_function()).apply(x);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRANSLATE_USER_NOTATION_H
