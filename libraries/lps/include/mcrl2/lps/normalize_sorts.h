// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/normalize_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_NORMALIZE_SORTS_H
#define MCRL2_LPS_NORMALIZE_SORTS_H

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2
{

namespace lps
{

template <typename T>
void normalize_sorts(T& x,
                     const data::sort_specification& sortspec,
                     typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value >::type* = nullptr
                    )
{
  core::make_update_apply_builder<lps::sort_expression_builder>(data::detail::normalize_sorts_function(sortspec)).update(x);
}

template <typename T>
T normalize_sorts(const T& x,
                  const data::sort_specification& sortspec,
                  typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                 )
{
  T result;
  core::make_update_apply_builder<lps::sort_expression_builder>(data::detail::normalize_sorts_function(sortspec)).apply(result, x);
  return result;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_NORMALIZE_SORTS_H
