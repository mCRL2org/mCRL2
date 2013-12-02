// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/io.h
/// \brief add your file description here.

#ifndef MCRL2_BES_DETAIL_IO_H
#define MCRL2_BES_DETAIL_IO_H

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/core/index_traits.h"

namespace mcrl2 {

namespace bes {

namespace detail {

// transforms BooleanVariable to BooleanVariableNoIndex
struct index_remover
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_BooleanVariable())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_BooleanVariableNoIndex(), x.begin(), --x.end());
    }
    return x;
  }
};

// transforms BooleanVariableNoIndex to BooleanVariable
struct index_adder
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_BooleanVariableNoIndex())
    {
      const bes::boolean_variable& y = atermpp::aterm_cast<const bes::boolean_variable>(x);
      std::size_t index = core::index_traits<bes::boolean_variable, bes::boolean_variable_key_type, 1>::insert(y.name());
      return atermpp::aterm_appl(core::detail::function_symbol_BooleanVariable(), x[0], atermpp::aterm_int(index));
    }
    return x;
  }
};

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

} // namespace detail

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_DETAIL_IO_H
