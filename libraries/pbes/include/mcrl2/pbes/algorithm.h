// Author(s): Jan Friso Groote, Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/algorithm.h
/// \brief Basic algorithms of the pbes library.

#ifndef MCRL2_PBES_ALGORITHM_H
#define MCRL2_PBES_ALGORITHM_H

#include "mcrl2/atermpp/set.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
namespace detail {

  inline
  void split_and_impl(const pbes_expression& expr, atermpp::set<pbes_expression>& result)
  {
    if (pbes_expr::is_and(expr))
    {
      split_and_impl(pbes_expr::lhs(expr), result);
      split_and_impl(pbes_expr::rhs(expr), result);
    }
    else
    {
      result.insert(expr);
    }
  }
  
  inline
  void split_or_impl(const pbes_expression& expr, atermpp::set<pbes_expression>& result)
  {
    if (pbes_expr::is_and(expr))
    {
      split_or_impl(pbes_expr::lhs(expr), result);
      split_or_impl(pbes_expr::rhs(expr), result);
    }
    else
    {
      result.insert(expr);
    }
  }

} // namespace detail
/// \endcond

  /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a 
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main 
  /// function symbol.
  inline
  atermpp::set<pbes_expression> split_and(const pbes_expression& expr)
  {
    atermpp::set<pbes_expression> result;
    detail::split_and_impl(expr, result);
    return result;
  }
  
  /// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a 
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main 
  /// function symbol.
  inline
  atermpp::set<pbes_expression> split_or(const pbes_expression& expr)
  {
    atermpp::set<pbes_expression> result;
    detail::split_or_impl(expr, result);
    return result;
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHM_H
