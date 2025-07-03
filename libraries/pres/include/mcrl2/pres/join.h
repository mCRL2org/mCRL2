// Author(s): Jan Friso Groote based on a similar file for PBESs by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/join.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_JOIN_H
#define MCRL2_PRES_JOIN_H

#include "mcrl2/pres/pres_expression.h"



namespace mcrl2::pres_system {

/// \brief Returns or applied to the sequence of pres expressions [first, last)
/// \param first Start of a sequence of pres expressions
/// \param last End of a sequence of of pres expressions
/// \return Or applied to the sequence of pres expressions [first, last)
template <typename FwdIt>
pres_expression join_or(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, [](const pres_expression& x, const pres_expression& y) { return or_(x, y); }, false_());
}

/// \brief Returns and applied to the sequence of pres expressions [first, last)
/// \param first Start of a sequence of pres expressions
/// \param last End of a sequence of of pres expressions
/// \return And applied to the sequence of pres expressions [first, last)
template <typename FwdIt>
pres_expression join_and(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, [](const pres_expression& x, const pres_expression& y) { return and_(x, y); }, true_());
}

/// \brief Splits a disjunction into a sequence of operands
/// Given a pres expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PRES expression
/// \param split_data_expressions if true, both data and pres disjunctions are
///        split, otherwise only pres disjunctions are split.
/// \return A sequence of operands
inline
std::set<pres_expression> split_or(const pres_expression& expr, bool split_data_expressions = false)
{
  using namespace accessors;
  std::set<pres_expression> result;

  if (split_data_expressions)
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pres_expression> >(result, result.begin()), is_universal_or, data_left, data_right);
  }
  else
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pres_expression> >(result, result.begin()), is_pres_or, left, right);
  }

  return result;
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a pres expression of the form p1 && p2 && .... && pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
/// function symbol.
/// \param expr A PRES expression
/// \param split_data_expressions if true, both data and pres conjunctions are
///        split, otherwise only pres conjunctions are split.
/// \return A sequence of operands
inline
std::set<pres_expression> split_and(const pres_expression& expr, bool split_data_expressions = false)
{
  using namespace accessors;
  std::set<pres_expression> result;

  if (split_data_expressions)
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pres_expression> >(result, result.begin()), is_universal_and, data_left, data_right);
  }
  else
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pres_expression> >(result, result.begin()), is_pres_and, left, right);
  }

  return result;
}

/// \brief Returns or applied to the sequence of pres expressions [first, last)
/// \param first Start of a sequence of pres expressions
/// \param last End of a sequence of pres expressions
/// \return Or applied to the sequence of pres expressions [first, last)
template <typename FwdIt>
inline pres_expression optimized_join_or(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, 
                                 last, 
                                 [](const pres_expression& arg0, const pres_expression& arg1) -> pres_expression
                                 { 
                                   pres_expression result;
                                   data::optimized_or(result, arg0, arg1);
                                   return result;
                                 }, 
                                 false_());
}

/// \brief Returns and applied to the sequence of pres expressions [first, last)
/// \param first Start of a sequence of pres expressions
/// \param last End of a sequence of pres expressions
/// \return And applied to the sequence of pres expressions [first, last)
template <typename FwdIt>
inline pres_expression optimized_join_and(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, 
                                 last, 
                                 [](const pres_expression& arg0, const pres_expression& arg1) -> pres_expression
                                 { 
                                   pres_expression result;
                                   data::optimized_and(result, arg0, arg1);
                                   return result;
                                 }, 
                                 true_());
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_JOIN_H
