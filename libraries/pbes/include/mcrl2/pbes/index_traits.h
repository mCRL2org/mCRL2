// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/index_traits.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_INDEX_TRAITS_H
#define MCRL2_PBES_INDEX_TRAITS_H

#include "mcrl2/core/index_traits.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

#ifdef MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP
namespace std {

template<>
struct hash<mcrl2::pbes_system::propositional_variable_instantiation>
{
  std::size_t operator()(const mcrl2::pbes_system::propositional_variable_instantiation& x) const
  {
    return mcrl2::core::hash_value(x.name(), x.variables());
  }
};

}
#endif // MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

typedef std::pair<core::identifier_string, data::data_expression_list> propositional_variable_key_type;

inline
void on_create_propositional_variable_instantiation(const atermpp::aterm& t)
{
  const pbes_system::propositional_variable_instantiation& v = atermpp::aterm_cast<const pbes_system::propositional_variable_instantiation>(t);
  core::index_traits<pbes_system::propositional_variable_instantiation, propositional_variable_key_type>::insert(std::make_pair(v.name(), v.parameters()));
}

inline
void on_delete_propositional_variable_instantiation(const atermpp::aterm& t)
{
  const pbes_system::propositional_variable_instantiation& v = atermpp::aterm_cast<const pbes_system::propositional_variable_instantiation>(t);
  core::index_traits<pbes_system::propositional_variable_instantiation, propositional_variable_key_type>::erase(std::make_pair(v.name(), v.parameters()));
}

inline
void register_propositional_variable_instantiation_hooks()
{
  add_creation_hook(core::detail::function_symbol_PropVarInst(), on_create_propositional_variable_instantiation);
  add_deletion_hook(core::detail::function_symbol_PropVarInst(), on_delete_propositional_variable_instantiation);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_INDEX_TRAITS_H
