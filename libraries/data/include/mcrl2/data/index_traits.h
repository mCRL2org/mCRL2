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

namespace mcrl2 {

namespace data {

inline
void on_create_function_symbol(const atermpp::aterm& t)
{
//  const data::function_symbol& v = atermpp::down_cast<const data::function_symbol>(t);
//  std::size_t i = core::index_traits<data::function_symbol, function_symbol_key_type, 2>::insert(std::make_pair(v.name(), v.sort()));
}

inline
void on_delete_function_symbol(const atermpp::aterm& t)
{
  const data::function_symbol& v = atermpp::down_cast<const data::function_symbol>(t);
#ifndef NDEBUG
  typedef core::index_traits<data::function_symbol, function_symbol_key_type, 2> function_symbol_index_trait_type;
  std::size_t i = function_symbol_index_trait_type::insert(std::make_pair(v.name(), v.sort()));
  assert(i==function_symbol_index_trait_type::index(v));
#endif
  core::index_traits<data::function_symbol, function_symbol_key_type, 2>::erase(std::make_pair(v.name(), v.sort()));
}

inline
void on_create_variable(const atermpp::aterm& t)
{
//  const data::variable& v = atermpp::down_cast<const data::variable>(t);
//  std::size_t i=core::index_traits<data::variable, variable_key_type, 2>::insert(std::make_pair(v.name(), v.sort()));
}

inline
void on_delete_variable(const atermpp::aterm& t)
{
  const data::variable& v = atermpp::down_cast<const data::variable>(t);
#ifndef NDEBUG
  typedef core::index_traits<data::variable, variable_key_type, 2> variable_index_trait_type;
  std::size_t i=variable_index_trait_type::insert(std::make_pair(v.name(), v.sort()));
  assert(i==variable_index_trait_type::index(v));
#endif
  core::index_traits<data::variable, variable_key_type, 2>::erase(std::make_pair(v.name(), v.sort()));
}

inline
void register_function_symbol_hooks()
{
//  add_creation_hook(core::detail::function_symbol_OpId(), on_create_function_symbol);
  add_deletion_hook(core::detail::function_symbol_OpId(), on_delete_function_symbol);
}

inline
void register_variable_hooks()
{
//  add_creation_hook(core::detail::function_symbol_DataVarId(), on_create_variable);
  add_deletion_hook(core::detail::function_symbol_DataVarId(), on_delete_variable);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INDEX_TRAITS_H
