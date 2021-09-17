// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/io.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_IO_H
#define MCRL2_DATA_DETAIL_IO_H

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_io.h"

#include "mcrl2/data/index_traits.h"

namespace mcrl2 {

namespace data {

namespace detail {

// transforms DataVarId to DataVarIdNoIndex
// transforms OpId to OpIdNoIndex
inline atermpp::aterm_appl remove_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
  }
  else if (x.function() == core::detail::function_symbol_OpId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
  }
  return x;
}

// transforms DataVarIdNoIndex to DataVarId
// transforms OpIdNoIndex to OpId
inline atermpp::aterm_appl add_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
  {
    const data::variable& y = reinterpret_cast<const data::variable&>(x);
    return variable(y.name(), y.sort()); 
  }
  else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
  {
    const data::function_symbol& y = reinterpret_cast<const data::function_symbol&>(x);
    return function_symbol(y.name(), y.sort());
  }
  return x;
}

inline
atermpp::aterm add_index(const atermpp::aterm& x)
{
  return atermpp::bottom_up_replace(x, detail::add_index_impl);
}

inline
atermpp::aterm remove_index(const atermpp::aterm& x)
{
  return atermpp::bottom_up_replace(x, detail::remove_index_impl);
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_IO_H
