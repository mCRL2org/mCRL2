// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/sort_expression_assignment.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_SORT_EXPRESSION_ASSIGNMENT_H
#define MCRL2_DATA_SUBSTITUTIONS_SORT_EXPRESSION_ASSIGNMENT_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"

namespace mcrl2::data
{

/// \brief Substitution that maps a sort expression to a sort expression.
struct sort_expression_assignment
{
  using variable_type = sort_expression;
  using expression_type = sort_expression;

  sort_expression lhs;
  sort_expression rhs;

  sort_expression_assignment(const sort_expression& lhs_, const sort_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  const sort_expression& operator()(const sort_expression& x) const
  {
    if (x == lhs)
    {
      return rhs;
    }
    return x;
  }
};

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_SORT_EXPRESSION_ASSIGNMENT_H
