// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/join.h
/// \brief add your file description here.

#ifndef MCRL2_BES_JOIN_H
#define MCRL2_BES_JOIN_H

#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/utilities/detail/join.h"

namespace mcrl2 {

namespace bes {

/// \brief Returns or applied to the sequence of boolean expressions [first, last)
/// \param first Start of a sequence of boolean expressions
/// \param last End of a sequence of of boolean expressions
/// \return Or applied to the sequence of boolean expressions [first, last)
template <typename FwdIt>
inline
boolean_expression join_or(FwdIt first, FwdIt last)
{
  typedef core::term_traits<boolean_expression> tr;
  return utilities::detail::join(first, last, tr::or_, tr::false_());
}

/// \brief Returns and applied to the sequence of boolean expressions [first, last)
/// \param first Start of a sequence of boolean expressions
/// \param last End of a sequence of of boolean expressions
/// \return And applied to the sequence of boolean expressions [first, last)
template <typename FwdIt>
inline
boolean_expression join_and(FwdIt first, FwdIt last)
{
  typedef core::term_traits<boolean_expression> tr;
  return utilities::detail::join(first, last, tr::and_, tr::true_());
}

/// \brief Splits a disjunction into a sequence of operands
/// Given a boolean expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A boolean expression
/// \return A sequence of operands
inline
std::set<boolean_expression> split_or(const boolean_expression& expr)
{
  using namespace accessors;
  std::set<boolean_expression> result;
  utilities::detail::split(expr, std::insert_iterator<std::set<boolean_expression> >(result, result.begin()), is_or, left, right);
  return result;
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a boolean expression of the form p1 && p2 && .... && pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
/// function symbol.
/// \param expr A boolean expression
/// \return A sequence of operands
inline
std::set<boolean_expression> split_and(const boolean_expression& expr)
{
  using namespace accessors;
  std::set<boolean_expression> result;
  utilities::detail::split(expr, std::insert_iterator<std::set<boolean_expression> >(result, result.begin()), is_and, left, right);
  return result;
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_JOIN_H
