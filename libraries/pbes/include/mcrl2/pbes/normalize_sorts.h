// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/normalize_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_NORMALIZE_SORTS_H
#define MCRL2_PBES_NORMALIZE_SORTS_H

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

template <typename T>
void normalize_sorts(T& x,
                     const data::sort_specification& sortspec,
                     typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
                    )
{
  core::make_update_apply_builder<pbes_system::sort_expression_builder>(data::detail::normalize_sorts_function(sortspec)).update(x);
}

template <typename T>
T normalize_sorts(const T& x,
                  const data::sort_specification& sortspec,
                  typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
                 )
{
  return core::make_update_apply_builder<pbes_system::sort_expression_builder>(data::detail::normalize_sorts_function(sortspec)).apply(x);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_NORMALIZE_SORTS_H
