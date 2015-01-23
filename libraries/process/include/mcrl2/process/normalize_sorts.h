// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/normalize_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_NORMALIZE_SORTS_H
#define MCRL2_PROCESS_NORMALIZE_SORTS_H

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/builder.h"

namespace mcrl2
{

namespace process
{

template <typename T>
void normalize_sorts(T& x,
                     const data::data_specification& data_spec,
                     typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                    )
{
  core::make_update_apply_builder<process::sort_expression_builder>(data::detail::normalize_sorts_function(data_spec)).update(x);
}

template <typename T>
T normalize_sorts(const T& x,
                  const data::data_specification& data_spec,
                  typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                 )
{
  return core::make_update_apply_builder<process::sort_expression_builder>(data::detail::normalize_sorts_function(data_spec)).apply(x);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_NORMALIZE_SORTS_H
