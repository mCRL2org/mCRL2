// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/data_expression_assignment.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_DATA_EXPRESSION_ASSIGNMENT_H
#define MCRL2_DATA_SUBSTITUTIONS_DATA_EXPRESSION_ASSIGNMENT_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

/// \brief Substitution that maps a data expression to a data expression
struct data_expression_assignment: public std::unary_function<data_expression, data_expression>
{
  typedef data_expression variable_type;
  typedef data_expression expression_type;

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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_DATA_EXPRESSION_ASSIGNMENT_H
