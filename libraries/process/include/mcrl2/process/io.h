// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/io.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IO_H
#define MCRL2_PROCESS_IO_H

#include "mcrl2/data/index_traits.h"
#include "mcrl2/data/io.h"
#include "mcrl2/process/process_identifier.h"

namespace mcrl2 {

namespace process {

namespace detail {

// TODO: reuse code in index_remover/index_adder

// transforms DataVarId to DataVarIdNoIndex
// transforms OpId to OpIdNoIndex
// transforms ProcVarId to ProcVarIdNoIndex
struct index_remover
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_DataVarId())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
    }
    else if (x.function() == core::detail::function_symbol_OpId())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
    }
    else if (x.function() == core::detail::function_symbol_ProcVarId())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_ProcVarIdNoIndex(), x.begin(), --x.end());
    }
    return x;
  }
};

// transforms DataVarIdNoIndex to DataVarId
// transforms OpIdNoIndex to OpId
// transforms ProcVarIdNoIndex to ProcVarId
struct index_adder
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
    {
      const data::variable& y = atermpp::aterm_cast<const data::variable>(x);
      std::size_t index = core::index_traits<data::variable, data::variable_key_type>::insert(std::make_pair(y.name(), y.variables()));
      return atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), x[0], x[1], atermpp::aterm_int(index));
    }
    else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
    {
      const data::function_symbol& y = atermpp::aterm_cast<const data::function_symbol>(x);
      std::size_t index = core::index_traits<data::function_symbol, data::function_symbol_key_type>::insert(std::make_pair(y.name(), y.variables()));
      return atermpp::aterm_appl(core::detail::function_symbol_OpId(), x[0], x[1], atermpp::aterm_int(index));
    }
    else if (x.function() == core::detail::function_symbol_ProcVarIdNoIndex())
    {
      const process::process_identifier& y = atermpp::aterm_cast<const process::process_identifier>(x);
      std::size_t index = core::index_traits<process::process_identifier, process::process_identifier_key_type>::index(y);
      return atermpp::aterm_appl(core::detail::function_symbol_ProcVarId(), x[0], x[1], atermpp::aterm_int(index));
    }
    return x;
  }
};

} // namespace detail

inline
atermpp::aterm add_index(const atermpp::aterm& x)
{
  return atermpp::bottom_up_replace(x, detail::index_adder());
}

inline
atermpp::aterm remove_index(const atermpp::aterm& x)
{
  return atermpp::bottom_up_replace(x, detail::index_remover());
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_IO_H
