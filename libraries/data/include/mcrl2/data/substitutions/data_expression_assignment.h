// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/data_expression_assignment.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_DATA_EXPRESSION_ASSIGNMENT_H
#define MCRL2_DATA_SUBSTITUTIONS_DATA_EXPRESSION_ASSIGNMENT_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"

namespace mcrl2::data
{

/// \brief Substitution that maps a data expression to a data expression
struct data_expression_assignment
{
  using variable_type = data_expression;
  using expression_type = data_expression;
  using argument_type = data_expression;
  using result_type = data_expression;

  const data_expression& lhs;
  const data_expression& rhs;

  data_expression_assignment(const data_expression& lhs_, const data_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  const data_expression& operator()(const data_expression& x) const
  {
    if (x == lhs)
    {
      return rhs;
    }
    return x;
  }
};

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_DATA_EXPRESSION_ASSIGNMENT_H
