// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/precedence.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PRECEDENCE_H
#define MCRL2_PROCESS_DETAIL_PRECEDENCE_H

#include "mcrl2/data/detail/precedence.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

namespace detail {

using namespace core::detail::precedences;

// From the documentation:
// The descending order of precedence of the operators is: "|", "@", ".", { "<<", ">>" }, "->", { "||", "||_" }, "sum", "+".

inline
int precedence(const process_expression& x)
{
  if (is_choice(x))
  {
    return 0;
  }
  else if (is_sum(x)) 
  {
    return 1;
  }
  else if (is_merge(x) || is_left_merge(x)) 
  {
    return 2;
  }
  else if (is_if_then(x) || is_if_then_else(x)) 
  {
    return 3;
  }
  else if (is_bounded_init(x)) 
  {
    return 4;
  }
  else if (is_seq(x)) 
  {
    return 5;
  }
  else if (is_at(x)) 
  {
    return 6;
  }
  else if (is_sync(x)) 
  {
    return 7;
  }
  return max_precedence;
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_PRECEDENCE_H
