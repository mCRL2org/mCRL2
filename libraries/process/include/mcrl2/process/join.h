// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/join.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_JOIN_H
#define MCRL2_PROCESS_JOIN_H

#include "mcrl2/process/process_expression.h"

namespace mcrl2::process
{

/// \brief Splits a choice into a set of operands
/// Given a process expression of the form p1 + p2 + .... + pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a + as main
/// function symbol.
/// \param x A process expression.
/// \return A set of process expressions.
inline
std::vector<process_expression> split_summands(const process_expression& x)
{
  std::vector<process_expression> result;
  utilities::detail::split(x,
                           std::back_inserter(result),
                           is_choice,
                           [](const process_expression& x) { return atermpp::down_cast<choice>(x).left(); },
                           [](const process_expression& x) { return atermpp::down_cast<choice>(x).right(); }
                          );
  return result;
}

/// \brief Returns or applied to the sequence of process expressions [first, last).
/// \param first Start of a sequence of process expressions.
/// \param last End of a sequence of of process expressions.
/// \return The choice operator applied to the sequence of process expressions [first, last).
template<typename FwdIt>
process_expression join_summands(FwdIt first, FwdIt last)
{
  process_expression delta_ = delta();
  return utilities::detail::join(first, last, [](const process_expression& x, const process_expression& y) {
      return choice(x, y);
  }, delta_);
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_JOIN_H
