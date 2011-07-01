// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/precedence.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PRECEDENCE_H
#define MCRL2_PBES_DETAIL_PRECEDENCE_H

#include "mcrl2/data/precedence.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

using namespace core::detail::precedences;

// From the documentation:
// The "!" operator has the highest priority, followed by "&&" and "||", followed by "=>", followed by "forall" and "exists".
// The infix operators "&&", "||" and "=>" associate to the right. 

inline
int precedence(const pbes_expression& x)
{
  if (is_forall(x) || is_exists(x))
  {
    return 0;
  }
  else if (is_imp(x)) 
  {
    return 1;
  }
  else if (is_and(x) || is_or(x)) 
  {
    return 2;
  }
  else if (is_not(x)) 
  {
    return 3;
  }
  return max_precedence;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PRECEDENCE_H
