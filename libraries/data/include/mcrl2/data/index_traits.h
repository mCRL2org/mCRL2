// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/index_traits.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_INDEX_TRAITS_H
#define MCRL2_DATA_INDEX_TRAITS_H

#include "mcrl2/data/function_symbol.h"

namespace mcrl2::data
{

inline
void on_delete_function_symbol(const atermpp::aterm& t)
{
  const data::function_symbol& v = atermpp::down_cast<const data::function_symbol>(t);
#ifndef NDEBUG
  using function_symbol_index_trait_type
      = atermpp::detail::index_traits<data::function_symbol, function_symbol_key_type, 2>;
  std::size_t i = function_symbol_index_trait_type::insert(std::make_pair(v.name(), v.sort()));
  assert(i==function_symbol_index_trait_type::index(v));
#endif
  atermpp::detail::index_traits<data::function_symbol, function_symbol_key_type, 2>::erase(std::make_pair(v.name(), v.sort()));
}

inline
void register_function_symbol_hooks()
{
  add_deletion_hook(core::detail::function_symbol_OpId(), on_delete_function_symbol);
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_INDEX_TRAITS_H
