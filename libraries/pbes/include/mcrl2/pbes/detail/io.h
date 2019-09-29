// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/io.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_IO_H
#define MCRL2_PBES_DETAIL_IO_H

#include "mcrl2/data/detail/io.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// transforms DataVarId to DataVarIdNoIndex
// transforms OpId to OpIdNoIndex
// transforms PropVarInst to PropVarInstNoIndex
atermpp::aterm_appl remove_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
  }
  else if (x.function() == core::detail::function_symbol_OpId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
  }
  else if (x.function() == core::detail::function_symbol_PropVarInst())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_PropVarInstNoIndex(), x.begin(), --x.end());
  }
  return x;
}

// transforms DataVarIdNoIndex to DataVarId
// transforms OpIdNoIndex to OpId
// transforms PropVarInstNoIndex to PropVarInst
atermpp::aterm_appl add_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
  {
    const data::variable& y = atermpp::down_cast<const data::variable>(x);
    std::size_t index = core::index_traits<data::variable, data::variable_key_type, 2>::insert(std::make_pair(y.name(), y.sort()));
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), x[0], x[1], atermpp::aterm_int(index));
  }
  else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
  {
    const data::function_symbol& y = atermpp::down_cast<const data::function_symbol>(x);
    std::size_t index = core::index_traits<data::function_symbol, data::function_symbol_key_type, 2>::insert(std::make_pair(y.name(), y.sort()));
    return atermpp::aterm_appl(core::detail::function_symbol_OpId(), x[0], x[1], atermpp::aterm_int(index));
  }
  else if (x.function() == core::detail::function_symbol_PropVarInstNoIndex())
  {
    const pbes_system::propositional_variable_instantiation& y = atermpp::down_cast<const pbes_system::propositional_variable_instantiation>(x);
    std::size_t index = core::index_traits<propositional_variable_instantiation, propositional_variable_key_type, 2>::insert(std::make_pair(y.name(), y.parameters()));
    return atermpp::aterm_appl(core::detail::function_symbol_PropVarInst(), x[0], x[1], atermpp::aterm_int(index));
  }
  return x;
}


} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_IO_H
