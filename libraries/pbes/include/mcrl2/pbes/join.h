// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/join.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_JOIN_H
#define MCRL2_PBES_JOIN_H

#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/detail/join.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Returns or applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of of pbes expressions
/// \return Or applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
pbes_expression join_or(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, [](const pbes_expression& x, const pbes_expression& y) { return or_(x, y); }, false_());
}

/// \brief Returns and applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of of pbes expressions
/// \return And applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
pbes_expression join_and(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, [](const pbes_expression& x, const pbes_expression& y) { return and_(x, y); }, true_());
}

/// \brief Splits a disjunction into a sequence of operands
/// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PBES expression
/// \param split_data_expressions if true, both data and pbes disjunctions are
///        split, otherwise only pbes disjunctions are split.
/// \return A sequence of operands
inline
std::set<pbes_expression> split_or(const pbes_expression& expr, bool split_data_expressions = false)
{
  using namespace accessors;
  std::set<pbes_expression> result;

  if (split_data_expressions)
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), is_universal_or, data_left, data_right);
  }
  else
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), is_pbes_or, left, right);
  }

  return result;
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
/// function symbol.
/// \param expr A PBES expression
/// \param split_data_expressions if true, both data and pbes conjunctions are
///        split, otherwise only pbes conjunctions are split.
/// \return A sequence of operands
inline
std::set<pbes_expression> split_and(const pbes_expression& expr, bool split_data_expressions = false)
{
  using namespace accessors;
  std::set<pbes_expression> result;

  if (split_data_expressions)
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), is_universal_and, data_left, data_right);
  }
  else
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), is_pbes_and, left, right);
  }

  return result;
}

/// \brief Returns or applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of pbes expressions
/// \return Or applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
inline pbes_expression optimized_join_or(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, data::optimized_or<pbes_expression>, false_());
}

/// \brief Returns and applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of pbes expressions
/// \return And applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
inline pbes_expression optimized_join_and(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, data::optimized_and<pbes_expression>, true_());
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_JOIN_H
