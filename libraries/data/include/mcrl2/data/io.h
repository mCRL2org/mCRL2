// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/io.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_IO_H
#define MCRL2_DATA_IO_H

#define MCRL2_USE_INDEX_TRAITS

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/index_traits.h"

namespace mcrl2 {

namespace data {

namespace detail {

// DataVarIdIndex
inline
const atermpp::function_symbol& function_symbol_DataVarIdIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("DataVarIdIndex", 3);
  return f;
}

// OpIdIndex
inline
const atermpp::function_symbol& function_symbol_OpIdIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("OpIdIndex", 3);
  return f;
}

// transforms DataVarIdIndex to DataVarId
// transforms OpIdIndex to OpId
struct index_remover
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == function_symbol_DataVarIdIndex())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), x.begin(), --x.end());
    }
    else if (x.function() == function_symbol_OpIdIndex())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_OpId(), x.begin(), --x.end());
    }
    return x;
  }
};

// transforms DataVarId to DataVarIdIndex
// transforms OpId to OpIdIndex
struct index_adder
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_DataVarId())
    {
      const variable& v = atermpp::aterm_cast<const variable>(x);
      std::size_t index = core::index_traits<variable, variable_key_type>::index(variable_key_type(v.name(), v.sort()));
      return atermpp::aterm_appl(function_symbol_DataVarIdIndex(), x[0], x[1], atermpp::aterm_int(index));
    }
    else if (x.function() == core::detail::function_symbol_OpId())
    {
      const function_symbol& f = atermpp::aterm_cast<const function_symbol>(x);
      std::size_t index = core::index_traits<function_symbol, function_symbol_key_type>::index(function_symbol_key_type(f.name(), f.sort()));
      return atermpp::aterm_appl(function_symbol_OpIdIndex(), x[0], x[1], atermpp::aterm_int(index));
    }
    return x;
  }
};

} // namespace detail

inline
atermpp::aterm add_index(const atermpp::aterm& x)
{
  return atermpp::replace(x, detail::index_adder());
}

inline
atermpp::aterm remove_index(const atermpp::aterm& x)
{
  return atermpp::replace(x, detail::index_remover());
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_IO_H
