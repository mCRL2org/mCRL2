// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/index_traits.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_INDEX_TRAITS_H
#define MCRL2_PROCESS_INDEX_TRAITS_H

#include "mcrl2/data/index_traits.h"
#include "mcrl2/process/process_identifier.h"

namespace mcrl2 {

namespace process {

inline
void on_create_process_identifier(const atermpp::aterm& t)
{
  const process_identifier& p = atermpp::aterm_cast<const process_identifier>(t);
  core::index_traits<process_identifier, process_identifier_key_type>::insert(std::make_pair(p.name(), p.variables()));
}

inline
void on_delete_process_identifier(const atermpp::aterm& t)
{
  const process_identifier& p = atermpp::aterm_cast<const process_identifier>(t);
  core::index_traits<process_identifier, process_identifier_key_type>::erase(std::make_pair(p.name(), p.variables()));
}

inline
void register_process_identifier_hooks()
{
  add_creation_hook(core::detail::function_symbol_ProcVarId(), on_create_process_identifier);
  add_deletion_hook(core::detail::function_symbol_ProcVarId(), on_delete_process_identifier);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_INDEX_TRAITS_H
