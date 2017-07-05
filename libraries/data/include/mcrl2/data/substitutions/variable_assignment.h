// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/variable_assignment.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_VARIABLE_ASSIGNMENT_H
#define MCRL2_DATA_SUBSTITUTIONS_VARIABLE_ASSIGNMENT_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace mcrl2 {

namespace data {

/// \brief Substitution that maps a single variable to a data expression
struct variable_assignment: public std::unary_function<variable, data_expression>
{
  typedef variable variable_type;
  typedef data_expression expression_type;

  const variable& lhs;
  const data_expression& rhs;

  variable_assignment(const variable& lhs_, const data_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  const data_expression& operator()(const variable& x) const
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

#endif // MCRL2_DATA_SUBSTITUTIONS_VARIABLE_ASSIGNMENT_H
