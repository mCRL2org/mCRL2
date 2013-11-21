// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/index_traits.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_INDEX_TRAITS_H
#define MCRL2_DATA_INDEX_TRAITS_H

#include <functional>
#include <utility>

#include "mcrl2/core/index_traits.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/variable.h"

#ifdef MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

namespace std {

/// \brief hash specialization
template<>
struct hash<std::pair<mcrl2::core::identifier_string, mcrl2::data::sort_expression> >
{
  std::size_t operator()(const std::pair<mcrl2::core::identifier_string, mcrl2::data::sort_expression>& x) const
  {
    return mcrl2::core::hash_value(x.first, x.second);
  }
};

/// \brief hash specialization
template<>
struct hash<std::pair<mcrl2::core::identifier_string, mcrl2::data::variable_list> >
{
  std::size_t operator()(const std::pair<mcrl2::core::identifier_string, mcrl2::data::variable_list>& x) const
  {
    return mcrl2::core::hash_value(x.first, x.second);
  }
};

/// \brief hash specialization
template<>
struct hash<std::pair<mcrl2::core::identifier_string, mcrl2::data::data_expression_list> >
{
  std::size_t operator()(const std::pair<mcrl2::core::identifier_string, mcrl2::data::data_expression_list>& x) const
  {
    return mcrl2::core::hash_value(x.first, x.second);
  }
};

} // namespace std

#endif

namespace mcrl2 {

namespace data {

typedef std::pair<core::identifier_string, data::sort_expression> function_symbol_key_type;
typedef std::pair<core::identifier_string, data::sort_expression> variable_key_type;

inline
void on_create_function_symbol(const atermpp::aterm& t)
{
  const data::function_symbol& v = atermpp::aterm_cast<const data::function_symbol>(t);
  core::index_traits<data::function_symbol, function_symbol_key_type>::insert(std::make_pair(v.name(), v.sort()));
}

inline
void on_delete_function_symbol(const atermpp::aterm& t)
{
  const data::function_symbol& v = atermpp::aterm_cast<const data::function_symbol>(t);
  core::index_traits<data::function_symbol, function_symbol_key_type>::erase(std::make_pair(v.name(), v.sort()));
}

inline
void on_create_variable(const atermpp::aterm& t)
{
  const data::variable& v = atermpp::aterm_cast<const data::variable>(t);
  core::index_traits<data::variable, variable_key_type>::insert(std::make_pair(v.name(), v.sort()));
}

inline
void on_delete_variable(const atermpp::aterm& t)
{
  const data::variable& v = atermpp::aterm_cast<const data::variable>(t);
  core::index_traits<data::variable, variable_key_type>::erase(std::make_pair(v.name(), v.sort()));
}

inline
void register_function_symbol_hooks()
{
  add_creation_hook(core::detail::function_symbol_OpId(), on_create_function_symbol);
  add_deletion_hook(core::detail::function_symbol_OpId(), on_delete_function_symbol);
}

inline
void register_variable_hooks()
{
  add_creation_hook(core::detail::function_symbol_DataVarId(), on_create_variable);
  add_deletion_hook(core::detail::function_symbol_DataVarId(), on_delete_variable);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INDEX_TRAITS_H
